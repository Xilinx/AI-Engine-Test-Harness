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

ifneq ($(findstring 2023.1, $(XILINX_VITIS)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202310_1/xilinx_vck190_base_dfx_202310_1.xpfm
endif

ifneq ($(findstring 2022.2, $(XILINX_VITIS)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202220_1/xilinx_vck190_base_dfx_202220_1.xpfm
endif

${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness.xsa:
	@echo "Please download the pre-built vck190_test_harness.xsa to ${TEST_HARNESS_REPO_PATH}/bin"
	@echo "Or you can go to ${TEST_HARNESS_REPO_PATH}/test_harness and run "make xsa" && false

sd_card: ${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness.xsa
	v++ -p -t hw -o ${BUILD_DIR}/vck190_test_harness.xclbin ${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness.xsa --package.defer_aie_run --platform ${TEST_HARNESS_PLATFORM} --package.out_dir ${BUILD_DIR} ${AIE_EXE}
	v++ -p -t hw --platform ${TEST_HARNESS_PLATFORM} --package.out_dir ${BUILD_DIR} --package.rootfs ${SDKTARGETSYSROOT}/../../rootfs.ext4 --package.kernel_image ${SDKTARGETSYSROOT}/../../Image --package.boot_mode=sd --package.image_format=ext4 --package.sd_file ${BUILD_DIR}/vck190_test_harness.xclbin ${OTHER_FILE}
