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

# ############################### Help Section ###################################
help:
	@echo "Makefile Usage:"
	@echo "  make all "
	@echo "      Command to generate the design. By default, client is running on x86 host. "
	@echo "      If the client to be running in arm (board), please setup your cross compiler (CXX) from sdk correctly. And add 'CPU=arm' to the build command:"
	@echo "          make all CPU=arm "
	@echo ""
	@echo "  make run "
	@echo "      Command to run with connection to server.  "
	@echo ""
	@echo "  make cleanall"
	@echo "      Command to remove all the generated files."

# ############################### Settings Section ###################################

# Setting up Project Variables
MK_PATH 			   := $(abspath $(lastword $(MAKEFILE_LIST)))
export TEST_HARNESS_REPO_PATH ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%/examples/*}')

# Setting common configurations
DEVICE ?= vek280
DEBUG ?= 0

include ${TEST_HARNESS_REPO_PATH}/test_harness/common.mk

# Compilation settings according to Release selection
TEMP_DIR := _x_temp.$(TARGET).${PLATFORM_NAME}.xpfm
PKG_DIR := pkg.$(TARGET).${PLATFORM_NAME}.xpfm

TEST_HARNESS_XSA 	   := ${TEST_HARNESS_REPO_PATH}/bin/vek280_test_harness.xsa
# aie compilation flags
AIE_CXX_FLAGS     := -I $(XILINX_VITIS)/aietools/include 
AIE_CXX_FLAGS     += -I ./aie 
AIE_CXX_FLAGS     += -I ${TEST_HARNESS_REPO_PATH}/include/aie
AIE_CXX_FLAGS     += -I ${TEST_HARNESS_REPO_PATH}/src/aie
AIE_CXX_FLAGS     += --target=$(AIETARGET) --aie.pl-freq=312.5 --aie.event-trace=runtime --aie.event-trace-port=gmio 
AIE_CXX_FLAGS     += --platform=${TEST_HARNESS_XSA}
AIE_CXX_FLAGS     += --aie.workdir=$(TEMP_DIR)/Work --aie.output-archive=$(TEMP_DIR)/libadf.a

# host compilation flags
# Compilation settings according to TARGET selection
HOST_CXX_FLAGS 	:= ${HOST_CONFIG_FLAGS} -I ./ -I ./aie -I ./ps -I${TEST_HARNESS_REPO_PATH}/include/ps 
HOST_LD_FLAGS   := -lpthread

ifeq ($(DEBUG), 1)
	HOST_CXX_FLAGS += -g -DDEBUG
endif

# Host settings according to TARGET selection 
HOST_EFL         	:= $(TEMP_DIR)/host_elf
XCLBIN				:= $(TEMP_DIR)/vek280_test_harness.xclbin
AIE_ADF				:= $(TEMP_DIR)/libadf.a
RUN_DEPS 		   	:= $(HOST_EFL) ${XCLBIN} run_script.sh

# pre_build & compilations
${PKG_DIR}:
	mkdir -p $@

$(TEST_HARNESS_XSA):
	cd ${TEST_HARNESS_REPO_PATH}/bin && ${TEST_HARNESS_REPO_PATH}/test_harness/scripts/download.sh

$(HOST_EFL): ps/host.cpp 
	mkdir -p $(dir $@)
	${CXX} -o $@ $^ $(HOST_CXX_FLAGS) ${HOST_LD_FLAGS}

# ########################## Build Flow ############################ #

# Step 1. Download XSA
download_xsa: $(TEST_HARNESS_XSA)

# Step 2. Build AIE
$(AIE_ADF): ./aie/graph.cpp ${TEST_HARNESS_XSA}
	@echo "Compiling: $(AIE_ADF)"
	v++ -c --mode aie $(AIE_CXX_FLAGS) $^

# Step 3. Build Host 
build_host_elf: $(HOST_EFL)

# Step 4. Build Package
${XCLBIN}: ${AIE_ADF}
ifeq ($(TARGET), hw)
	v++ -p -t ${TARGET} -f ${TEST_HARNESS_PLATFORM} -o $@ $^ \
 		--config ${TEST_HARNESS_REPO_PATH}/cfg/package_aie_only.cfg \
 		--package.defer_aie_run \
		--advanced.param package.enableAiePartitionDrc=0 \
		--package.out_dir ${TEMP_DIR}
endif

package: $(RUN_DEPS) ${PKG_DIR}
	cp $^

all: check_vitis download_xsa package

run: all
ifeq ($(TARGET), hw)
ifneq ($(CPU), arm)
	cd ${PKG_DIR} && LD_LIBRARY_PATH=${LD_LIBRARY_PATH} ./run_script.sh
endif
endif

#################### Clean ###################################
cleanall:
	rm -rf AIECompiler.log Work* x86simulator_output xcd.log .Xil AIESimulator.log Map_Report.csv \
		aiesimulator_output/ diag_report.log pl_sample_counts sol.db xsc_report.log v++* *libadf.a pkg* _x* \
		package_hw .AIE_SIM_CMD_LINE_OPTIONS *.log