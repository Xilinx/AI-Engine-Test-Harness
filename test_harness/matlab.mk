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

# ############################### Variable Section ###################################
# Setting up Project Variables
MK_PATH 			   := $(abspath $(lastword $(MAKEFILE_LIST)))
TEST_HARNESS_REPO_PATH ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%/test_harness/*}')
DEBUG ?= 0

MATLAB := mex
MATLAB_SRC_DIR := ${TEST_HARNESS_REPO_PATH}/src/matlab/
MATLAB_OBJ_DIR := ${TEST_HARNESS_REPO_PATH}/lib/matlab/

MATLAB_SRC_FILES := $(wildcard $(MATLAB_SRC_DIR)/m_*.cpp)
MATLAB_OBJ_FILES := $(patsubst $(MATLAB_SRC_DIR)/%.cpp, $(MATLAB_OBJ_DIR)/%, $(MATLAB_SRC_FILES))

MATLAB_COMPILE_FLAGS := -I${TEST_HARNESS_REPO_PATH}/include/ps 
# MATLAB_COMPILE_FLAGS += -v
MATLAB_LD_FLAGS := -lmex -lmx

ifeq ($(DEBUG), 1)
	MATLAB_COMPILE_FLAGS += -g -DDEBUG
endif

# ############################### Target Section ###################################

# check if mex is installed
check_matlab:
ifeq (, $(shell which mex 2>/dev/null))
	$(error "No mex in $(PATH), please setup MATLAB path")
endif

$(MATLAB_OBJ_DIR):
	mkdir -p ${MATLAB_OBJ_DIR}

$(MATLAB_OBJ_DIR)/%: ${MATLAB_SRC_DIR}/%.cpp
	@echo "Making target $*"
	$(MATLAB) -output $@ $^ ${MATLAB_COMPILE_FLAGS} ${MATLAB_LD_FLAGS}

mlib: check_matlab ${MATLAB_OBJ_DIR} ${MATLAB_OBJ_FILES}

clean:
	rm -rf ${MATLAB_OBJ_DIR}
