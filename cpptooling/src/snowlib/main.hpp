
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <snowlib/diag/DiagLogger.hpp>

bool snowlibinit(std::optional<util::fs::Path> pathToLogFile = {},
    diag::LogLevel logLevel = diag::LogLevel::Default);

#ifdef TEST_ENABLED
bool testonly_snowlibinit();
#endif
