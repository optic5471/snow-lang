
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/main.hpp>

#include <snowlib/diag/Desc.hpp>
#include <snowlib/diag/DiagLogger.hpp>
#include <snowlib/src/Project.hpp>
#include <snowlib/src/SourceVault.hpp>
#include <util/ServiceLocator.hpp>

bool snowlibinit(SnowArgs args,
    std::optional<util::fs::Path> pathToLogFile) {

    diag::Desc::init();
    ServiceLocator<src::SourceVault>::init();
    ServiceLocator<diag::DiagLogger>::init(pathToLogFile, args.logLevel);
    ServiceLocator<SnowArgs>::init(std::move(args));
    //ServiceLocator<src::Project>::init();
    return true;
}

#ifdef TEST_ENABLED
bool testonly_snowlibinit() {
    diag::Desc::init();
    return true;
}
#endif
