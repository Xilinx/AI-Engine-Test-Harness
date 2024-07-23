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
	@echo "  make all TARGET=<aiesim/x86sim/hw>"
	@echo "      Command to generate the design for specified TARGET."
	@echo ""
	@echo "  make run TARGET=<aiesim/x86sim/hw>"
	@echo "      Command to run simulation."
	@echo ""
	@echo "  make cleanall"
	@echo "      Command to remove all the generated files."

# ############################### Settings Section ###################################

# Setting up Project Variables
MK_PATH 			   := $(abspath $(lastword $(MAKEFILE_LIST)))
export TEST_HARNESS_REPO_PATH ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%/examples/*}')

# Setting common config 
include ${TEST_HARNESS_REPO_PATH}/test_harness/common.mk

# aie compilation flags
AIE_CXX_FLAGS 	   := -I $(XILINX_VITIS)/aietools/include   
AIE_CXX_FLAGS      += -I ./aie -I ${TEST_HARNESS_REPO_PATH}/include
AIE_CXX_FLAGS      += -I ${TEST_HARNESS_REPO_PATH}/src/aie
AIE_CXX_FLAGS      += --target=$(AIETARGET) --aie.pl-freq=200.0 --aie.event-trace=runtime --aie.event-trace-port=gmio 
AIE_CXX_FLAGS      += --platform=${TEST_HARNESS_REPO_PATH}/bin/vek280_test_harness.xsa
AIE_CXX_FLAGS      += --aie.workdir=./Work --aie.output-archive=./libadf.a

# host compilation flags
HOST_CXX_FLAGS 		   := -I ./ -I ./aie -I ./ps -I ${TEST_HARNESS_REPO_PATH}/include 
HOST_CXX_FLAGS 		   += -I $(XILINX_VITIS)/aietools/include -I $(XILINX_XRT)/include -std=c++14
HOST_LD_FLAGS 		   := -pthread -lxrt_coreutil -ladf_api_xrt
RUN_DEPS 		       := libadf.a

# Compilation settings according to Release selection
ifneq ($(findstring 2023.2, $(XILINX_VITIS)), )
TEMP_DIR := _x_temp.$(TARGET).xilinx_vek280_es1_base_202320_1.xpfm
PKG_DIR := pkg.$(TARGET).xilinx_vek280_es1_base_202320_1.xpfm
endif
ifneq ($(findstring 2023.1, $(XILINX_VITIS)), )
TEMP_DIR := _x_temp.$(TARGET).xilinx_vek280_es1_base_202310_1.xpfm
PKG_DIR := pkg.$(TARGET).xilinx_vek280_es1_base_202310_1.xpfm
endif
ifneq ($(findstring 2022.2, $(XILINX_VITIS)), )
$(error ERROR: VEK280 is not supported by Vitis 2022.2 or older versions)
endif

# Compilation settings according to TARGET selection
ifneq ($(filter x86sim, $(TARGET)),)
HOST_LD_FLAGS 		   += -L $(XILINX_VITIS)/aietools/lib/lnx64.o -L $(XILINX_XRT)/lib
else
HOST_LD_FLAGS          += -L $(XILINX_VITIS)/aietools/lib/aarch64.o -L $(SDKTARGETSYSROOT)/usr/lib 
HOST_CXX_FLAGS         += --sysroot=$(SDKTARGETSYSROOT)
endif

# Host settings according to TARGET selection 
ifneq ($(filter hw, $(TARGET)),)
RUN_DEPS               += $(TEMP_DIR)/host_elf
endif
# pre_build & compilations
TEST_HARNESS_XSA 	   := ${TEST_HARNESS_REPO_PATH}/bin/vek280_test_harness.xsa
HOST_ELF        	   := $(TEMP_DIR)/host_elf

$(TEST_HARNESS_XSA):
	cd ${TEST_HARNESS_REPO_PATH}/bin && ./download.sh

$(HOST_ELF): ps/host.cpp ./Work/ps/c_rts/aie_control_xrt.cpp
	mkdir -p $(dir $@)
	$(CXX) -g -o $@ $^ $(HOST_CXX_FLAGS) $(HOST_LD_FLAGS)


# ########################## Build Flow ############################ #

# Step 1. Download XSA
download_xsa: $(TEST_HARNESS_XSA)

# Step 2. Build AIE
libadf.a: ./aie/test.cpp ${TEST_HARNESS_REPO_PATH}/bin/vek280_test_harness.xsa
	@echo "Compiling: libadf.a"
	v++ -c --mode aie $(AIE_CXX_FLAGS) $^ 

# Step 3. Build Host 
./Work/ps/c_rts/aie_control_xrt.cpp: libadf.a

build_host_elf: $(HOST_ELF)

# Step 4. Build Package
package: $(RUN_DEPS)
ifeq ($(TARGET), hw)
	${TEST_HARNESS_REPO_PATH}/test_harness/package_vek280_hw.sh ${PKG_DIR} libadf.a $(TEMP_DIR)/host_elf run_script.sh
	ln -s ${PKG_DIR} package_hw
endif

# #################### Build All + Run Emulation ##############
all: check_setup download_xsa build_host_elf package
run: all
ifeq ($(TARGET), x86sim)
	x86simulator --pkg-dir=./Work 
endif
ifeq ($(TARGET), aiesim)
	aiesimulator --pkg-dir=./Work
endif

#################### Clean ###################################
cleanall:
	rm -rf AIECompiler.log Work x86simulator_output xcd.log .Xil AIESimulator.log Map_Report.csv aiesimulator_output/ diag_report.log pl_sample_counts sol.db xsc_report.log v++* libadf.a pkg* _x* package_hw
	rm pl_sample_count* .AIE_SIM_CMD_LINE_OPTIONS