
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <util/FileSystem.hpp>
#include <util/SpinLock.hpp>

namespace diag {
    enum class LogLevel {
        ErrorsOnly,
        Default,
        Verbose
    };

    // Windowed logger, also logs to file
    class DiagLogger {
        struct Progress {
            size_t mFailed = 0;
            size_t mTotal = 0;
            size_t mCurrent = 0;
            std::string mMessage;
        };

        std::unique_ptr<util::fs::WriteCharFileStream> mLogFileStream;
        util::SpinLock mLock;
        LogLevel mLogLevel = LogLevel::Default;
        std::optional<Progress> mProgress;
        bool mHasProgressWritten = false;

        // Go up 3 lines, /r, write message, /n
        // erase line, /n
        // erase line, write progress message, /n
        // write progress, /n
        void _writeLog(const std::string& message, bool isError);
        void _log(const std::string& message, bool isError);

        // Just write to log file directly
        void _writeFile(const std::string& message);

        // test helper
        bool _isEnabled() const;

        std::string _buildProgressLoader();
        void _rewriteProgress(); // called when progress is initialized or message changes, only rewrites the progress section

    public:
        DiagLogger(std::optional<util::fs::Path> logFile, LogLevel logLevel = LogLevel::Default);

        // note that (current + failed) <= total or the message might be odd

        void incProgress(); // does nothing if progress not initialized
        void incFailedProgress(); // does nothing if progress not initialized
        void initProgress(size_t total, const std::string& message); // resets progress
        void initProgress(size_t total, const std::string& message, size_t current, size_t failed); // resets progress to these values
        void clearProgress();
        void setProgressMessage(const std::string& message); // does nothing if progress not initialized

        void writeError(const std::string& message); // or warning
        void writeVerbose(const std::string& message);
        void writeInfo(const std::string& message);

#ifdef TEST_ENABLED
        DiagLogger(bool enabledForTest = false, LogLevel logLevel = LogLevel::Default);
    private:
        bool mEnabled = true;
    public:
#endif
    };
}