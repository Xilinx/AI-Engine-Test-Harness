# MIT License
#
# Copyright (C) 2023-2025 Advanced Micro Devices, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.
#

##################### Project Variables ######################
TARGET ?= hw
CPU ?= x86

ifneq ($(filter sw_emu x86sim, $(TARGET)),)
AIETARGET := x86sim
else
AIETARGET := hw
endif

ifneq ($(filter arm, $(CPU)),)
CXX := $(XILINX_VITIS)/gnu/aarch64/lin/aarch64-linux/bin/aarch64-linux-gnu-g++
else
ifeq ($(shell expr $(shell echo "__GNUG__" | g++ -E -x c++ - | tail -1) \>= 9), 1)
CXX := g++
else
ifndef XILINX_VITIS
$(error [ERROR]: the g++ version is too old. Please use g++-9 or above)
else
CXX := $(XILINX_VITIS)/tps/lnx64/gcc-9.3.0/bin/g++
ifeq ($(LD_LIBRARY_PATH),)
	export LD_LIBRARY_PATH := $(XILINX_VITIS)/tps/lnx64/gcc-9.3.0/lib64
else
	export LD_LIBRARY_PATH := $(XILINX_VITIS)/tps/lnx64/gcc-9.3.0/lib64:$(LD_LIBRARY_PATH)
endif
endif
endif
endif

##################### Check Vitis Setup ######################
check_vitis:
ifeq (,$(wildcard $(XILINX_VITIS)/bin/v++))
	@echo "Cannot locate Vitis installation. Please set XILINX_VITIS variable." && false
endif

check_xrt:
ifeq (,$(wildcard $(XILINX_XRT)/lib/libxilinxopencl.so))
	@echo "Cannot locate XRT installation. Please set XILINX_XRT variable." && false
endif

##################### Platform Setup ######################
TEST_HARNESS_PLATFORM_PATH := ${XILINX_VITIS}/base_platforms/

ifeq ($(DEVICE), vck190)
COMMON_CONFIG_FLAGS := -DPARAM_CHANNELS=36 -DPARAM_DEPTH=4096
else ifeq ($(DEVICE), vek280)
COMMON_CONFIG_FLAGS := -DPARAM_CHANNELS=16 -DPARAM_DEPTH=8192
else
$(error ERROR: DEVICE should be set to either vck190 or vek280)
endif
COMMON_CONFIG_FLAGS += -DPARAM_DEVICE=${DEVICE} -DPARAM_MEM_WIDTH=16 -DPARAM_WIDTH=16 
HOST_CONFIG_FLAGS += ${COMMON_CONFIG_FLAGS} -std=c++17

ifneq ($(findstring 2025.1, $(XILINX_VITIS)), )
ifneq ($(findstring vck190, $(DEVICE)), )
PLATFORM_NAME := xilinx_vck190_base_dfx_202510_1
else
PLATFORM_NAME := xilinx_vek280_base_202510_1
endif
endif

ifneq ($(findstring 2024.2, $(XILINX_VITIS)), )
ifneq ($(findstring vck190, $(DEVICE)), )
PLATFORM_NAME := xilinx_vck190_base_dfx_202420_1
else
PLATFORM_NAME := xilinx_vek280_base_202420_1
endif
endif

ifneq ($(findstring 2024.1, $(XILINX_VITIS)), )
ifneq ($(findstring vck190, $(DEVICE)), )
PLATFORM_NAME := xilinx_vck190_base_dfx_202410_1
else
PLATFORM_NAME := xilinx_vek280_base_202410_1
endif
endif

TEST_HARNESS_PLATFORM := ${TEST_HARNESS_PLATFORM_PATH}/${PLATFORM_NAME}/${PLATFORM_NAME}.xpfm
