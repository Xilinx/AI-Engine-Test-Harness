# Copyright 2019-2022 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# vitis makefile-generator v2.0.8
#

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
