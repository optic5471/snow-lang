
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <snowlib/diag/DiagLogger.hpp>
#include <snowlib/SnowArgs.hpp>

bool snowlibinit(SnowArgs args, std::optional<util::fs::Path> pathToLogFile = {});

#ifdef TEST_ENABLED
bool testonly_snowlibinit();
#endif
