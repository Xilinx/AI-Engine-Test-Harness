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

ROOTFS ?= ${SDKTARGETSYSROOT}/../../rootfs.ext4
IMAGE ?= ${SDKTARGETSYSROOT}/../../Image

DEVICE := vck190
include ${TEST_HARNESS_REPO_PATH}/test_harness/common.mk

${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness.xsa:
	@echo "Please download the pre-built vck190_test_harness.xsa to ${TEST_HARNESS_REPO_PATH}/bin"
	@echo "Or you can go to ${TEST_HARNESS_REPO_PATH}/test_harness and run "make xsa TARGET=hw DEVICE=vck190" && false

xclbin := ${BUILD_DIR}/vck190_test_harness.xclbin

${xclbin}: ${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness.xsa ${AIE_EXE_PATH}
	v++ -p --advanced.param package.enableAiePartitionDrc=0 -t hw -o $@ $^ --package.defer_aie_run --platform ${TEST_HARNESS_PLATFORM}

xclbin: ${xclbin}
