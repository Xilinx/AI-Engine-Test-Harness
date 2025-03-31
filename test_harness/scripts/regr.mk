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

sd_card: ${SD_CARD_PATH}
ifeq (${DEVICE}, vek280)
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/scripts/regr.mk vek280_sd_card
else
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/scripts/regr.mk vck190_sd_card
endif

${scripts}:
	echo "#!/bin/bash" > ${scripts}
	echo "/mnt/media/mmcblk0p1/${DEVICE} && ./run_server.sh&" >> ${scripts}
	echo "/mnt/media/mmcblk0p1/pkg.$(TARGET).${PLATFORM_NAME}.xpfm && ./run_script.sh" >> ${scripts}
	chmod +x ${scripts}

clean:
	rm -rf ${SD_CARD_PATH}