#!/bin/bash
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

if [ $# -lt 2 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <path of hw package> <libadf.a> <ps executable and other files to be packaged> "
    exit 1
fi

HW_TEMP_DIR=_hw_package_build
arglist=($@)
PACKAGE_PATH=$(realpath ${arglist[0]})
AIE_EXE_PATH=$(realpath ${arglist[1]})

rm -rf ${HW_TEMP_DIR}
mkdir -p ${HW_TEMP_DIR}
mkdir -p ${PACKAGE_PATH}

PACKAGE_SD_FILE=""
for (( c=2; c<$#; c++ ))
do
  PACKAGE_SD_FILE="${PACKAGE_SD_FILE} --package.sd_file $(realpath ${arglist[$c]})"
done

make sd_card -f ${TEST_HARNESS_REPO_PATH}/test_harness/hw.mk BUILD_DIR=$(realpath ${HW_TEMP_DIR}) AIE_EXE=${AIE_EXE_PATH} OTHER_FILE="${PACKAGE_SD_FILE}"
