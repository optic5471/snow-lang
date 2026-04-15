
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/diag/DiagLogger.hpp>

#include <util/Assertions.hpp>
#include <util/cmd/manip.hpp>

namespace diag {
    DiagLogger::DiagLogger(
        std::optional<util::fs::Path> logFile,
        LogLevel level)
        : mLogLevel(level) {

        if (logFile && !logFile->empty()) {
            mLogFileStream = util::fs::openTextFileWriter(logFile.value());
            writeVerbose(util::format("Log file opened at: '${}'", logFile.value().str()));
            if (!mLogFileStream || !mLogFileStream->isOpen()) {
                writeError("Failed to open log file. No log file will be used.");
                mLogFileStream.reset();
            }
        }
        else {
            writeVerbose("Log file path not set, no file will be made.");
        }
    }

#ifdef TEST_ENABLED
    DiagLogger::DiagLogger(bool enabledForTest, LogLevel logLevel)
        : mLogLevel(logLevel)
        , mEnabled(enabledForTest) {
    }
#endif

    void DiagLogger::incProgress() {
        auto lock = mLock.lock();
        if (mProgress) {
            mProgress->mCurrent++;
            _rewriteProgress();
        }
    }

    void DiagLogger::incFailedProgress() {
        auto lock = mLock.lock();
        if (mProgress) {
            mProgress->mFailed++;
            _rewriteProgress();
        }
    }

    void DiagLogger::setProgressMessage(const std::string& message) {
        auto lock = mLock.lock();
        if  (mProgress) {
            mProgress->mMessage = message;
            _rewriteProgress();
        }
    }

    void DiagLogger::initProgress(size_t total, const std::string& message) {
        auto lock = mLock.lock();
        if (total == 0) {
            DEBUG_FAIL("Total must be more than 0");
            total = 1;
        }
        mProgress = Progress{
            .mFailed=0,
            .mTotal=total,
            .mCurrent=0,
            .mMessage=message
        };
        _rewriteProgress();
    }

    void DiagLogger::initProgress(size_t total, const std::string& message, size_t current, size_t failed) {
        auto lock = mLock.lock();
        if (total == 0) {
            DEBUG_FAIL("Total must be more than 0");
            total = 1;
        }
        mProgress = Progress{
            .mFailed=failed,
            .mTotal=total,
            .mCurrent=current,
            .mMessage=message
        };
        _rewriteProgress();
    }

    void DiagLogger::clearProgress() {
        auto lock = mLock.lock();
        mProgress.reset();
        _rewriteProgress();
    }

    void DiagLogger::writeError(const std::string& message) {
        auto lock = mLock.lock();
        _writeLog(message, true);
        _writeFile(message);
    }

    void DiagLogger::writeInfo(const std::string& message) {
        auto lock = mLock.lock();
        if (mLogLevel != LogLevel::ErrorsOnly) {
            _writeLog(message, false);
        }
        _writeFile(message);
    }

    void DiagLogger::writeVerbose(const std::string& message) {
        auto lock = mLock.lock();
        if (mLogLevel == LogLevel::Verbose) {
            _writeLog(message, false);
            _writeFile(message);
        }
    }

    void DiagLogger::_log(const std::string& message, bool isError) {
        if (isError) {
            Log::error(message);
        }
        else {
            Log::log(message);
        }
    }

    void DiagLogger::_rewriteProgress() {
        if (_isEnabled()) {
            // rewrite
            if (mHasProgressWritten && mProgress.has_value()) {
                _log(util::format("${}\r${}${}\n${}\r${}",
                    util::cmd::manip::cursorPrevLine(2),
                    util::cmd::manip::clearLine(),
                    mProgress->mMessage,
                    util::cmd::manip::clearLine(),
                    _buildProgressLoader()
                    ), false);
            }
            // write new progress
            else if (!mHasProgressWritten && mProgress.has_value()) {
                _log(util::format("\n${}\n${}",
                    mProgress->mMessage,
                    _buildProgressLoader()
                    ), false);
                mHasProgressWritten = true;
            }
            // clear progress
            else if (mHasProgressWritten && !mProgress.has_value()) {
                _log(util::format("${}${}${}${}${}\r",
                    util::cmd::manip::cursorPrevLine(),
                    util::cmd::manip::clearLine(),
                    util::cmd::manip::cursorPrevLine(),
                    util::cmd::manip::clearLine(),
                    util::cmd::manip::cursorPrevLine()
                    ), false);
                mHasProgressWritten = false;
            }
            // do nothing, already cleared and not adding new progress
        }
    }

