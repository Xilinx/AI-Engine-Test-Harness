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
	v++ -p -t hw --platform ${TEST_HARNESS_PLATFORM} --package.out_dir ${BUILD_DIR} --package.rootfs ${EDGE_COMMON_SW}/rootfs.ext4 --package.kernel_image ${EDGE_COMMON_SW}/Image --package.boot_mode=sd --package.image_format=ext4 --package.sd_file ${BUILD_DIR}/vck190_test_harness.xclbin ${OTHER_FILE}
