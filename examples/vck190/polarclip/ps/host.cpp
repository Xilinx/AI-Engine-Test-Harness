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
#include <complex>

// Properties of the AIE graph
#include "include.h"

// The test harness APIs
#include "test_harness_mgr.hpp"
using namespace test_harness;

// Utilities (read_data_from_file, check_size...)
#include "utils.hpp"

typedef std::complex<int16_t> cint16_t;

int main(int argc, char** argv) {
    auto num_repetitions = (argc >= 3) ? atoi(argv[2]) : 1;
    auto num_delay = (argc >= 4) ? atoi(argv[3]) : 0;
    // PERF_MODE by default, please take adder case as example for how to use FUNC_MODE
    uint64_t test_mode = PERF_MODE;
    if ((test_mode != FUNC_MODE) && (test_mode != PERF_MODE)) {
        std::cout << "Only FUNC_MODE & PERF_MODE are supported by AIE test harness on VCK190.\n";
        exit(1);
    }

    std::vector<cint16_t> inputs;
    std::vector<int32_t> outputs;
    std::vector<int32_t> golden;

    std::string xclbin_path(argv[1]);
    if (test_mode == FUNC_MODE) {
        xclbin_path.insert(xclbin_path.find(".xclbin"), "_func");
        std::cout << "Testing mode: FUNC_MODE\n";
    } else {
        xclbin_path.insert(xclbin_path.find(".xclbin"), "_perf");
        std::cout << "Testing mode: PERF_MODE\n";
    }
    std::cout << "Using XCLBIN file: " << xclbin_path << std::endl;

    read_data_from_file("./input.txt", inputs);
    read_data_from_file("./golden_32iters.txt", golden);
    outputs.resize(golden.size());

    auto num_inputs = inputs.size();
    auto num_outputs = outputs.size();

    // Make sure the number of samples agree with what the graph expects.
    // One graph iteration requires INTERPOLATOR27_INPUT_SAMPLES cint16 inputs and
    // produces CLASSIFIER_OUTPUT_SAMPLES int32 outputs.
    // These values are defined in src/aie/include.h
    if (num_outputs % CLASSIFIER_OUTPUT_SAMPLES != 0) {
        printf("Error: number of ouputs (%d) must be a multiple of CLASSIFIER_OUTPUT_SAMPLES (%d)\n", num_outputs,
               CLASSIFIER_OUTPUT_SAMPLES);
        return -1;
    }
    if (num_inputs % INTERPOLATOR27_INPUT_SAMPLES != 0) {
        printf("Error: number of inputs (%d) must be a multiple of INTERPOLATOR27_INPUT_SAMPLES (%d)\n", num_inputs,
               INTERPOLATOR27_INPUT_SAMPLES);
        return -1;
    }

    auto num_iterations = num_outputs / CLASSIFIER_OUTPUT_SAMPLES;

    check_size("Input", inputs);
    check_size("Output", outputs);

    // Instantiate the test harness and load the xclbin on device 0
    test_harness_mgr<36, 16, 4096> mgr(0, xclbin_path, {"clipgraph"});

    // Configuration: channel index, size_in_bytes, repetition, delay, pointer to data
    std::vector<test_harness_args> args;
    args.push_back(
        {channel_index(PLIO_01_TO_AIE), num_bytes(inputs), num_repetitions, num_delay, (char*)inputs.data()});
    args.push_back(
        {channel_index(PLIO_01_FROM_AIE), num_bytes(outputs), num_repetitions, num_delay, (char*)outputs.data()});

    printf("Running example POLARCLIP\n");
    printf(" - Number of graph iterations         : %8d\n", num_iterations);
    printf(" - Number of input values             : %8d (%dKB)\n", num_inputs, num_bytes(inputs) / 1024);
    printf(" - Number of ouput values             : %8d (%dKB)\n", num_outputs, num_bytes(outputs) / 1024);
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

    // Get validity of the result
    bool is_valid = mgr.result_valid;

    int errorCount = 0;
    if (is_valid) {
        for (int i = 0; i < num_outputs; i++) {
            if (outputs[i] != golden[i]) {
                // printf("Error found @ %d, %d != %d\n", i, outputs[i], golden[i]);
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
