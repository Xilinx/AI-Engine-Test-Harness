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

ifneq ($(shell expr $(shell echo "__GNUG__" | g++ -E -x c++ - | tail -1) \>= 6), 1)
ifndef XILINX_VIVADO
$(error [ERROR]: g++ version too old. Please use $(CXX_VER) or above)
else
CXX := $(XILINX_VIVADO)/tps/lnx64/gcc-8.3.0/bin/g++
ifeq ($(LD_LIBRARY_PATH),)
export LD_LIBRARY_PATH := $(XILINX_VIVADO)/tps/lnx64/gcc-8.3.0/lib64
else
export LD_LIBRARY_PATH := $(XILINX_VIVADO)/tps/lnx64/gcc-8.3.0/lib64:$(LD_LIBRARY_PATH)
endif
endif
endif

else
AIETARGET := hw
CXX := $(XILINX_VITIS)/gnu/aarch64/lin/aarch64-linux/bin/aarch64-linux-gnu-g++
endif

ifneq ($(findstring 2023.2, $(XILINX_VITIS)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202320_1/xilinx_vck190_base_dfx_202320_1.xpfm
TEMP_DIR := _x_temp.$(TARGET).xilinx_vck190_base_dfx_202320_1.xpfm
PKG_DIR := pkg.$(TARGET).xilinx_vck190_base_dfx_202320_1.xpfm
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
