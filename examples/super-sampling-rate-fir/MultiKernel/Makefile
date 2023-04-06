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

##################### Check Vitis Setup ######################
.PHONY: check_setup
check_setup:
ifeq (,$(wildcard $(XILINX_VITIS)/bin/v++))
	@echo "Cannot locate Vitis installation. Please set XILINX_VITIS variable." && false
endif
ifeq (,$(wildcard $(XILINX_XRT)/lib/libxilinxopencl.so))
	@echo "Cannot locate XRT installation. Please set XILINX_XRT variable." && false
endif

##################### Project Variables ######################
TARGET := sw_emu
ifneq ($(filter x86sim sw_emu, $(TARGET)),)
AIETARGET := x86sim
CXX := g++
else
AIETARGET := hw
CXX := $(XILINX_VITIS)/gnu/aarch64/lin/aarch64-linux/bin/aarch64-linux-gnu-g++
endif

ifneq ($(findstring 2023.1, $(XILINX_VITIS)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202310_1/xilinx_vck190_base_dfx_202310_1.xpfm
TEMP_DIR := _x_temp.$(TARGET).xilinx_vck190_base_dfx_202310_1.xpfm
PKG_DIR := pkg.$(TARGET).xilinx_vck190_base_dfx_202310_1.xpfm
endif
ifneq ($(findstring 2022.2, $(XILINX_VITIS)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202220_1/xilinx_vck190_base_dfx_202220_1.xpfm
TEMP_DIR := _x_temp.$(TARGET).xilinx_vck190_base_dfx_202220_1.xpfm
PKG_DIR := pkg.$(TARGET).xilinx_vck190_base_dfx_202220_1.xpfm
endif
PRE_BUILD_DIR := ../../../bin

##################### Build AIE ##############################
AIE_CXX_FLAGS := -I $(XILINX_VITIS)/aietools/include
AIE_CXX_FLAGS += -I ./aie -I ../../../include -I ../../../src/aie
AIE_CXX_FLAGS += --target=$(AIETARGET) --platform=$(TEST_HARNESS_PLATFORM) --pl-freq=250 --constraints=../../../cfg/aie_constraints.json 
#AIE_CXX_FLAGS += --event-trace=runtime --event-trace-port=gmio
AIE_EXE := $(TEMP_DIR)/libadf.a
$(AIE_EXE): ./aie/graph.cpp
	@echo "Compiling: libadf.a"
	mkdir -p $(dir $@)
	aiecompiler $(AIE_CXX_FLAGS) -workdir=./Work $^ 
	cp ./libadf.a $(TEMP_DIR)/libadf.a

##################### Build Host #############################
./Work/ps/c_rts/aie_control_xrt.cpp: $(AIE_EXE)
HOST_SRC := ps/host.cpp ./Work/ps/c_rts/aie_control_xrt.cpp

HOST_CXX_FLAGS := -I ./ -I ./aie -I ../../../include 
HOST_CXX_FLAGS += -I $(XILINX_VITIS)/aietools/include -I $(XILINX_XRT)/include
HOST_CXX_FLAGS += -fmessage-length=0 -std=c++14 -O3 -Wall 

ifneq ($(filter x86sim sw_emu, $(TARGET)),)
HOST_LD_FLAGS := -L $(XILINX_VITIS)/aietools/lib/lnx64.o -L $(XILINX_XRT)/lib
else
HOST_LD_FLAGS := -L $(XILINX_VITIS)/aietools/lib/aarch64.o -L $(SDKTARGETSYSROOT)/usr/lib 
HOST_CXX_FLAGS += --sysroot=$(SDKTARGETSYSROOT)
endif
HOST_LD_FLAGS += -pthread -lxrt_coreutil -ladf_api_xrt
HOST_EXE := $(TEMP_DIR)/host_elf

$(HOST_EXE): $(HOST_SRC) $(AIE_EXE)
	mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(HOST_CXX_FLAGS) $(HOST_LD_FLAGS)

##################### Determine What To Build ################
RUN_DEPS := $(TEMP_DIR)/libadf.a
ifeq ($(TARGET), sw_emu)
RUN_DEPS += $(HOST_EXE)
endif
ifeq ($(TARGET), hw)
RUN_DEPS += $(HOST_EXE)
endif

##################### Package  ###############################
package: $(RUN_DEPS)
ifeq ($(TARGET), sw_emu)
	${TEST_HARNESS_REPO_PATH}/test_harness/package_sw_emu.sh ${PKG_DIR} ${AIE_EXE} ${HOST_EXE}
endif
ifeq ($(TARGET), hw)
	${TEST_HARNESS_REPO_PATH}/test_harness/package_hw.sh ${PKG_DIR} ${AIE_EXE} ${HOST_EXE}
endif

sd_card: package

##################### Build All + Run Emulation ##############
all: check_setup $(RUN_DEPS) sd_card
run: all
ifeq ($(TARGET), x86sim)
	x86simulator --pkg-dir=./Work
endif
ifeq ($(TARGET), aiesim)
	aiesimulator --pkg-dir=./Work
endif
ifeq ($(TARGET), sw_emu)
	LD_LIBRARY_PATH=$(LIBRARY_PATH):$$LD_LIBRARY_PATH \
	XCL_EMULATION_MODE=sw_emu $(PKG_DIR)/host_elf $(PKG_DIR)/vck190_test_harness.xclbin
endif
ifeq ($(TARGET), hw_emu)
endif

#################### Clean ###################################
cleanall:
	rm -rf AIECompiler.log Work $(TEMP_DIR) $(PKG_DIR) x86simulator_output xcd.log .Xil AIESimulator.log Map_Report.csv aiesimulator_output/ diag_report.log pl_sample_counts sol.db xsc_report.log v++*log _x libadf.a _sw_emu_package_build/ _hw_package_build
