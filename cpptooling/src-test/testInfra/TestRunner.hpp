
// Copyright (c) 2025 Andrew Griffin - All Rights Reserved

#ifdef TEST_ENABLED

namespace util::cmd {
    class ArgParse;
}

namespace test {
    util::cmd::ArgParse makeArgParse();
    void bindArgParse(util::cmd::ArgParse& argparse);
}

#endif