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
#include "test.hpp"

xf::dsp::aie::testcase::test_graph ddsMix;

// REQUIRED: Instantiate a graph to occupy the PLIOs unused by the user graph.
static std::vector<std::string> cust_in;
static std::vector<std::string> cust_out = {"PLIO_02_FROM_AIE"};
test_harness::occupyUnusedPLIO<0, 1, 16> unusedPLIOs(cust_in, cust_out);

#if defined(__AIESIM__) || defined(__X86SIM__)
int main(void) {
    printf("\n");
    printf("========================\n");
    printf("TEST.CPP STARTED\n");
    printf("UUT: ");
    printf(QUOTE(UUT_GRAPH));
    printf("\n");
    printf("========================\n");
    printf("Input stimulus file");
    printf(QUOTE(INPUT_FILE));
    printf("\n");
    printf("Input stimulus file 2");
    printf(QUOTE(INPUT_FILE2));
    printf("\n");
    printf("Input samples   = %d \n", INPUT_SAMPLES);
    printf("Output samples  = %d \n", OUTPUT_SAMPLES);
    printf("Data type       = ");
    printf(QUOTE(DATA_TYPE));
    printf("\n");
    printf("TEST.CPP PRINT FINISHED \n");
    printf("\n");

    ddsMix.init();

    ddsMix.run(NITER);

    ddsMix.end();

    printf("TEST.CPP IS FINISHED \n");
    printf("========================\n");
    printf("========================\n");

    return 0;
}
#endif
