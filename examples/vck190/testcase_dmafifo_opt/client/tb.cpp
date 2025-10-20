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

//#define KERNEL_LOOP_ITERS 512 // XXX: this should be changed along with the same macro in aie/common.h
#define KERNEL_WORDS_PER_ITER 1024

int main(int argc, char** argv) {
	std::cout<<"PS version: 1.0"<<std::endl;
    std::string xclbin_path(argv[1]);
    auto num_iterations = (argc >= 3) ? atoi(argv[2]) : 1;
    auto num_repetitions = (argc >= 4) ? atoi(argv[3]) : 1;
    auto num_delay = (argc >= 5) ? atoi(argv[4]) : 0;
    auto num_values = num_iterations * KERNEL_WORDS_PER_ITER;
    
    std::cout << "Using XCLBIN file: " << xclbin_path << std::endl;

    std::vector<int> a(num_values);
    std::vector<int> s(num_values);
    std::vector<int> s2(num_values);

    for (int i = 0; i < num_values; i++) {
        a[i] = rand() % 100 - 50;
        s[i] = 0;
		s2[i] = 0;
    }

    int errorCount = 0;
    printf("Running example ADDER\n");
    printf(" - Number of graph iterations         : %8d\n", num_iterations);
    printf(" - Number of values                   : %8d (%dKB)\n", num_values, num_values * sizeof(int) / 1024);
    printf(" - Number of repetitions              : %8d\n", num_repetitions);
    printf(" - Number of graph iterations (total) : %8d\n", num_iterations * num_repetitions);
    printf(" - Channel delay                      : %8d cycles\n", num_delay);

    bool is_valid;
    // Instantiate the test harness.
    // This loads the xclbin on device 0 and creates the necessary XRT kernel handles
    test_harness_mgr_client mgr(xclbin_path, {"gr"}, "vck190");
    // configuration: channel index, size_in_bytes, repetition, delay, pointer to data
    std::vector<test_harness_args> args;
    args.push_back({channel_index(PLIO_01_TO_AIE), num_values * sizeof(int), num_repetitions, num_delay, (char*)a.data()});
    args.push_back({channel_index(PLIO_01_FROM_AIE), num_values * sizeof(int), num_repetitions, num_delay, (char*)s.data()});

    std::cout << "Testing mode: FUNC_MODE\n";
    // Run the AIE graph(s).
    // The first parameter is the index of the desired graph in the vector of graph names set during initialization.
    // If there is a single user graph, the index is 0. The second parameter is the desired number of graph
    // iterations.
    mgr.runAIEGraph(0, num_iterations * num_repetitions);

    // Start the DMA engine
    mgr.runTestHarness(FUNC_MODE, args);

    // Wait for all DMA transactions and for the AIE graph to finish.
    // The argument is an optional timeout (in millisecond) for the AIE graph.
    mgr.waitForRes(0);
    mgr.printPerf();

    // Comparing the execution data to the golden data
    {
        if (!is_valid) {
            printf("[INFO]: Result checking is not valid if test size is beyond the capacity of URAM in each channel.\n");
        } else {
            for (int i = 0; i < num_values; i++) {
                auto golden = a[i] * 2;
                // Access values in the output buffer
                if ((signed)s[i] != golden) {
                    errorCount++;
                    std::cout << "ERROR: s[" << i << "] = " << s[i] << " != " << golden << " = a[" << i << "] + b[" << i << "]" << std::endl;
                }
            }
        }

        if (errorCount)
            printf("Test failed with %d errors\n", errorCount);
        else
            printf("TEST PASSED\n");
    }
    
	args.pop_back();
	args.pop_back();
    args.push_back({channel_index(PLIO_01_TO_AIE), num_values * sizeof(int), num_repetitions, num_delay, (char*)s.data()});
    args.push_back({channel_index(PLIO_01_FROM_AIE), num_values * sizeof(int), num_repetitions, num_delay, (char*)s2.data()});
    std::cout << "Testing mode: PERF_MODE\n";
    mgr.runAIEGraph(0, num_iterations * num_repetitions);

    // Start the DMA engine

    mgr.runTestHarness(PERF_MODE, args);

    // Wait for all DMA transactions and for the AIE graph to finish.
    // The argument is an optional timeout (in millisecond) for the AIE graph.
    mgr.waitForRes(0);

    mgr.printPerf();

    // Get the validity of the result
    is_valid = mgr.isResultValid();
   

    // Comparing the execution data to the golden data
    {
        if (!is_valid) {
            printf("[INFO]: Result checking is not valid if test size is beyond the capacity of URAM in each channel.\n");
        } else {
            for (int i = 0; i < num_values; i++) {
                auto golden = a[i] * 4;
                // Access values in the output buffer
                if ((signed)s2[i] != golden) {
                    errorCount++;
                    std::cout << "ERROR: s2[" << i << "] = " << s2[i] << " != " << golden << " = a[" << i << "] * 4" << std::endl;
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

