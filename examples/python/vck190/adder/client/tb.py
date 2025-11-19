# MIT License
#
# Copyright (C) 2024-2025 Advanced Micro Devices, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
# documentation files (the “Software”), to deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or
# otherwise to promote the sale, use or other dealings in this Software without prior written authorization from
# Advanced Micro Devices, Inc.

import os
import sys
import numpy as np
import argparse

if os.environ.get('TEST_HARNESS_REPO_PATH') is None:
    print("ERROR: TEST_HARNESS_REPO_PATH not set")
    sys.exit(1)

path = os.environ['TEST_HARNESS_REPO_PATH']
file_path = os.path.join(path, 'include/python/')
sys.path.insert(1, file_path)

from test_harness_mgr import *

def default(args):
    num_values = 65536 * args.iterations
    a = np.random.randint(-65536, 65536, num_values, dtype=np.int32)
    b = np.random.randint(-65536, 65536, num_values, dtype=np.int32)
    ref = np.add(a, b)

    xclbin_path = args.xclbin
    if os.path.exists(xclbin_path) == False:
        print("ERROR: xclbin file %s not found" % xclbin_path)
        sys.exit(1)

    mgr = test_harness_mgr(xclbin_path, ['gr'], 'vck190')
    test_modes = list()
    if args.function:
        test_modes.append(test_mode.FUNC_MODE)
        
    if args.performance:
        test_modes.append(test_mode.PERF_MODE)

    for mode in test_modes:
        if mode == test_mode.FUNC_MODE:
            print("INFO: Running test in functional mode")
        else:
            print("INFO: Running test in performance mode")

        res = np.zeros(num_values, dtype=np.int32)

        targs = list()
        targs.append(test_harness_args(channel_index.PLIO_01_TO_AIE, num_values * 4, 1, 1, a))
        targs.append(test_harness_args(channel_index.PLIO_03_TO_AIE, num_values * 4, 1, 1, b))
        targs.append(test_harness_args(channel_index.PLIO_02_FROM_AIE, num_values * 4, 1, 1, res))

        mgr.runAIEGraph(0, args.iterations)
        mgr.runTestHarness(mode, targs)
        mgr.waitForRes()
        mgr.printPerf()
        delay = mgr.getDelayBetween(channel_index.PLIO_01_TO_AIE, channel_index.PLIO_02_FROM_AIE)
        print(f"The delay between PLIO_01_TO_AIE and PLIO_02_FROM_AIE is {delay} clock cycles.")

        if mgr.isResultValid():
            if (res != ref).any():
                print("ERROR: a + b != res")
                sys.exit(1)
        elif mode is test_mode.FUNC_MODE:
            print("ERROR: Results should be always valid in the functional mode, a bug happened in the test harness manager")
            sys.exit(1)
        else:
            print("Results are not valid in the performance mode due to large test vectors")

        print("INFO: Test passed")

def pipelined(args):
    number_iterations_per_transactions = 512
    num_values_per_iteration = 65536
    num_values_per_transaction = num_values_per_iteration * number_iterations_per_transactions
    num_values = num_values_per_iteration * args.iterations
    assert(args.iterations % number_iterations_per_transactions == 0)
    a = np.random.randint(-65536, 65536, num_values, dtype=np.int32)
    b = np.random.randint(-65536, 65536, num_values, dtype=np.int32)
    res = np.zeros(num_values, dtype=np.int32)
    ref = np.add(a, b)

    xclbin_path = args.xclbin
    if os.path.exists(xclbin_path) == False:
        print("ERROR: xclbin file %s not found" % xclbin_path)
        sys.exit(1)

    print("INFO: Running test harness in pipelined function testing mode.")
    mgr = test_harness_mgr(xclbin_path, ['gr'], 'vck190')
    mgr.runAIEGraph(0, args.iterations)

    for i in range(args.iterations // number_iterations_per_transactions):
        print("INFO: Running transaction %d" % i)
        fargs = list()
        fargs.append(test_harness_args(channel_index.PLIO_01_TO_AIE, num_values_per_transaction * 4, 1, 1, 
                                       a[i * num_values_per_transaction: (i + 1) * num_values_per_transaction]))
        fargs.append(test_harness_args(channel_index.PLIO_03_TO_AIE, num_values_per_transaction * 4, 1, 1, 
                                       b[i * num_values_per_transaction: (i + 1) * num_values_per_transaction]))
        fargs.append(test_harness_args(channel_index.PLIO_02_FROM_AIE, num_values_per_transaction * 4, 1, 1, 
                                       res[i * num_values_per_transaction: (i + 1) * num_values_per_transaction]))
        mgr.runTestHarness(test_mode.FUNC_MODE, fargs)

    mgr.waitForRes()
    if (res != ref).any():
        print("ERROR: a + b != res")
        sys.exit(1)
    print("INFO: Function mode test passed")

def main(args):
    if args.pipeline:
        pipelined(args)
    else:
        default(args)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Run adder test')
    parser.add_argument('xclbin', type=str, help='Path to xclbin file')
    parser.add_argument('--iterations', type=int, help='number of iterations to run', default=1)
    parser.add_argument('--pipeline', action='store_true', help='pipelined mode')
    parser.add_argument('--function', action='store_true', help='function mode')
    parser.add_argument('--performance', action='store_true', help='performance mode')
    args = parser.parse_args()
    main(args)
