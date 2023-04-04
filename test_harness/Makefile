# Copyright (C) 2023 Advanced Micro Devices, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

TARGET ?= hw
ifneq ($(filter aiesim hw_emu hw, $(TARGET)),)
AIETARGET := hw
else
AIETARGET := x86sim
endif
HOST_ARCH := aarch64

BUILD_DIR := ./tmp

ifneq ($(findstring 2023.1, $(XILINX_VITIS)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202310_1/xilinx_vck190_base_dfx_202310_1.xpfm
endif

ifneq ($(findstring 2022.2, $(XILINX_VITIS)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202220_1/xilinx_vck190_base_dfx_202220_1.xpfm
endif

.PHONY: xsa clean

xsa:
	mkdir -p ${BUILD_DIR}
	aiecompiler --platform ${TEST_HARNESS_PLATFORM} --target ${AIETARGET} -I ./ -I ../include -I ../src/aie -I ${XILINX_VITIS}/aietools/include --constraints=../cfg/aie_constraints.json --event-trace=runtime --event-trace-port=gmio --workdir=./Work ../src/aie/dummy_graph.cpp
	mv libadf.a ${BUILD_DIR}/
	v++ -c -t ${TARGET} --platform ${TEST_HARNESS_PLATFORM} -I ./ -I ../include -k vck190_test_harness --hls.clock 250000000:vck190_test_harness ../src/pl/test_harness.cpp -o ${BUILD_DIR}/vck190_test_harness.xo
	v++ -l -t ${TARGET} --platform ${TEST_HARNESS_PLATFORM} --config ../cfg/system.cfg ${BUILD_DIR}/vck190_test_harness.xo ${BUILD_DIR}/libadf.a -o ${BUILD_DIR}/vck190_test_harness.xsa
	cp ${BUILD_DIR}/vck190_test_harness.xsa ../bin

clean:
	rm -rf ${BUILD_DIR}
	rm -rf AIECompiler.log Map_Report.csv ./Work _x sol.db v++*log xcd.log _x .Xil temp
