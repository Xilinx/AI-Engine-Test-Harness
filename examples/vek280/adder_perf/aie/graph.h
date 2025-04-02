/*
 * MIT License
 *
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
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

#pragma once

#include <string>
#include <adf.h>
#include "common.h"

using namespace adf;

void aie_adder(input_buffer<int32>& in0, input_buffer<int32>& in1, output_buffer<int32> &out);

// REQUIRED: Include the test harness header file
#include "test_harness_graph.hpp"

class test_graph : public graph {
   private:
    kernel adder;

   public:
    input_plio pl_in0, pl_in1;
    output_plio pl_out;

    test_graph() {
        adder = kernel::create(aie_adder);

        // REQUIRED: Declare PLIOs using one of the predefined names, and using the predefined width of 128 bits
        pl_in0 = input_plio::create("PLIO_01_TO_AIE", adf::plio_128_bits, "data/DataIn0.txt");
        pl_in1 = input_plio::create("PLIO_03_TO_AIE", adf::plio_128_bits, "data/DataIn1.txt");
        pl_out = output_plio::create("PLIO_02_FROM_AIE", adf::plio_128_bits, "data/DataOut0.txt");

        connect<> net0(pl_in0.out[0], adder.in[0]);
        connect<> net1(pl_in1.out[0], adder.in[1]);
        connect<> net2(adder.out[0], pl_out.in[0]);

        source(adder) = "adder.cc";

        runtime<ratio>(adder) = 1;
		dimensions(adder.in[0])={4096};
		dimensions(adder.in[1])={4096};
		dimensions(adder.out[0])={4096};
    };
};
