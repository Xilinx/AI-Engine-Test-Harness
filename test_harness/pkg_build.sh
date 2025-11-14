#!/bin/bash
# MIT License
#
# Copyright (C) 2025 Advanced Micro Devices, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 
# Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.
#

vitis_ver=$1
ver=$(echo "$vitis_ver" | awk -F'_' '{print $1}')
time=`date "+%Y%m%d%H%M%S"`

targets=("vek280_test_harness.xsa" "vek280_sd_card.img.zip" "vck190_test_harness.xsa" "vck190_sd_card.img.zip" "vrk160_test_harness.xsa" "vek385_test_harness.xsa"  "server", "vrk160_server.zip", "vek385_server.zip")

prepare() {
    commit_id=$(git log -1 --format=%H)

    if [ -f pkg_info ]; then
        rm pkg_info
        echo delete existed pkg_info
    fi

    touch pkg_info
    echo "vitis version: $vitis_ver" >>pkg_info
    echo "commit id: $commit_id" >>pkg_info
    echo "time: $time" >>pkg_info
}

build() {
    # set environment variable
    # set PLATFORM_REPO_PATHS environment variable 
    # source <path to Versal common image>/environment-setup-cortexa72-cortexa53-xilinx-linux # SDKTARGETSYSROOT
    # source <path to Vitis installation>/settings64.sh # VITIS_TOOL
    # source <path to XRT installation>/setup.sh # VITIS_XRT
    source $VITIS_TOOL
    source $VITIS_XRT
    set -xe
    make xsa server DEVICE=vck190 TARGET=hw
    make xsa server DEVICE=vek280 TARGET=hw
    make xsa server DEVICE=vek385 TARGET=hw
    make xsa server DEVICE=vrk160 TARGET=hw

    bin=""
    for target in "${targets[@]}"
    do
        cp -r ../bin/$target ./
        bin="$bin $(basename $target)"
    done

    tar -czvf test_harness_xsa_sd_card_${ver}_${time}.tar.gz $bin pkg_info
    rm -rf $bin
}

prepare
build

# manual run:
# set environment variable first
# bash pkg_build.sh <vitis_version>
# e.g.
# bash pkg_build.sh 2025.1_daily_latest
