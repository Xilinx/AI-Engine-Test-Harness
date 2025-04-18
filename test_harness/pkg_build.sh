#!/bin/bash
set -xe

vitis_ver=$1
ver=$(echo "$vitis_ver" | awk -F'_' '{print $1}')
time=`date "+%Y%m%d%H%M%S"`

targets=("vek280_test_harness.xsa" "vek280_sd_card.img.zip" "vck190_test_harness.xsa" "vck190_sd_card.img.zip")

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
    make xsa DEVICE=vck190 TARGET=hw
    make xsa DEVICE=vek280 TARGET=hw
    make sd_card DEVICE=vek280 TARGET=hw
    make sd_card DEVICE=vck190 TARGET=hw

    bin=""
    for target in "${targets[@]}"
    do
        cp ../bin/$target ./
        bin="$bin $target"
    done

    tar -czvf test_harness_xsa_sd_card_${ver}_${time}.tar.gz $bin pkg_info
    rm $bin
}

prepare
build

# manual run:
# set environment variable first
# bash pkg_build.sh <vitis_version>
# e.g.
# bash pkg_build.sh 2025.1_daily_latest
