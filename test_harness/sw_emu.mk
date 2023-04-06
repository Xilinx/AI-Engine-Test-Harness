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

ifneq ($(findstring 2023.1, $(XILINX_VITIS)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202310_1/xilinx_vck190_base_dfx_202310_1.xpfm
endif

ifneq ($(findstring 2022.2, $(XILINX_VITIS)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202220_1/xilinx_vck190_base_dfx_202220_1.xpfm
endif

${BUILD_DIR}/vck190_test_harness.xsa: ${TEST_HARNESS_REPO_PATH}/src/pl/test_harness.cpp ${TEST_HARNESS_REPO_PATH}/cfg/system.cfg
	v++ -c -t sw_emu --platform ${TEST_HARNESS_PLATFORM} -I ./ -I ${TEST_HARNESS_REPO_PATH}/include -k vck190_test_harness --hls.clock 250000000:vck190_test_harness ${TEST_HARNESS_REPO_PATH}/src/pl/test_harness.cpp -o ${BUILD_DIR}/vck190_test_harness.xo
	v++ -l -t sw_emu --platform ${TEST_HARNESS_PLATFORM} --config ${TEST_HARNESS_REPO_PATH}/cfg/system.cfg ${BUILD_DIR}/vck190_test_harness.xo ${AIE_EXE} -o ${BUILD_DIR}/vck190_test_harness.xsa
	v++ -p -t sw_emu -o ${BUILD_DIR}/vck190_test_harness.xclbin ${BUILD_DIR}/vck190_test_harness.xsa --package.defer_aie_run --platform ${TEST_HARNESS_PLATFORM} --package.out_dir ${BUILD_DIR} ${AIE_EXE}

${BUILD_DIR}/emconfig.json: ${BUILD_DIR}/vck190_test_harness.xsa
	emconfigutil --platform ${TEST_HARNESS_PLATFORM} --od ${BUILD_DIR}

xsa: ${BUILD_DIR}/vck190_test_harness.xsa ${BUILD_DIR}/emconfig.json

