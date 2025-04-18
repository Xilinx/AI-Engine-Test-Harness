/*
 * MIT License
 *
 * Copyright (C) 2024-2025 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the “Software”), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or
 * otherwise to promote the sale, use or other dealings in this Software without prior written authorization from
 * Advanced Micro Devices, Inc.
 */

#ifndef __TEST_HARNESS_MAIN_HPP__
#define __TEST_HARNESS_MAIN_HPP__

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "test_harness_server.hpp"

// The test harness APIs

namespace test_harness {

namespace fs = std::filesystem;
class test_harness_main {
public:
    test_harness_main(test_harness_server* server);
    ~test_harness_main() = default;
    void run();

    void setSessionExecPath(const fs::path& path);
    fs::path createSessionPath();
    void syncWorkDir(const fs::path& sessPath);
    void runSession();

    void setResetFlag(bool flag);
    std::vector<fs::path> getXrtConfigs();
    void setTimeOut(uint64_t timeOut);

private:
    test_harness_server* server;
    fs::path sessionExecPath;
    bool mResetFlag;
    int mSessionId;
    uint64_t mSessionTimeOut;
};

} // namespace test_harness


#endif // __TEST_HARNESS_SERVER_HPP__