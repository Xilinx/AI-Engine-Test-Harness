/*
 * MIT License
 *
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
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

#include "test_harness_mgr.hpp"

using namespace test_harness;

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace std;

int main(int argc, char** argv) {
    // Prepare data
    const int in_ch = 1;
    const int out_ch = 1;
    int in_sz = 512 * 4 * 4;
    int out_sz = 512 * 4 * 4;
    char* in_data[in_ch];
    char* out_data[out_ch];
    // PERF_MODE by default, please take adder case as example for how to use FUNC_MODE
    uint64_t test_mode = PERF_MODE;
    if ((test_mode != FUNC_MODE) && (test_mode != PERF_MODE)) {
        std::cout << "Only FUNC_MODE & PERF_MODE are supported by AIE test harness on VCK190.\n";
        exit(1);
    }

    std::string xclbin_path(argv[1]);
    if (test_mode == FUNC_MODE) {
        xclbin_path.insert(xclbin_path.find(".xclbin"), "_func");
        std::cout << "Testing mode: FUNC_MODE\n";
    } else {
        xclbin_path.insert(xclbin_path.find(".xclbin"), "_perf");
        std::cout << "Testing mode: PERF_MODE\n";
    }
    std::cout << "Using XCLBIN file: " << xclbin_path << std::endl;

    for (int i = 0; i < in_ch; i++) {
        in_data[i] = (char*)malloc(in_sz);
    }
    for (int i = 0; i < out_ch; i++) {
        out_data[i] = (char*)malloc(out_sz);
    }
    // run test with test harness
    test_harness_mgr<36, 16, 4096> mgr(0, xclbin_path, {"G"});
    std::vector<test_harness_args> args;
    args.push_back({channel_index(PLIO_01_TO_AIE), in_sz, 1, 0, (char*)in_data[0]});
    args.push_back({channel_index(PLIO_02_FROM_AIE), out_sz, 1, 0, (char*)out_data[0]});
    mgr.runAIEGraph(0, 4);
    mgr.runTestHarness(args);
    mgr.waitForRes(10000);
    bool is_valid = mgr.result_valid;

    //
    for (int i = 0; i < in_ch; i++) {
        free(in_data[i]);
    }
    for (int i = 0; i < out_ch; i++) {
        free(out_data[i]);
    }

    return 0;
}
