#!/bin/sh

echo "Incorrect arguments supplied"
echo "Usage: $(basename $0) <path to vck190 platform> <libadf.a> <optional files to be packaged> "

RUNDIR=${PWD}
WORKDIR=${PWD}/work/hw
XCLBIN=test_harness_dfx_perf.xclbin
TEST_HARNESS_XSA=./vck190_test_harness.xsa

# Put all input arguments in a list
arglist=($@)

# The libadf.a file must be the first argument
TEST_HARNESS_PFM=$(realpath ${arglist[0]})
LIBADF=$(realpath ${arglist[1]})

# All other arguments are files to be added to the SD card
PACKAGE_SD_FILE=""
for (( c=2; c<$#; c++ ))
do  
  PACKAGE_SD_FILE="${PACKAGE_SD_FILE} --package.sd_file $(realpath ${arglist[$c]})"
done

# Create the work directory
mkdir -p ${WORKDIR}
cd ${WORKDIR}

# Create the xclbin containing the RM PDI
v++ -p -t hw --platform ${TEST_HARNESS_PFM} \
    --package.defer_aie_run \
    -o ${XCLBIN} \
    ${TEST_HARNESS_XSA} \
    ${LIBADF}

# Package the xclbin and other user-supplied files in the SD card
v++ -p -t hw --platform ${TEST_HARNESS_PFM} \
    --package.out_dir ${RUNDIR}/package.hw \
    --package.rootfs $(realpath ${SDKTARGETSYSROOT}/../../rootfs.ext4) \
    --package.kernel_image $(realpath ${SDKTARGETSYSROOT}/../../Image) \
    --package.boot_mode=sd \
    --package.image_format=ext4 \
    --package.sd_file ${XCLBIN} \
    ${PACKAGE_SD_FILE}

cd ${RUNDIR}
