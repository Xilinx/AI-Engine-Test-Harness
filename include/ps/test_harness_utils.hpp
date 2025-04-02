/*
 * MIT License
 *
 * Copyright (C) 2023-2025 Advanced Micro Devices, Inc.
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
 
#ifndef _TEST_HARNESS_UTILS_HPP_
#define _TEST_HARNESS_UTILS_HPP_

#include <iostream>
#include <vector>
#include <cstdlib>
#include <stdexcept>

#define mc_stringfy(s) mc_str(s)
#define mc_str(s) #s

namespace test_harness {

enum TestMode : uint8_t { FUNC_MODE = 0, PERF_MODE };

enum channel_index : uint16_t {
    PLIO_01_TO_AIE,
    PLIO_02_TO_AIE,
    PLIO_03_TO_AIE,
    PLIO_04_TO_AIE,
    PLIO_05_TO_AIE,
    PLIO_06_TO_AIE,
    PLIO_07_TO_AIE,
    PLIO_08_TO_AIE,
    PLIO_09_TO_AIE,
    PLIO_10_TO_AIE,
    PLIO_11_TO_AIE,
    PLIO_12_TO_AIE,
    PLIO_13_TO_AIE,
    PLIO_14_TO_AIE,
    PLIO_15_TO_AIE,
    PLIO_16_TO_AIE,
    PLIO_17_TO_AIE,
    PLIO_18_TO_AIE,
    PLIO_19_TO_AIE,
    PLIO_20_TO_AIE,
    PLIO_21_TO_AIE,
    PLIO_22_TO_AIE,
    PLIO_23_TO_AIE,
    PLIO_24_TO_AIE,
    PLIO_25_TO_AIE,
    PLIO_26_TO_AIE,
    PLIO_27_TO_AIE,
    PLIO_28_TO_AIE,
    PLIO_29_TO_AIE,
    PLIO_30_TO_AIE,
    PLIO_31_TO_AIE,
    PLIO_32_TO_AIE,
    PLIO_33_TO_AIE,
    PLIO_34_TO_AIE,
    PLIO_35_TO_AIE,
    PLIO_36_TO_AIE,
    PLIO_01_FROM_AIE,
    PLIO_02_FROM_AIE,
    PLIO_03_FROM_AIE,
    PLIO_04_FROM_AIE,
    PLIO_05_FROM_AIE,
    PLIO_06_FROM_AIE,
    PLIO_07_FROM_AIE,
    PLIO_08_FROM_AIE,
    PLIO_09_FROM_AIE,
    PLIO_10_FROM_AIE,
    PLIO_11_FROM_AIE,
    PLIO_12_FROM_AIE,
    PLIO_13_FROM_AIE,
    PLIO_14_FROM_AIE,
    PLIO_15_FROM_AIE,
    PLIO_16_FROM_AIE,
    PLIO_17_FROM_AIE,
    PLIO_18_FROM_AIE,
    PLIO_19_FROM_AIE,
    PLIO_20_FROM_AIE,
    PLIO_21_FROM_AIE,
    PLIO_22_FROM_AIE,
    PLIO_23_FROM_AIE,
    PLIO_24_FROM_AIE,
    PLIO_25_FROM_AIE,
    PLIO_26_FROM_AIE,
    PLIO_27_FROM_AIE,
    PLIO_28_FROM_AIE,
    PLIO_29_FROM_AIE,
    PLIO_30_FROM_AIE,
    PLIO_31_FROM_AIE,
    PLIO_32_FROM_AIE,
    PLIO_33_FROM_AIE,
    PLIO_34_FROM_AIE,
    PLIO_35_FROM_AIE,
    PLIO_36_FROM_AIE
};

class test_harness_args {
    public:
    channel_index idx;
    size_t size_in_byte;
    size_t repetition;
    uint64_t delay;
    char* data;
    test_harness_args(channel_index idx, size_t size_in_byte, size_t repetition, uint64_t delay, char* data)
        : idx(idx), size_in_byte(size_in_byte), repetition(repetition), delay(delay), data(data) {}
};

}

#endif
