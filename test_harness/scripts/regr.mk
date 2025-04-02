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

CPU := arm
include Makefile

ROOTFS ?= ${SDKTARGETSYSROOT}/../../rootfs.ext4
IMAGE ?= ${SDKTARGETSYSROOT}/../../Image

# for vck190, the server path contains run_server.sh test_harness_server test_harness_session
# for vek280, the server path contains run_server.sh vek280_libadf.a test_harness_server test_harness_session vek280_test_harness.xclbin
SERVER_PATH := ${TEST_HARNESS_REPO_PATH}/bin/server/${DEVICE}
SD_CARD_PATH := ./package_hw

${SD_CARD_PATH}:
	mkdir -p ${SD_CARD_PATH}

scripts := ${SD_CARD_PATH}/run_all.sh

vek280_sd_card: package ${scripts}
	v++ -p -t hw --platform ${TEST_HARNESS_PLATFORM} --package.out_dir ${SD_CARD_PATH} \
		--package.rootfs ${ROOTFS} --package.kernel_image ${IMAGE} --package.boot_mode=sd --package.image_format=ext4 \
		--package.sd_file ${PKG_DIR} \
		--package.sd_file ${SERVER_PATH} \
		--package.sd_file ${scripts} \
		--temp_dir ${SD_CARD_PATH} \
		${TEST_HARNESS_REPO_PATH}/bin/vek280_test_harness.xsa ${SERVER_PATH}/vek280_libadf.a


vck190_sd_card: package ${scripts}
	v++ -p -t hw --platform ${TEST_HARNESS_PLATFORM} --package.out_dir ${SD_CARD_PATH} \
		--package.rootfs ${ROOTFS} --package.kernel_image ${IMAGE} --package.boot_mode=sd --package.image_format=ext4 \
		--package.sd_file ${PKG_DIR} \
		--package.sd_file ${SERVER_PATH} \
		--package.sd_file ${scripts} \
		--temp_dir ${SD_CARD_PATH}

sd_card: check_vitis ${SD_CARD_PATH}
ifeq (${DEVICE}, vek280)
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/scripts/regr.mk vek280_sd_card
else
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/scripts/regr.mk vck190_sd_card
endif

all: sd_card
	echo "make all from regr.mk"

${scripts}:
	echo "#!/bin/bash" > ${scripts}
	echo "export SERVER_ROOT=/run/media/mmcblk0p1/${DEVICE}" >> ${scripts}
	echo "/run/media/mmcblk0p1/${DEVICE}/test_harness_server 8080 60 1 1 &" >> ${scripts}
	echo "sleep 5" >> ${scripts}
	echo "cd /run/media/mmcblk0p1/${PKG_DIR} && ./run_script.sh" >> ${scripts}
	chmod +x ${scripts}

clean:
	rm -rf ${SD_CARD_PATH}