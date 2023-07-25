/*
Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: X11
*/

#include "graph.h"

int base_col = 0, base_row = 0, matrixCtr = 0;

GeMM g;

// REQUIRED: Instantiate a graph to occupy the PLIOs unused by the user graph.
static std::vector<std::string> cust_in = {
    "PLIO_01_TO_AIE", "PLIO_02_TO_AIE", "PLIO_03_TO_AIE", "PLIO_04_TO_AIE", "PLIO_05_TO_AIE", "PLIO_06_TO_AIE",
    "PLIO_07_TO_AIE", "PLIO_08_TO_AIE", "PLIO_09_TO_AIE", "PLIO_10_TO_AIE", "PLIO_11_TO_AIE", "PLIO_12_TO_AIE",
    "PLIO_13_TO_AIE", "PLIO_14_TO_AIE", "PLIO_15_TO_AIE", "PLIO_16_TO_AIE", "PLIO_17_TO_AIE", "PLIO_18_TO_AIE",
    "PLIO_19_TO_AIE", "PLIO_20_TO_AIE", "PLIO_21_TO_AIE", "PLIO_22_TO_AIE", "PLIO_23_TO_AIE", "PLIO_24_TO_AIE",
    "PLIO_25_TO_AIE", "PLIO_26_TO_AIE", "PLIO_27_TO_AIE", "PLIO_28_TO_AIE", "PLIO_29_TO_AIE", "PLIO_30_TO_AIE",
    "PLIO_31_TO_AIE", "PLIO_32_TO_AIE", "PLIO_33_TO_AIE", "PLIO_34_TO_AIE", "PLIO_35_TO_AIE", "PLIO_36_TO_AIE"};
static std::vector<std::string> cust_out = {"PLIO_01_FROM_AIE", "PLIO_02_FROM_AIE", "PLIO_03_FROM_AIE",
                                            "PLIO_04_FROM_AIE", "PLIO_05_FROM_AIE", "PLIO_06_FROM_AIE",
                                            "PLIO_07_FROM_AIE", "PLIO_08_FROM_AIE"};
vck190_test_harness::occupyUnusedPLIO<36, 8> unusedPLIOs(cust_in, cust_out);

#if defined(__AIESIM__) || defined(__X86SIM__)
int main(void) {
    g.init();
    g.run(GRAPH_ITER_CNT);
    g.end();

    return 0;
}
#endif
