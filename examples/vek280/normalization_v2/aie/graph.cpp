/*
Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT
*/
#include "graph.h"
#include "test_harness_graph.hpp"

SimpleGraph gr;
static std::vector<std::string> cust_in={"PLIO_01_TO_AIE"};
static std::vector<std::string> cust_out={"PLIO_02_FROM_AIE"};
test_harness::occupyUnusedPLIO<1,1,16> unusedPLIOs(cust_in, cust_out); //<PLIO_IN_NUM, PLIO_OUT_NUM, Total PLIO IN OR OUT NUM=36>

int main(int argc, char ** argv) {
	gr.init();
	gr.run(4);
	gr.wait();
	gr.end();
	return 0;
}
