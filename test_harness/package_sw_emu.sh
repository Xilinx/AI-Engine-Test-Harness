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
    echo "Usage: $(basename $0) <path of sw_emu package> <libadf.a> <ps executable and other files to be packaged> "
    exit 1
fi

SW_EMU_TEMP_DIR=_sw_emu_package_build
arglist=($@)
PACKAGE_PATH=$(realpath ${arglist[0]})
AIE_EXE_PATH=$(realpath ${arglist[1]})

rm -rf ${SW_EMU_TEMP_DIR}
mkdir -p ${SW_EMU_TEMP_DIR}
mkdir -p ${PACKAGE_PATH}

make xsa -f ${TEST_HARNESS_REPO_PATH}/test_harness/sw_emu.mk BUILD_DIR=$(realpath ${SW_EMU_TEMP_DIR}) AIE_EXE=${AIE_EXE_PATH}

cp ${SW_EMU_TEMP_DIR}/vck190_test_harness.xclbin ${PACKAGE_PATH}
cp ${SW_EMU_TEMP_DIR}/emconfig.json ${PACKAGE_PATH}
for (( c=2; c<$#; c++ ))
do  
    cp -rp $(realpath ${arglist[$c]}) ${PACKAGE_PATH}
done
