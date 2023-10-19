# MIT License
#
# Copyright (C) 2023 Advanced Micro Devices, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.
#

TARGET ?= hw
ifneq ($(filter aiesim hw_emu hw, $(TARGET)),)
AIETARGET := hw
else
AIETARGET := x86sim
endif

DEVICE ?= vck190
ifneq ($(filter vck190, $(TARGET)),)
DEVICE := vck190
else
DEVICE := vek280
endif

HOST_ARCH := aarch64
BUILD_DIR := ./tmp

ifneq ($(findstring 2023.2, $(XILINX_VITIS)), )
ifneq ($(findstring vck190, $(DEVICE)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202320_1/xilinx_vck190_base_dfx_202320_1.xpfm
else
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vek280_es1_base_202320_1/xilinx_vek280_es1_base_202320_1.xpfm
endif
endif

ifneq ($(findstring 2023.1, $(XILINX_VITIS)), )
ifneq ($(findstring vck190, $(DEVICE)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202310_1/xilinx_vck190_base_dfx_202310_1.xpfm
else
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vek280_es1_base_202310_1/xilinx_vek280_es1_base_202310_1.xpfm
endif
endif

ifneq ($(findstring 2022.2, $(XILINX_VITIS)), )
ifneq ($(findstring vck190, $(DEVICE)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202220_1/xilinx_vck190_base_dfx_202220_1.xpfm
else
$(error ERROR: VEK280 is not supported by Vitis 2022.2 or older versions)
endif
endif

.PHONY: vck190_func_xsa vck190_perf_xsa vek280_xsa clean

vck190_func_xsa:
	mkdir -p ${BUILD_DIR}_vck190_func
	v++ -c --mode aie --platform ${TEST_HARNESS_PLATFORM} --target ${AIETARGET} -I ./ -I ../include -I ../src/aie -I ${XILINX_VITIS}/aietools/include --aie.constraints ../cfg/vck190_aie_constraints.json --aie.event-trace runtime --aie.event-trace-port gmio --aie.workdir ./Work_vck190_func ../src/aie/vck190_dummy_graph.cpp
	mv libadf.a ${BUILD_DIR}_vck190_func/
	v++ -c -t ${TARGET} --platform ${TEST_HARNESS_PLATFORM} -I ./ -I ../include -k vck190_test_harness_func --hls.clock 312500000:vck190_test_harness_func ../src/pl/test_harness.cpp -o ${BUILD_DIR}_vck190_func/vck190_test_harness_func.xo
	v++ -l -t ${TARGET} --platform ${TEST_HARNESS_PLATFORM} --clock.defaultFreqHz 200000000 --config ../cfg/vck190_func_system.cfg ${BUILD_DIR}_vck190_func/vck190_test_harness_func.xo ${BUILD_DIR}_vck190_func/libadf.a -o ${BUILD_DIR}_vck190_func/vck190_test_harness_func.xsa --config post_opt.tcl
	cp ${BUILD_DIR}_vck190_func/vck190_test_harness_func.xsa ../bin

vck190_perf_xsa:
	mkdir -p ${BUILD_DIR}_vck190_perf
	v++ -c --mode aie --platform ${TEST_HARNESS_PLATFORM} --target ${AIETARGET} -I ./ -I ../include -I ../src/aie -I ${XILINX_VITIS}/aietools/include --aie.constraints ../cfg/vck190_aie_constraints.json --aie.event-trace runtime --aie.event-trace-port gmio --aie.workdir ./Work_vck190_perf ../src/aie/vck190_dummy_graph.cpp
	mv libadf.a ${BUILD_DIR}_vck190_perf/
	v++ -c -t ${TARGET} --platform ${TEST_HARNESS_PLATFORM} -I ./ -I ../include -k vck190_test_harness_perf --hls.clock 400000000:vck190_test_harness_perf ../src/pl/test_harness.cpp -o ${BUILD_DIR}_vck190_perf/vck190_test_harness_perf.xo
	v++ -l -t ${TARGET} --platform ${TEST_HARNESS_PLATFORM} --clock.defaultFreqHz 312500000 --config ../cfg/vck190_perf_system.cfg ${BUILD_DIR}_vck190_perf/vck190_test_harness_perf.xo ${BUILD_DIR}_vck190_perf/libadf.a -o ${BUILD_DIR}_vck190_perf/vck190_test_harness_perf.xsa --config post_opt.tcl
	cp ${BUILD_DIR}_vck190_perf/vck190_test_harness_perf.xsa ../bin

vek280_xsa:
	mkdir -p ${BUILD_DIR}_vek280
	v++ -c --mode aie --platform ${TEST_HARNESS_PLATFORM} --target ${AIETARGET} -I ./ -I ../include -I ../src/aie -I ${XILINX_VITIS}/aietools/include --aie.constraints ../cfg/vek280_aie_constraints.json --aie.event-trace runtime --aie.event-trace-port gmio --aie.workdir ./Work_vek280 ../src/aie/vek280_dummy_graph.cpp
	mv libadf.a ${BUILD_DIR}_vek280/
	v++ -c -t ${TARGET} --platform ${TEST_HARNESS_PLATFORM} -I ./ -I ../include -k vek280_test_harness --hls.clock 400000000:vek280_test_harness ../src/pl/test_harness.cpp -o ${BUILD_DIR}_vek280/vek280_test_harness.xo
	v++ -l -t ${TARGET} --platform ${TEST_HARNESS_PLATFORM} --clock.defaultFreqHz 312500000 --config ../cfg/vek280_system.cfg ${BUILD_DIR}_vek280/vek280_test_harness.xo ${BUILD_DIR}_vek280/libadf.a -o ${BUILD_DIR}_vek280/vek280_test_harness.xsa --config post_opt.tcl
	cp ${BUILD_DIR}_vek280/vek280_test_harness.xsa ../bin

clean:
	rm -rf ${BUILD_DIR}* libadf.a
	rm -rf AIECompiler.log Map_Report.csv ./Work* _x sol.db v++*log xcd.log _x .Xil temp .crashReporter/ .ipcache/
