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

if [ $# -lt 3 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <path of hw package> <func_libadf.a> <perf_libadf.a> <ps executable and other files to be packaged> "
    exit 1
fi

arglist=($@)
PACKAGE_PATH=$(realpath ${arglist[0]})
FUNC_AIE_EXE_PATH=$(realpath ${arglist[1]})
PERF_AIE_EXE_PATH=$(realpath ${arglist[2]})

func_xsa_name=${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness_func.xsa
perf_xsa_name=${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness_func.xsa
func_adf_name=${FUNC_AIE_EXE_PATH}
perf_adf_name=${PERF_AIE_EXE_PATH}

func_xsa_ver=23.2
perf_xsa_ver=23.2
func_adf_ver=23.2
perf_adf_ver=23.2
vts_ver=23.2

if [ ! -f ${func_xsa_name} ]
    then
        echo "ERROR: ${func_xsa_name} does not exist!"
        echo "Please download or re-build it from source."
    exit 1
else
    mkdir xsa_tmp
    cd xsa_tmp
    unzip ${func_xsa_name}
    tmp_ver="`grep Version xsa.json | grep -o 20[0-9][0-9]\.[0-9]`"
    func_xsa_ver=${tmp_ver}
    cd ..
    rm -rf xsa_tmp
fi

if [ ! -f ${perf_xsa_name} ]
    then
        echo "ERROR: ${perf_xsa_name} does not exist!"
        echo "Please download or re-build it from source."
    exit 1
else
    mkdir xsa_tmp
    cd xsa_tmp
    unzip ${perf_xsa_name}
    tmp_ver="`grep Version xsa.json | grep -o 20[0-9][0-9]\.[0-9]`"
    perf_xsa_ver=${tmp_ver}
    cd ..
    rm -rf xsa_tmp
fi

if [ ! -f ${func_adf_name} ]
    then
        echo "ERROR: ${func_adf_name} does not exist!"
    exit 1
else
    tmp_ver="`hexdump -C func_libadf.a|grep Vitis\/20[0-9][0-9]\.[0-9] -m 1 |grep -o 20[0-9][0-9]\.[0-9]`"
    func_adf_ver=${tmp_ver}
fi

if [ ! -f ${perf_adf_name} ]
    then
        echo "ERROR: ${perf_adf_name} does not exist!"
    exit 1
else
    tmp_ver="`hexdump -C perf_libadf.a|grep Vitis\/20[0-9][0-9]\.[0-9] -m 1 |grep -o 20[0-9][0-9]\.[0-9]`"
    perf_adf_ver=${tmp_ver}
fi

if [ ${XILINX_VITIS} ]
    then tmp_ver="`vitis -version | grep v20[0-9][0-9]\.[0-9] -m 1 |grep -o 20[0-9][0-9]\.[0-9]`"
    vts_ver=${tmp_ver}
else
    echo "ERROR: Please source your Vitis setup."
    exit 1
fi

if [ $func_adf_ver != $func_xsa_ver ] || [ $perf_adf_ver != $perf_xsa_ver ] || [ $vts_ver != $func_xsa_ver] || [ $vts_ver != $perf_xsa_ver ]
    then
        echo "ERROR: Vitis version not match!"
        echo "${func_xsa_name} is built with Vitis ${func_xsa_ver}."
        echo "${perf_xsa_name} is built with Vitis ${perf_xsa_ver}."
        echo "${func_adf_name} is built with Vitis ${func_adf_ver}."
        echo "${perf_adf_name} is built with Vitis ${perf_adf_ver}."
        echo "You're now using Vitis ${vts_ver}."
        echo "We strongly suggest that using same version of Vitis as ${func_xsa_name} or ${perf_xsa_name} for compiling, linking and packaging."
    exit 1
fi

mkdir -p ${PACKAGE_PATH}

PACKAGE_SD_FILE=""
for (( c=3; c<$#; c++ ))
do
  PACKAGE_SD_FILE="${PACKAGE_SD_FILE} --package.sd_file $(realpath ${arglist[$c]})"
done

make sd_card -f ${TEST_HARNESS_REPO_PATH}/test_harness/vck190_hw.mk BUILD_DIR=$(realpath ${PACKAGE_PATH}) FUNC_AIE_EXE=${FUNC_AIE_EXE_PATH} PERF_AIE_EXE=${PERF_AIE_EXE_PATH} OTHER_FILE="${PACKAGE_SD_FILE}"
