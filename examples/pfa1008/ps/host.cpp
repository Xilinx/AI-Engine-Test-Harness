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

//#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
//#include <fstream>

#include "vck190_test_harness_mgr.hpp"

// #define DATA_SIZE 16

using namespace vck190_test_harness;

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace std;

int main(int argc, char** argv) {
    // Prepare data
    const int in_ch = 6;
    const int out_ch = 6;
    int in_sz = 1008 * 16; // total number of elements * sizeof(int128)
    int out_sz = 1008 * 16; // total number of elements * sizeof(int128)
    char* in_data[in_ch];
    char* out_data[out_ch];

    for (int i = 0; i < in_ch; i++) {
        in_data[i] = (char*)malloc(in_sz);
    }
    for (int i = 0; i < out_ch; i++) {
        out_data[i] = (char*)malloc(out_sz);
    }
    // run test with test harness
    test_harness_mgr mgr(0, argv[1], {"aie_dut"});
    std::vector<test_harness_args> args;
    args.push_back({channel_index(PLIO_01_TO_AIE), in_sz, 1, 0, (char*)in_data[0]});
    args.push_back({channel_index(PLIO_03_TO_AIE), in_sz, 1, 0, (char*)in_data[1]});
    args.push_back({channel_index(PLIO_05_TO_AIE), in_sz, 1, 0, (char*)in_data[2]});
    args.push_back({channel_index(PLIO_07_TO_AIE), in_sz, 1, 0, (char*)in_data[3]});
    args.push_back({channel_index(PLIO_09_TO_AIE), in_sz, 1, 0, (char*)in_data[4]});
    args.push_back({channel_index(PLIO_11_TO_AIE), in_sz, 1, 0, (char*)in_data[5]});
    args.push_back({channel_index(PLIO_02_FROM_AIE), out_sz, 1, 0, (char*)out_data[0]});
    args.push_back({channel_index(PLIO_04_FROM_AIE), out_sz, 1, 0, (char*)out_data[1]});
    args.push_back({channel_index(PLIO_06_FROM_AIE), out_sz, 1, 0, (char*)out_data[2]});
    args.push_back({channel_index(PLIO_08_FROM_AIE), out_sz, 1, 0, (char*)out_data[3]});
    args.push_back({channel_index(PLIO_10_FROM_AIE), out_sz, 1, 0, (char*)out_data[4]});
    args.push_back({channel_index(PLIO_12_FROM_AIE), out_sz, 1, 0, (char*)out_data[5]});
    mgr.runAIEGraph(0, 8);
    mgr.runTestHarness(args);
    mgr.waitForRes(10000);

    //
    for (int i = 0; i < in_ch; i++) {
        free(in_data[i]);
    }
    for (int i = 0; i < out_ch; i++) {
        free(out_data[i]);
    }

    return 0;
}
