
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/main.hpp>

#include <snowlib/diag/Desc.hpp>

bool snowlibinit() {
    diag::Desc::init();

    return true;
}
