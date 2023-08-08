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

#include "m16_ssr8_graph.h"

// Instantiate AIE graph:
m16_ssr8_graph aie_dut;

// REQUIRED: Instantiate a graph to occupy the PLIOs unused by the user graph.
static std::vector<std::string> cust_in = {"PLIO_01_TO_AIE", "PLIO_03_TO_AIE", "PLIO_05_TO_AIE", "PLIO_07_TO_AIE",
                                           "PLIO_09_TO_AIE", "PLIO_11_TO_AIE", "PLIO_13_TO_AIE", "PLIO_15_TO_AIE",
                                           "PLIO_17_TO_AIE", "PLIO_19_TO_AIE", "PLIO_21_TO_AIE", "PLIO_23_TO_AIE",
                                           "PLIO_25_TO_AIE", "PLIO_27_TO_AIE", "PLIO_29_TO_AIE", "PLIO_31_TO_AIE"};
static std::vector<std::string> cust_out = {
    "PLIO_02_FROM_AIE", "PLIO_04_FROM_AIE", "PLIO_06_FROM_AIE", "PLIO_08_FROM_AIE",
    "PLIO_10_FROM_AIE", "PLIO_12_FROM_AIE", "PLIO_14_FROM_AIE", "PLIO_16_FROM_AIE",
    "PLIO_18_FROM_AIE", "PLIO_20_FROM_AIE", "PLIO_22_FROM_AIE", "PLIO_24_FROM_AIE",
    "PLIO_26_FROM_AIE", "PLIO_28_FROM_AIE", "PLIO_30_FROM_AIE", "PLIO_32_FROM_AIE"};
vck190_test_harness::occupyUnusedPLIO<16, 16> unusedPLIOs(cust_in, cust_out);

// Initialize and run the graph:
#if defined(__AIESIM__) || defined(__X86SIM__)
int main(int argc, char** argv) {
    aie_dut.init();
    aie_dut.run(2);
    aie_dut.end();

    return 0;
}
#endif
