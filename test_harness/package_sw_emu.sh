#!/bin/bash
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

if [ $# -lt 2 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <path of sw_emu package> <libadf.a> <ps executable and other files to be packaged> "
    exit 1
fi

arglist=($@)
PACKAGE_PATH=$(realpath ${arglist[0]})
AIE_EXE_PATH=$(realpath ${arglist[1]})

mkdir -p ${PACKAGE_PATH}

make xsa -f ${TEST_HARNESS_REPO_PATH}/test_harness/sw_emu.mk BUILD_DIR=${PACKAGE_PATH} AIE_EXE=${AIE_EXE_PATH}

for (( c=2; c<$#; c++ ))
do  
    cp -rp $(realpath ${arglist[$c]}) ${PACKAGE_PATH}
done
