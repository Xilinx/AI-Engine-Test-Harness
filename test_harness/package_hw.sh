#!/bin/bash

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
