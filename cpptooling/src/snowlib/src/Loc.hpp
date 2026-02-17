
// Copyright (c) 2026 Andrew Griffin - All Rights Reserved

#pragma once

#include <stdint.h>

namespace src {
    using FileID = uint32_t;

    struct Loc {
        FileID mFileID;
        uint32_t mOffset; // We only support files up to 4GB in size
    };

    struct LocRange {
        FileID mFileID;
        uint32_t mOffset; // We only support files up to 4GB in size
        uint32_t mLength;

        operator Loc () const {
            return { mFileID, mOffset };
        }
    };
}
