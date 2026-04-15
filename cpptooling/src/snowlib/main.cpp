
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/main.hpp>

#include <snowlib/diag/Desc.hpp>
#include <snowlib/diag/DiagLogger.hpp>
#include <snowlib/src/Project.hpp>
#include <snowlib/src/SourceVault.hpp>
#include <util/ServiceLocator.hpp>

template<>
src::SourceVault* ServiceLocator<src::SourceVault>::mService = nullptr;
template<>
diag::DiagLogger* ServiceLocator<diag::DiagLogger>::mService = nullptr;

//template<>
//src::Project* ServiceLocator<src::Project>::mService = nullptr;

bool snowlibinit(
    std::optional<util::fs::Path> pathToLogFile,
    diag::LogLevel logLevel) {

    diag::Desc::init();
    ServiceLocator<src::SourceVault>::init();
    ServiceLocator<diag::DiagLogger>::init(pathToLogFile, logLevel);
    //ServiceLocator<src::Project>::init();
    return true;
}

#ifdef TEST_ENABLED
bool testonly_snowlibinit() {
    diag::Desc::init();
    ServiceLocator<src::SourceVault>::init();
    ServiceLocator<diag::DiagLogger>::init();
    //ServiceLocator<src::Project>::init();
    return true;
}
#endif
