//
// Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
// SPDX-License-Identifier: MIT
//
// Author: Mark Rollins

#include <iostream>
#include "pfa1008_graph.h"

// Instantiate AIE graph:
pfa1008_graph aie_dut;

// REQUIRED: Instantiate a graph to occupy the PLIOs unused by the user graph.
static std::vector<std::string> cust_in = {"PLIO_01_TO_AIE", "PLIO_03_TO_AIE", "PLIO_05_TO_AIE", "PLIO_07_TO_AIE",
                                           "PLIO_09_TO_AIE", "PLIO_11_TO_AIE"};
static std::vector<std::string> cust_out = {
    "PLIO_02_FROM_AIE", "PLIO_04_FROM_AIE", "PLIO_06_FROM_AIE", "PLIO_08_FROM_AIE",
    "PLIO_10_FROM_AIE", "PLIO_12_FROM_AIE"};
vck190_test_harness::occupyUnusedPLIO<6, 6> unusedPLIOs(cust_in, cust_out);

// Initialize and run the graph:
#if defined(__AIESIM__) || defined(__X86SIM__)
int main(void)
{
  std::cout << "*** pfa1008_app ***" << std::endl;

  aie_dut.init();
  aie_dut.run(8);
  aie_dut.end();

  return 0;
}
#endif
