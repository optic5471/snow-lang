
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#include <snowlib/main.hpp>

#include <snowlib/diag/Desc.hpp>
#include <snowlib/ServiceLocator.hpp>
#include <snowlib/src/Project.hpp>
#include <snowlib/src/SourceVault.hpp>

template<>
src::SourceVault* ServiceLocator<src::SourceVault>::mService = nullptr;

template<>
src::Project* ServiceLocator<src::Project>::mService = nullptr;

bool snowlibinit() {
    diag::Desc::init();
    ServiceLocator<src::SourceVault>::init();
    ServiceLocator<src::Project>::init();
    return true;
}
