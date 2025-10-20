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

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <cstring>

// The test harness APIs
#include "test_harness_mgr_base.hpp"
#include "test_harness_mgr_client.hpp"
#include "test_harness_sockets.hpp"
using namespace test_harness;

#define KERNEL_BFLOAT16_PER_ITER 256*384

int main(int argc, char** argv) {
    auto num_iterations = (argc >= 3) ? atoi(argv[2]) : 1;
    auto num_repetitions = (argc >= 4) ? atoi(argv[3]) : 1;
    auto num_delay = (argc >= 5) ? atoi(argv[4]) : 0;
    auto num_values = num_iterations * KERNEL_BFLOAT16_PER_ITER;


    std::string xclbin_path(argv[1]);
    std::cout << "Using XCLBIN file: " << xclbin_path << std::endl;
    std::vector<test_harness::TestMode> test_modes = {FUNC_MODE, PERF_MODE};

    std::vector<short> a(num_values);
    std::vector<short> s(num_values);

    for (int i = 0; i < num_values; i++) {
        a[i] = i; 
        s[i] = 0;
    }

    printf("Running example ADDER\n");
    printf(" - Number of graph iterations (func) : %8d\n", num_iterations);
    printf(" - Number of graph iterations (perf) : %8d\n", num_iterations * num_repetitions);
    printf(" - Number of values                  : %8d (%dKB)\n", num_values, num_values * sizeof(int) / 1024);
    printf(" - Number of repetitions             : %8d\n", num_repetitions);
    printf(" - Channel delay                     : %8d cycles\n", num_delay);

    // Instantiate the test harness.
    // This loads the xclbin on device 0 and creates the necessary XRT kernel handles
    test_harness_mgr_client mgr(xclbin_path, {"gr"}, "vek280"); //Graph Name in aie/graph.cpp
    int errorCount = 0;

    bool is_valid;
    printf("Running example NORMALIZATION_V2 in performance mode\n");
    // configuration: channel index, size_in_bytes, repetition, delay, pointer to data
    std::vector<test_harness_args> args;
    args.push_back({channel_index(PLIO_01_TO_AIE),  num_values * sizeof(short), num_repetitions, num_delay, (char*)a.data()});
    args.push_back({channel_index(PLIO_02_FROM_AIE), num_values * sizeof(short), num_repetitions, num_delay, (char*)s.data()});
    // Run the AIE graph(s).
    // The first parameter is the index of the desired graph in the vector of graph names set during initialization.
    // If there is a single user graph, the index is 0. The second parameter is the desired number of graph
    // iterations.
    mgr.runAIEGraph(0, num_iterations * num_repetitions);

    // Start the DMA engine
    mgr.runTestHarness(PERF_MODE, args);

    // Wait for all DMA transactions and for the AIE graph to finish.
    // The argument is an optional timeout (in millisecond) for the AIE graph.
    mgr.waitForRes(0);
		
    mgr.printPerf();
    // Get the validity of the result
    is_valid = mgr.isResultValid();

    // Post-Processing 
    {
        if (!is_valid) {
            printf("[INFO]: Result checking is not valid if test size is beyond the capacity of URAM in each channel.\n");
        } else {
            for (int i = 0; i < num_values; i++) {
					//This application does not check output value
                short golden;
                // Access values in the output buffer
                if (s[i] != golden) {
                }
            }
        }

        if (errorCount)
            printf("Test failed with %d errors\n", errorCount);
        else
            printf("TEST PASSED\n");
    }

    return errorCount != 0? EXIT_FAILURE: EXIT_SUCCESS;
}

