# MIT License
#
# Copyright (C) 2024-2025 Advanced Micro Devices, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.
#

# ############################### Variable Section ###################################

# Setting up Project Variables
MK_PATH 			   := $(abspath $(lastword $(MAKEFILE_LIST)))
TEST_HARNESS_REPO_PATH ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%/test_harness/*}')

TARGET ?= hw
DEVICE ?= vek280
DEBUG ?= 0
CPU ?= arm

include ${TEST_HARNESS_REPO_PATH}/test_harness/common.mk

BUILD_DIR := ./build_${TARGET}_${PLATFORM_NAME}

######################### TEST HARNESS AIE AND PL TARGETS ############################

AIE_COMPILE_FLAGS := --mode aie --platform ${TEST_HARNESS_PLATFORM} --target ${AIETARGET} 
AIE_COMPILE_FLAGS += -I ./ -I ${TEST_HARNESS_REPO_PATH}/include/aie -I ${TEST_HARNESS_REPO_PATH}/src/aie -I ${XILINX_VITIS}/aietools/include 
AIE_COMPILE_FLAGS += --aie.event-trace runtime --aie.event-trace-port gmio --aie.pl-freq=312.5
AIE_COMPILE_FLAGS += --aie.Xmapper=disableautostamprepeat

CLOCK := 312500000
COMMON_BUILD_FLAGS := ${COMMON_CONFIG_FLAGS} -t ${TARGET} --save-temps --temp_dir ${BUILD_DIR}
TEST_HARNESS_COMPILE_FLAGS := ${COMMON_BUILD_FLAGS} -I ./ -I${TEST_HARNESS_REPO_PATH}/include/pl/ --hls.pre_tcl ${TEST_HARNESS_REPO_PATH}/cfg/hls.tcl
TEST_HARNESS_LINK_FLAGS := ${COMMON_BUILD_FLAGS} --clock.defaultFreqHz ${CLOCK} --advanced.param compiler.userPreSysLinkOverlayTcl=${TEST_HARNESS_REPO_PATH}/cfg/vivado.tcl
TEST_HARNESS_PACKAGE_FLAGS := ${COMMON_BUILD_FLAGS} --config ${TEST_HARNESS_REPO_PATH}/cfg/package_pl_only.cfg

ifeq ($(DEBUG), 1)
	TEST_HARNESS_COMPILE_FLAGS += -g
	TEST_HARNESS_LINK_FLAGS += -g
else
	TEST_HARNESS_LINK_FLAGS += --config ${TEST_HARNESS_REPO_PATH}/cfg/post_opt.tcl
endif

${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}

####################### ${DEVICE} Test Harness #######################
${BUILD_DIR}/${DEVICE}_libadf.a: ${TEST_HARNESS_REPO_PATH}/src/aie/${DEVICE}_dummy_graph.cpp
	mkdir -p ${BUILD_DIR}
	v++ -c -o $@ $^ ${AIE_COMPILE_FLAGS} --aie.constraints ${TEST_HARNESS_REPO_PATH}/cfg/${DEVICE}_aie_constraints.json --aie.workdir ${BUILD_DIR}/Work_${DEVICE}

${BUILD_DIR}/${DEVICE}_test_harness.xo: ${TEST_HARNESS_REPO_PATH}/src/pl/${DEVICE}_test_harness.cpp
	mkdir -p ${BUILD_DIR}
	v++ -c -o $@ $^ ${TEST_HARNESS_COMPILE_FLAGS} -k ${DEVICE}_test_harness --platform ${TEST_HARNESS_PLATFORM} --hls.clock ${CLOCK}:${DEVICE}_test_harness

${BUILD_DIR}/${DEVICE}_test_harness.xsa: ${BUILD_DIR}/${DEVICE}_libadf.a ${BUILD_DIR}/${DEVICE}_test_harness.xo
	v++ -l -o $@ $^ ${TEST_HARNESS_LINK_FLAGS} --platform ${TEST_HARNESS_PLATFORM} --config ${TEST_HARNESS_REPO_PATH}/cfg/${DEVICE}_system.cfg

${DEVICE}_xsa: check_vitis ${BUILD_DIR}/${DEVICE}_test_harness.xsa
	cp ${BUILD_DIR}/${DEVICE}_test_harness.xsa ${TEST_HARNESS_REPO_PATH}/bin


############################## SD CARD  ##############################

SD_IMAGE_PATH := ${TEST_HARNESS_REPO_PATH}/bin/${DEVICE}_sd_card.img.zip

ROOTFS ?= ${SDKTARGETSYSROOT}/../../rootfs.ext4
IMAGE ?= ${SDKTARGETSYSROOT}/../../Image
CXX := $(XILINX_VITIS)/gnu/aarch64/lin/aarch64-linux/bin/aarch64-linux-gnu-g++

COMMON_COMPILE_FLAGS := ${HOST_CONFIG_FLAGS} -I${TEST_HARNESS_REPO_PATH}/include/ps/ --sysroot=$(SDKTARGETSYSROOT) -std=c++17

ifeq ($(DEBUG), 1)
	COMMON_COMPILE_FLAGS += -g -DDEBUG
endif

SESSION_COMPILE_FLAGS := ${COMMON_COMPILE_FLAGS} -I${XILINX_XRT}/include
SERVER_COMPILE_FLAGS := ${COMMON_COMPILE_FLAGS}

COMMON_LINK_FLAGS := --sysroot=$(SDKTARGETSYSROOT) -L$(SDKTARGETSYSROOT)/usr/lib
SESSION_LINK_FLAGS := ${COMMON_LINK_FLAGS} -lxrt_coreutil -luuid -ladf_api_xrt -L$(XILINX_VITIS)/aietools/lib/aarch64.o
SERVER_LINK_FLAGS := ${COMMON_LINK_FLAGS}

test_harness_session := ${BUILD_DIR}/test_harness_session
test_harness_server  := ${BUILD_DIR}/test_harness_server

${SERVER_FILE_PATH}:
	mkdir -p ${SERVER_FILE_PATH}

${test_harness_session}: ${TEST_HARNESS_REPO_PATH}/src/ps/test_harness_mgr_server.cpp ${TEST_HARNESS_REPO_PATH}/src/ps/test_harness_session.cpp ${TEST_HARNESS_REPO_PATH}/src/ps/test_harness_server.cpp
	${CXX} -o $@ $^ ${SESSION_COMPILE_FLAGS} ${SERVER_MGR_COMPILE_FLAGS} ${SESSION_LINK_FLAGS}

${test_harness_server}: ${TEST_HARNESS_REPO_PATH}/src/ps/test_harness_server.cpp ${TEST_HARNESS_REPO_PATH}/src/ps/test_harness_main.cpp
	${CXX} -o $@ $^ ${SERVER_COMPILE_FLAGS} ${SERVER_LINK_FLAGS}

${BUILD_DIR}/${DEVICE}_test_harness.xclbin: ${BUILD_DIR}/${DEVICE}_test_harness.xsa
	v++ -p -o $@ --platform $^ ${TEST_HARNESS_PACKAGE_FLAGS}

####################### PAKCAGING  ###########################

${DEVICE}_server.zip: check_vitis check_xrt ${BUILD_DIR} ${SERVER_FILE_PATH} ${test_harness_server} ${test_harness_session} \
									${TEST_HARNESS_REPO_PATH}/test_harness/scripts/run_server.sh \
									${BUILD_DIR}/${DEVICE}_test_harness.xclbin

	v++ -p -t hw --platform ${BUILD_DIR}/${DEVICE}_test_harness.xsa --package.out_dir ${BUILD_DIR} \
		--package.defer_aie_run -o ${BUILD_DIR}/${DEVICE}_test_harness.xclbin \
		--save-temps --temp_dir ${BUILD_DIR} ${BUILD_DIR}/${DEVICE}_libadf.a

	@echo "Packaging AIE Test Harness Server Zipfile..."
	zip -j ${BUILD_DIR}/server.zip ${test_harness_server} ${test_harness_session} ${TEST_HARNESS_REPO_PATH}/test_harness/scripts/run_server.sh ${BUILD_DIR}/${DEVICE}_test_harness.xclbin ${BUILD_DIR}/${DEVICE}_test_harness.pdi ${BUILD_DIR}/${DEVICE}_test_harness.dtbo
	cp ${BUILD_DIR}/server.zip ${SD_IMAGE_PATH}

server: ${DEVICE}_server.zip
xsa: ${DEVICE}_xsa

clean:
	rm -rf build_* *.log *.csv *.xcl *.xclbin *.xsa *.xo *.a *.img.zip \
		AIECompiler.log Map_Report.csv ./Work* _x sol.db v++*log xcd.log _x .Xil temp .crashReporter/ .ipcache/ \
		${TEST_HARNESS_REPO_PATH}/bin/*.{xsa,tar.gz,img.zip} ${TEST_HARNESS_REPO_PATH}/bin/server/
