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

#include "graph.h"
#include "test_harness_mgr.hpp"

#define MATA_SZ 32
#define MATB_SZ 32
#define MATC_SZ 16

using namespace test_harness;

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace std;

int main(int argc, char** argv) {
    std::cout << "MATA_SZ = " << MATA_SZ << std::endl;
    std::cout << "MATB_SZ = " << MATB_SZ << std::endl;
    std::cout << "MATC_SZ = " << MATC_SZ << std::endl;
    // Prepare data
    const int in_ch_a = 4;
    const int in_ch_b = 32;
    const int out_ch_c = 8;
    int16_t* in_data_a[in_ch_a];
    int16_t* in_data_b[in_ch_b];
    int16_t* out_data_c[out_ch_c];

    for (int i = 0; i < in_ch_a; i++) {
        in_data_a[i] = (int16_t*)malloc(MATA_SZ * 16);
        for (int j = 0; j < MATA_SZ * 16 / 2; j++) {
            in_data_a[i][j] = 1;
        }
    }
    for (int i = 0; i < in_ch_b; i++) {
        in_data_b[i] = (int16_t*)malloc(MATB_SZ * 16);
        for (int j = 0; j < MATB_SZ * 16 / 2; j++) {
            in_data_b[i][j] = 2;
        }
    }
    for (int i = 0; i < out_ch_c; i++) {
        out_data_c[i] = (int16_t*)malloc(MATC_SZ * 16);
        for (int j = 0; j < MATC_SZ * 16 / 2; j++) {
            out_data_c[i][j] = 0;
        }
    }

    // run test with test harness
    test_harness_mgr<36, 16, 4096> mgr(0, argv[1], {"vck190_test_harness_perf"}, {"g"}, REP_MODE, "vck190");
    std::vector<test_harness_args> args;
    args.push_back({channel_index(PLIO_01_TO_AIE), MATA_SZ * 16, 1, 0, 0, 0, (char*)in_data_a[0]});
    args.push_back({channel_index(PLIO_02_TO_AIE), MATA_SZ * 16, 1, 0, 0, 0, (char*)in_data_a[1]});
    args.push_back({channel_index(PLIO_03_TO_AIE), MATA_SZ * 16, 1, 0, 0, 0, (char*)in_data_a[2]});
    args.push_back({channel_index(PLIO_04_TO_AIE), MATA_SZ * 16, 1, 0, 0, 0, (char*)in_data_a[3]});

    args.push_back({channel_index(PLIO_05_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[0]});
    args.push_back({channel_index(PLIO_06_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[1]});
    args.push_back({channel_index(PLIO_07_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[2]});
    args.push_back({channel_index(PLIO_08_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[3]});
    args.push_back({channel_index(PLIO_09_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[4]});
    args.push_back({channel_index(PLIO_10_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[5]});
    args.push_back({channel_index(PLIO_11_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[6]});
    args.push_back({channel_index(PLIO_12_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[7]});
    args.push_back({channel_index(PLIO_13_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[8]});
    args.push_back({channel_index(PLIO_14_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[9]});
    args.push_back({channel_index(PLIO_15_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[10]});
    args.push_back({channel_index(PLIO_16_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[11]});
    args.push_back({channel_index(PLIO_17_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[12]});
    args.push_back({channel_index(PLIO_18_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[13]});
    args.push_back({channel_index(PLIO_19_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[14]});
    args.push_back({channel_index(PLIO_20_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[15]});
    args.push_back({channel_index(PLIO_21_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[16]});
    args.push_back({channel_index(PLIO_22_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[17]});
    args.push_back({channel_index(PLIO_23_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[18]});
    args.push_back({channel_index(PLIO_24_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[19]});
    args.push_back({channel_index(PLIO_25_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[20]});
    args.push_back({channel_index(PLIO_26_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[21]});
    args.push_back({channel_index(PLIO_27_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[22]});
    args.push_back({channel_index(PLIO_28_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[23]});
    args.push_back({channel_index(PLIO_29_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[24]});
    args.push_back({channel_index(PLIO_30_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[25]});
    args.push_back({channel_index(PLIO_31_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[26]});
    args.push_back({channel_index(PLIO_32_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[27]});
    args.push_back({channel_index(PLIO_33_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[28]});
    args.push_back({channel_index(PLIO_34_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[29]});
    args.push_back({channel_index(PLIO_35_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[30]});
    args.push_back({channel_index(PLIO_36_TO_AIE), MATB_SZ * 16, 1, 0, 0, 0, (char*)in_data_b[31]});

    args.push_back({channel_index(PLIO_01_FROM_AIE), MATC_SZ * 16, 1, 0, 0, 0, (char*)out_data_c[0]});
    args.push_back({channel_index(PLIO_02_FROM_AIE), MATC_SZ * 16, 1, 0, 0, 0, (char*)out_data_c[1]});
    args.push_back({channel_index(PLIO_03_FROM_AIE), MATC_SZ * 16, 1, 0, 0, 0, (char*)out_data_c[2]});
    args.push_back({channel_index(PLIO_04_FROM_AIE), MATC_SZ * 16, 1, 0, 0, 0, (char*)out_data_c[3]});
    args.push_back({channel_index(PLIO_05_FROM_AIE), MATC_SZ * 16, 1, 0, 0, 0, (char*)out_data_c[4]});
    args.push_back({channel_index(PLIO_06_FROM_AIE), MATC_SZ * 16, 1, 0, 0, 0, (char*)out_data_c[5]});
    args.push_back({channel_index(PLIO_07_FROM_AIE), MATC_SZ * 16, 1, 0, 0, 0, (char*)out_data_c[6]});
    args.push_back({channel_index(PLIO_08_FROM_AIE), MATC_SZ * 16, 1, 0, 0, 0, (char*)out_data_c[7]});

    mgr.runAIEGraph(0, 8);
    mgr.runTestHarness(args);
    mgr.waitForRes(10000);

    // checking result
    int nerror = 0;
    for (int i = 0; i < out_ch_c; i++) {
        for (int j = 0; j < MATC_SZ * 16 / 2; j++) {
            if (out_data_c[i][j] != 64) {
                nerror++;
            }
        }
    }
    if (nerror) {
        printf("Test failed with %d errors\n", nerror);
    } else {
        printf("TEST PASSED\n");
    }

    // free buffers
    for (int i = 0; i < in_ch_a; i++) {
        free(in_data_a[i]);
    }
    for (int i = 0; i < in_ch_b; i++) {
        free(in_data_b[i]);
    }
    for (int i = 0; i < out_ch_c; i++) {
        free(out_data_c[i]);
    }

    return nerror;
}
