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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fstream>
#include <cstring>

#include "common.h"

// The test harness APIs
#include "vck190_test_harness_mgr.hpp"
using namespace vck190_test_harness;

// Utilities (read_data_from_file, check_size...)
#include "utils.hpp"

#define KERNEL_LOOP_ITERS 512
#define KERNEL_WORDS_PER_ITER 4

int main(int argc, char** argv) {
    auto num_iterations = (argc >= 3) ? atoi(argv[2]) : 1;
    auto num_repetitions = (argc >= 4) ? atoi(argv[3]) : 1;
    auto num_delay = (argc >= 5) ? atoi(argv[4]) : 0;
    auto num_values = num_iterations * KERNEL_LOOP_ITERS * KERNEL_WORDS_PER_ITER;

    std::vector<int> a(num_values);
    std::vector<int> b(num_values);
    std::vector<int> s(num_values);

    check_size("a", a);
    check_size("b", b);
    check_size("s", s);

    for (int i = 0; i < num_values; i++) {
        a[i] = rand() % 100 - 50;
        b[i] = rand() % 100 - 50;
        s[i] = 0;
    }

    // Instantiate the test harness.
    // This loads the xclbin on device 0 and creates the necessary XRT kernel handles
    test_harness_mgr mgr(0, argv[1], {"gr"});

    // configuration: channel index, size_in_bytes, repetition, delay, pointer to data
    std::vector<test_harness_args> args;
    args.push_back(
        {channel_index(Column_12_TO_AIE), num_values * sizeof(int), num_repetitions, num_delay, (char*)a.data()});
    args.push_back(
        {channel_index(Column_13_TO_AIE), num_values * sizeof(int), num_repetitions, num_delay, (char*)b.data()});
    args.push_back(
        {channel_index(Column_28_FROM_AIE), num_values * sizeof(int), num_repetitions, num_delay, (char*)s.data()});

    printf("Running example ADDER\n");
    printf(" - Number of graph iterations         : %8d\n", num_iterations);
    printf(" - Number of values                   : %8d (%dKB)\n", num_values, num_values * sizeof(int) / 1024);
    printf(" - Number of repetitions              : %8d\n", num_repetitions);
    printf(" - Number of graph iterations (total) : %8d\n", num_iterations * num_repetitions);
    printf(" - Channel delay                      : %8d cycles\n", num_delay);

    // Run the AIE graph(s).
    // The first parameter is the index of the desired graph in the vector of graph names set during initialization.
    // If there is a single user graph, the index is 0. The second parameter is the desired number of graph iterations.
    mgr.runAIEGraph(0, num_iterations * num_repetitions);

    // Start the DMA engine
    mgr.runTestHarness(args);

    // Wait for all DMA transactions and for the AIE graph to finish.
    // The argument is an optional timeout (in millisecond) for the AIE graph.
    mgr.waitForRes(0);

    // Comparing the execution data to the golden data
    int errorCount = 0;
    {
        for (int i = 0; i < num_values; i++) {
            auto golden = a[i] + b[i];
            // Access values in the output buffer
            if ((signed)s[i] != golden) {
                // printf("Error found @ %4d, %4d != %4d (%4d + %4d)\n", i, s[i], golden, a[i], b[i]);
                errorCount++;
            }
        }

        if (errorCount)
            printf("Test failed with %d errors\n", errorCount);
        else
            printf("TEST PASSED\n");
    }

    return errorCount;
}
