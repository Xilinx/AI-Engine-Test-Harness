/*
 * MIT License
 *
 * Copyright (C) 2024-2025 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the “Software”), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or
 * otherwise to promote the sale, use or other dealings in this Software without prior written authorization from
 * Advanced Micro Devices, Inc.
 */

#ifndef _TEST_HARNESS_MGR_BASE_HPP_
#define _TEST_HARNESS_MGR_BASE_HPP_

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <vector>
#include <cstring>

#include "test_harness_utils.hpp"
namespace test_harness {
    
/**
* test_harness_mgr_if - Interface for the test harness manager
*/
class test_harness_mgr_if {
   public:
    test_harness_mgr_if() = default;
    virtual ~test_harness_mgr_if() = default;

    /**
        * runTestHarness - Run the test harness with the given mode and arguments
        *
        * @param mode
        * Test mode to run the test harness in
        * @param args
        * Vector of test harness arguments
        * @param timeout
        * Timeout in milliseconds
        *
    */
    virtual void runTestHarness(TestMode mode, const std::vector<test_harness_args>& args, uint32_t timeout) = 0;

    /**
        * runAIEGraph - Run the AIE graph with the given graph index and number of iterations
        *
        * @param g_idx
        * Index of the AIE graph to run
        * @param iters
        * Number of iterations to run the AIE graph for
        *
    */
    virtual void runAIEGraph(unsigned int g_idx, unsigned int iters) = 0;

    /**
        * waitForRes - Wait for the test harness to complete
        *
        * @param timeout_millisec
        * Timeout in milliseconds
        * @param number
        * Number of test harnesses to wait for, 0 for all
        *
    */
    virtual void waitForRes(uint32_t timeout_millisec, uint32_t number) = 0;

    /**
        * printPerf - Print the performance of the test harness
        *
        * @param idx
        * Index of the test harness to print the performance for, -1 for the last finished transaction
        *
    */
    virtual void printPerf(int32_t idx = -1) = 0;

    /**
        * isResultValid - Check if the result of the test harness is valid
        *
        * @return bool - True if the result is valid, false otherwise
        * @param idx
        * Index of the test harness to print the result for, -1 for the last finished transaction
        *
    */
    virtual bool isResultValid(int32_t idx = -1) = 0;
};

/**
 * test_harness_mgr_base - Base class for the test harness manager
 *
 * @param device
 * Device name e.g., vck190 or vek280
 * @param num_channels
 * Number of PLIOs (for input/output respectively)
 * @param word_size
 * PLIO bit-width in bytes
 * @param depth
 * Depth of the 128-bit width cache in each PLIO channel
 * @param memWidth
 * Width of the memory in bytes
 *
 */
class test_harness_mgr_base {
   public:
    test_harness_mgr_base(const std::string& device_name): device(device_name), num_channels(0), word_size(0), depth(0), memWidth(0) {}
    
    void setup(unsigned int num_channels, unsigned int depth, unsigned int word_size, unsigned int memWidth) {
        this->num_channels = num_channels;
        this->depth = depth;
        this->word_size = word_size;
        this->memWidth = memWidth;
    }

    ~test_harness_mgr_base() = default;

   protected:
    std::string device;

    unsigned int num_channels;
    unsigned int word_size;
    unsigned int depth;
    unsigned int memWidth;
};

} // namespace test_harness

#endif