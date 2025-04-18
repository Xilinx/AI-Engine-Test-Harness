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

PYTHON_SRC_DIR := ${TEST_HARNESS_REPO_PATH}/src/python/
PYTHON_OBJ_DIR := ${TEST_HARNESS_REPO_PATH}/include/python/lib/

# check if python and pybind11 is installed

check_python := $(shell which python3 2>/dev/null)
check_config := $(shell which python3-config 2>/dev/null)
check_pybind11 := $(shell pip3 show pybind11 2>/dev/null)

check_all:
ifeq (, $(shell which python3 2>/dev/null)) 
	$(error "python3 is not available, please install python3")
else ifeq (, $(shell which python3-config 2>/dev/null)) 
	$(error "python3-config is not available, please install python3-config")
else ifeq (, $(shell python3 -m pybind11 --version 2>/dev/null)) 
	$(error "pybind11 is not available, please install pybind11")
else
SUFFIX := $(shell python3-config --extension-suffix)
PYTHON_SRC_FILES := $(wildcard $(PYTHON_SRC_DIR)/*.cpp)
PYTHON_OBJ_FILES := $(patsubst $(PYTHON_SRC_DIR)/%.cpp, $(PYTHON_OBJ_DIR)/%$(SUFFIX), $(PYTHON_SRC_FILES))
PYTHON_INCS := $(shell python3 -m pybind11 --includes) -I${TEST_HARNESS_REPO_PATH}/include/ps 
PYTHON_FLAGS := -Wall -shared -std=c++17 -fPIC
endif

ifeq ($(DEBUG), 1)
	PYTHON_FLAGS += -g -O0 -DDEBUG
else
	PYTHON_FLAGS += -O3
endif

$(PYTHON_OBJ_DIR):
	mkdir -p ${PYTHON_OBJ_DIR}

# ############################### Target Section ###################################
$(PYTHON_OBJ_DIR)/%$(SUFFIX): ${PYTHON_SRC_DIR}/%.cpp
	@echo "Making target $*"
	g++ -o $@ $^ ${PYTHON_FLAGS} ${PYTHON_INCS}

pylib: check_all clean ${PYTHON_OBJ_DIR} ${PYTHON_OBJ_FILES}

clean:
	@rm -rf ${PYTHON_OBJ_DIR}
