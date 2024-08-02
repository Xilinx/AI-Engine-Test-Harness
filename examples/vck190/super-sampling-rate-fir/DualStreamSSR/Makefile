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
	@echo "  make all TARGET=<aiesim/x86sim/sw_emu/hw>"
	@echo "      Command to generate the design for specified TARGET."
	@echo ""
	@echo "  make run TARGET=<aiesim/x86sim/sw_emu/hw>"
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
FUNC_AIE_CXX_FLAGS 	   := -I $(XILINX_VITIS)/aietools/include   
FUNC_AIE_CXX_FLAGS     += -I ./aie -I ${TEST_HARNESS_REPO_PATH}/include
FUNC_AIE_CXX_FLAGS     += -I ${TEST_HARNESS_REPO_PATH}/src/aie
FUNC_AIE_CXX_FLAGS     += --target=$(AIETARGET) --aie.pl-freq=200.0 --aie.event-trace=runtime --aie.event-trace-port=gmio 
FUNC_AIE_CXX_FLAGS     += --platform=${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness_func.xsa 
FUNC_AIE_CXX_FLAGS     += --aie.workdir=./Work_func --aie.output-archive=./func_libadf.a

PERF_AIE_CXX_FLAGS     := -I $(XILINX_VITIS)/aietools/include 
PERF_AIE_CXX_FLAGS     += -I ./aie 
PERF_AIE_CXX_FLAGS     += -I ${TEST_HARNESS_REPO_PATH}/include 
PERF_AIE_CXX_FLAGS     += -I ${TEST_HARNESS_REPO_PATH}/src/aie 
PERF_AIE_CXX_FLAGS     += --target=$(AIETARGET) --aie.pl-freq=312.5 --aie.event-trace=runtime --aie.event-trace-port=gmio 
PERF_AIE_CXX_FLAGS     += --platform=${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness_perf.xsa 
PERF_AIE_CXX_FLAGS     += --aie.workdir=./Work_perf --aie.output-archive=./perf_libadf.a

# Compilation settings according to Release selection
ifneq ($(findstring 2023.2, $(XILINX_VITIS)), )
TEMP_DIR := _x_temp.$(TARGET).xilinx_vck190_base_dfx_202320_1.xpfm
PKG_DIR := pkg.$(TARGET).xilinx_vck190_base_dfx_202320_1.xpfm
endif
ifneq ($(findstring 2023.1, $(XILINX_VITIS)), )
TEMP_DIR := _x_temp.$(TARGET).xilinx_vck190_base_dfx_202310_1.xpfm
PKG_DIR := pkg.$(TARGET).xilinx_vck190_base_dfx_202310_1.xpfm
endif
ifneq ($(findstring 2022.2, $(XILINX_VITIS)), )
TEMP_DIR := _x_temp.$(TARGET).xilinx_vck190_base_dfx_202220_1.xpfm
PKG_DIR := pkg.$(TARGET).xilinx_vck190_base_dfx_202220_1.xpfm
endif

# host compilation flags
# Compilation settings according to TARGET selection
HOST_CXX_FLAGS 		   := -I ./ -I ./aie -I ./ps -I ${TEST_HARNESS_REPO_PATH}/include 
HOST_CXX_FLAGS 		   += -I $(XILINX_VITIS)/aietools/include -I $(XILINX_XRT)/include -std=c++14
HOST_LD_FLAGS 		   := -pthread -lxrt_coreutil -ladf_api_xrt
ifneq ($(filter x86sim sw_emu, $(TARGET)),)
HOST_LD_FLAGS 		   += -L $(XILINX_VITIS)/aietools/lib/lnx64.o -L $(XILINX_XRT)/lib
else
HOST_LD_FLAGS          += -L $(XILINX_VITIS)/aietools/lib/aarch64.o -L $(SDKTARGETSYSROOT)/usr/lib 
HOST_CXX_FLAGS         += --sysroot=$(SDKTARGETSYSROOT)
endif

# Host settings according to TARGET selection 
FUNC_RUN_DEPS 		   := func_libadf.a
PERF_RUN_DEPS 		   := perf_libadf.a
ifneq ($(filter sw_emu hw, $(TARGET)),)
FUNC_RUN_DEPS += $(TEMP_DIR)_func/host_elf
PERF_RUN_DEPS += $(TEMP_DIR)_perf/host_elf
endif

# pre_build & compilations
TEST_HARNESS_XSA 	   := ${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness_func.xsa ${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness_perf.xsa
FUNC_HOST_ELF          := $(TEMP_DIR)_func/host_elf
PERF_HOST_ELF          := $(TEMP_DIR)_perf/host_elf

$(TEST_HARNESS_XSA):
	cd ${TEST_HARNESS_REPO_PATH}/bin && ./download.sh

$(FUNC_HOST_ELF): ps/host.cpp ./Work_func/ps/c_rts/aie_control_xrt.cpp
	mkdir -p $(dir $@)
	$(CXX) -g -o $@ $^ $(HOST_CXX_FLAGS) $(HOST_LD_FLAGS)

$(PERF_HOST_ELF): ps/host.cpp ./Work_perf/ps/c_rts/aie_control_xrt.cpp
	mkdir -p $(dir $@)
	$(CXX) -g -o $@ $^ $(HOST_CXX_FLAGS) $(HOST_LD_FLAGS)

# ########################## Build Flow ############################ #

# Step 1. Download XSA
download_xsa: $(TEST_HARNESS_XSA)

# Step 2. Build AIE
func_libadf.a: ./aie/SSRgraph.cpp ${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness_func.xsa
	@echo "Compiling: func_libadf.a"
	v++ -c --mode aie $(FUNC_AIE_CXX_FLAGS) $^ 

perf_libadf.a: ./aie/SSRgraph.cpp ${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness_perf.xsa
	@echo "Compiling: perf_libadf.a"
	v++ -c --mode aie $(PERF_AIE_CXX_FLAGS) $^ 

# Step 3. Build Host 
./Work_func/ps/c_rts/aie_control_xrt.cpp: func_libadf.a
./Work_perf/ps/c_rts/aie_control_xrt.cpp: perf_libadf.a

build_func_host_elf: $(FUNC_HOST_ELF)
build_perf_host_elf: $(PERF_HOST_ELF)

# Step 4. Build Package
package: $(FUNC_RUN_DEPS) $(PERF_RUN_DEPS)
ifeq ($(TARGET), sw_emu)
	${TEST_HARNESS_REPO_PATH}/test_harness/package_sw_emu.sh ${PKG_DIR} func_libadf.a perf_libadf.a $(TEMP_DIR)_perf/host_elf
endif
ifeq ($(TARGET), hw)
	${TEST_HARNESS_REPO_PATH}/test_harness/package_vck190_hw.sh ${PKG_DIR} func_libadf.a perf_libadf.a $(TEMP_DIR)_perf/host_elf run_script.sh
	ln -s ${PKG_DIR} package_hw
endif

# #################### Build All + Run Emulation ##############
# Example test for FUNC_MODE, please remember to align the path of host_elf and xclbin
# with the testing mode sepcified in ps/host.cpp
all: check_setup download_xsa build_func_host_elf build_perf_host_elf package
run: all
ifeq ($(TARGET), x86sim)
	x86simulator --pkg-dir=./Work_perf
endif
ifeq ($(TARGET), aiesim)
	aiesimulator --pkg-dir=./Work_perf
endif
ifeq ($(TARGET), sw_emu)
	LD_LIBRARY_PATH=$(LIBRARY_PATH):$$LD_LIBRARY_PATH \
	XCL_EMULATION_MODE=sw_emu $(PKG_DIR)/host_elf $(PKG_DIR)/vck190_test_harness.xclbin
endif

#################### Clean ###################################
cleanall:
	rm -rf AIECompiler.log Work* x86simulator_output xcd.log .Xil AIESimulator.log Map_Report.csv aiesimulator_output/ diag_report.log pl_sample_counts sol.db xsc_report.log v++* *libadf.a pkg* _x* package_hw .AIE_SIM_CMD_LINE_OPTIONS