    void DiagLogger::_writeLog(const std::string& message, bool isError) {
        if (_isEnabled()) {
            // write both
            if (mHasProgressWritten && mProgress.has_value()) {
                _log(util::format("${}\r${}\n${}\n${}\r${}\n${}",
                    util::cmd::manip::cursorPrevLine(3),
                    message,
                    util::cmd::manip::clearLine(),
                    util::cmd::manip::clearLine(),
                    mProgress->mMessage,
                    _buildProgressLoader()
                    ), isError);
            }
            // dont move up to write progress
            else if (!mHasProgressWritten && mProgress.has_value()) {
                _log(util::format("${}\n\n${}\n${}",
                    message,
                    mProgress->mMessage,
                    _buildProgressLoader()
                    ), isError);
                mHasProgressWritten = true;
            }
            // erase progress and write message
            else if (mHasProgressWritten && !mProgress.has_value()) {
                _log(util::format("${}${}${}${}\r${}",
                    util::cmd::manip::cursorPrevLine(),
                    util::cmd::manip::clearLine(),
                    util::cmd::manip::cursorPrevLine(),
                    util::cmd::manip::clearLine(),
                    message
                    ), isError);
                mHasProgressWritten = false;
            }
            // just write message
            else {
                _log(util::format("${}", message), isError);
            }
        }
    }

    void DiagLogger::_writeFile(const std::string& message) {
        if (_isEnabled() && mLogFileStream) {
            if (!mLogFileStream->isOpen()) {
                _writeLog("Log file error, file is no longer open. Stopping log to file", true);
                mLogFileStream.reset();
                return;
            }

            if (!mLogFileStream->writeLine(message)) {
                DEBUG_FAIL("Log file write error");
            }
            if (!mLogFileStream->flush()) {
                DEBUG_FAIL("Log file flush error");
            }
        }
    }

    bool DiagLogger::_isEnabled() const {
#ifdef TEST_ENABLED
        return mEnabled;
#else
        return true;
#endif
    }

    std::string DiagLogger::_buildProgressLoader() {
        DEBUG_ASSERT(mProgress, "Dont call when no progress exists");

        std::string percentDone;
        float percent =
            static_cast<float>(mProgress->mCurrent + mProgress->mFailed) / static_cast<float>(mProgress->mTotal);
        if (percent >= 1.0f) {
            percentDone = "100";
        }
        else if (percent < 10.0f) {
            percentDone = util::format("  ${}", static_cast<int>(percent));
        }
        else {
            percentDone = util::format(" ${}", static_cast<int>(percent));
        }

        std::string progress;
        size_t v = static_cast<size_t>(percent * 25.0f);
        v = std::min(v, (size_t)25);
        for (size_t i = 0; i < v; ++i) {
            progress.push_back('*');
        }
        for (size_t i = v; i < 25; ++i) {
            progress.push_back('-');
        }

        if (mProgress->mFailed > 0) {
            return util::format("${}/${} (${}% - ${} Failed) [${}]",
                mProgress->mCurrent + mProgress->mFailed,
                mProgress->mTotal,
                percentDone,
                mProgress->mFailed,
                progress
            );
        }
        else {
            return util::format("${}/${} (${}%) [${}]",
                mProgress->mCurrent,
                mProgress->mTotal,
                percentDone,
                progress
            );
        }
    }
}
