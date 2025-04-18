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

import os
import numpy as np
from lib import test_harness_python as tp

class test_mode(tp.TestMode):
    pass

class channel_index(tp.channel_index):
    pass

def test_harness_args(channel_index: channel_index, size_in_bytes: int, 
                        repetition: int, delay: int, data: np.array):
        if data.dtype == np.int8:
            return tp.test_harness_args_int8_t(channel_index, size_in_bytes, repetition, delay, data)
        elif data.dtype == np.int16:
            return tp.test_harness_args_int16_t(channel_index, size_in_bytes, repetition, delay, data)
        elif data.dtype == np.int32:
            return tp.test_harness_args_int32_t(channel_index, size_in_bytes, repetition, delay, data)
        elif data.dtype == np.int64:
            return tp.test_harness_args_int64_t(channel_index, size_in_bytes, repetition, delay, data)
        elif data.dtype == np.uint8:
            return tp.test_harness_args_uint8_t(channel_index, size_in_bytes, repetition, delay, data)
        elif data.dtype == np.uint16:
            return tp.test_harness_args_uint16_t(channel_index, size_in_bytes, repetition, delay, data)
        elif data.dtype == np.uint32:
            return tp.test_harness_args_uint32_t(channel_index, size_in_bytes, repetition, delay, data)
        elif data.dtype == np.uint64:
            return tp.test_harness_args_uint64_t(channel_index, size_in_bytes, repetition, delay, data)
        elif data.dtype == np.float32:
            return tp.test_harness_args_float(channel_index, size_in_bytes, repetition, delay, data)
        elif data.dtype == np.float64:
            return tp.test_harness_args_double(channel_index, size_in_bytes, repetition, delay, data)

class test_harness_mgr(tp.test_harness_python):
    def __init__(self, xclbin_path: str, graph_names: list,
                    device_name = "vck190", timeout: int = 0, device_index = 0):
        if os.path.exists(xclbin_path) == False:
            raise FileNotFoundError("XCLBIN file %s not found" % xclbin_path)
        tp.test_harness_python.__init__(self, xclbin_path, graph_names, 
                    device_name, timeout, device_index)

    def runTestHarness(self, mode: test_mode, args: list, timeout: int = 0):
        self.args = args
        for arg in self.args:
            if not isinstance(arg, tp.test_harness_args):
                raise TypeError("Invalid argument type. Expected test_harness_args")
        tp.test_harness_python.runTestHarness(self, mode, args, timeout)
    
    def waitForRes(self, millisecond: int = 0, num_trans: int = 0):
        tp.test_harness_python.waitForRes(self, millisecond, num_trans)

    def printPerf(self, idx: int = -1):
        tp.test_harness_python.printPerf(self, idx)

    def isResultValid(self, idx: int = -1):
        return tp.test_harness_python.isResultValid(self, idx)
    
    def getDelayBetween(self, idx0: channel_index, idx1: channel_index, t_id: int = -1):
        return tp.test_harness_python.getDelayBetween(self, idx0, idx1, t_id)
