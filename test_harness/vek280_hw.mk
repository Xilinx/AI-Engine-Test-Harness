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

ifneq ($(findstring 2023.2, $(XILINX_VITIS)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vek280_es1_base_202320_1/xilinx_vek280_es1_base_202320_1.xpfm
endif

ifneq ($(findstring 2023.1, $(XILINX_VITIS)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vek280_es1_base_202310_1/xilinx_vek280_es1_base_202310_1.xpfm
endif

ifneq ($(findstring 2022.2, $(XILINX_VITIS)), )
$(error ERROR: VEK280 is not supported by Vitis 2022.2 or older versions)
endif

${TEST_HARNESS_REPO_PATH}/bin/vek280_test_harness.xsa:
	@echo "Please download the pre-built vek280_test_harness.xsa to ${TEST_HARNESS_REPO_PATH}/bin"
	@echo "Or you can go to ${TEST_HARNESS_REPO_PATH}/test_harness and run "make vek280_xsa TARGET=hw DEVICE=vck190" && false

sd_card: ${TEST_HARNESS_REPO_PATH}/bin/vek280_test_harness.xsa
	v++ -t hw --platform ${TEST_HARNESS_PLATFORM} -o ${BUILD_DIR}/vek280_test_harness.xclbin --advanced.param package.enableAiePartitionDrc=0 -p ${TEST_HARNESS_REPO_PATH}/bin/vek280_test_harness.xsa ${AIE_EXE} --package.out_dir ${BUILD_DIR} --package.rootfs ${SDKTARGETSYSROOT}/../../rootfs.ext4 --package.kernel_image ${SDKTARGETSYSROOT}/../../Image --package.boot_mode sd ${OTHER_FILE}


