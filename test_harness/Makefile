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

# ############################### Help Section ###################################
help:
	@echo "Makefile Usage:"
	@echo "  make server DEVICE=vck190/vek280/vek385"
	@echo "      Command to generate the server applications and images for specified DEVICE."
	@echo ""
	@echo "  make xsa DEVICE=vck190/vek280/vek385"
	@echo "      Command to generate the xsa for specified DEVICE."
	@echo ""
	@echo "  make python"
	@echo "      Command to generate the test harness python APIs for specified DEVICE."
	@echo ""
	@echo "  make matlab"
	@echo "      Command to generate the test harness matlab APIs for specified DEVICE."
	@echo ""
	@echo "  make clean"
	@echo "      Command to remove all the generated files."

# Setting up Project Variables
MK_PATH 			   := $(abspath $(lastword $(MAKEFILE_LIST)))
TEST_HARNESS_REPO_PATH ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%/test_harness/*}')

TARGET = hw
DEVICE ?= vck190
DEBUG ?= 0

#################### Test Harness XSA ############################ 
xsa:
ifeq ($(DEVICE), vck190)
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/test_harness_base.mk xsa
else ifeq ($(DEVICE), vek280)
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/test_harness_base.mk xsa
else ifeq ($(DEVICE), vek385)
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/test_harness_edf_base.mk xsa
else
	$(error Invalid DEVICE specified. Please use vck190, vek280, or vek385.)
endif

####################### PAKCAGING  ################################

server:
ifeq ($(DEVICE), vck190)
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/test_harness_base.mk server DEVICE=$(DEVICE)
else ifeq ($(DEVICE), vek280)
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/test_harness_base.mk server DEVICE=$(DEVICE)
else ifeq ($(DEVICE), vek385)
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/test_harness_edf_base.mk server DEVICE=$(DEVICE)
else
	$(error Invalid DEVICE specified. Please use vck190, vek280, or vek385.)
endif

matlab:
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/matlab.mk mlib

python:
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/python.mk pylib

####################### Clean Section ############################
clean:
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/test_harness_base.mk clean
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/matlab.mk clean
	make -f ${TEST_HARNESS_REPO_PATH}/test_harness/python.mk clean