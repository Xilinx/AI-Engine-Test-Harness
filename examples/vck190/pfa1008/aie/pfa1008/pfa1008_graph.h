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

#include <adf.h>
#include <vector>

#include "dft7_graph.h"
#include "dft9_graph.h"
#include "dft16_graph.h"

#include "test_harness_graph.hpp"

using namespace adf;

class pfa1008_graph : public graph {
   public:
    // Declare vector of PLIO ports:
    std::array<input_plio, 2> dft7_i;
    std::array<output_plio, 2> dft7_o;
    std::array<input_plio, 2> dft9_i;
    std::array<output_plio, 2> dft9_o;
    std::array<input_plio, 2> dft16_i;
    std::array<output_plio, 2> dft16_o;

    // Declare all graphs:
    dft7_graph dft7;
    dft9_graph dft9;
    dft16_graph dft16;

    // Graph constructor:
    pfa1008_graph(void) {
        for (int ii = 0; ii < 2; ii++) {
            std::string pname_dft7_i = test_harness::in_names[ii * 6];
            std::string pname_dft7_o = test_harness::out_names[ii * 6 + 1];
            std::string pname_dft9_i = test_harness::in_names[ii * 6 + 2];
            std::string pname_dft9_o = test_harness::out_names[ii * 6 + 3];
            std::string pname_dft16_i = test_harness::in_names[ii * 6 + 4];
            std::string pname_dft16_o = test_harness::out_names[ii * 6 + 5];

            std::string fname_dft7_i = "data/dft7_i_" + std::to_string(ii) + ".txt";
            std::string fname_dft7_o = "data/dft7_o_" + std::to_string(ii) + ".txt";
            std::string fname_dft9_i = "data/dft9_i_" + std::to_string(ii) + ".txt";
            std::string fname_dft9_o = "data/dft9_o_" + std::to_string(ii) + ".txt";
            std::string fname_dft16_i = "data/dft16_i_" + std::to_string(ii) + ".txt";
            std::string fname_dft16_o = "data/dft16_o_" + std::to_string(ii) + ".txt";
            dft7_i[ii] = input_plio::create(pname_dft7_i, plio_128_bits, fname_dft7_i);
            dft7_o[ii] = output_plio::create(pname_dft7_o, plio_128_bits, fname_dft7_o);
            dft9_i[ii] = input_plio::create(pname_dft9_i, plio_128_bits, fname_dft9_i);
            dft9_o[ii] = output_plio::create(pname_dft9_o, plio_128_bits, fname_dft9_o);
            dft16_i[ii] = input_plio::create(pname_dft16_i, plio_128_bits, fname_dft16_i);
            dft16_o[ii] = output_plio::create(pname_dft16_o, plio_128_bits, fname_dft16_o);

            // Connect inputs and outputs:
            connect<stream, stream>(dft7_i[ii].out[0], dft7.sig_i[ii]);
            connect<stream, stream>(dft7.sig_o[ii], dft7_o[ii].in[0]);

            connect<stream, stream>(dft9_i[ii].out[0], dft9.sig_i[ii]);
            connect<stream, stream>(dft9.sig_o[ii], dft9_o[ii].in[0]);

            connect<stream, stream>(dft16_i[ii].out[0], dft16.sig_i[ii]);
            connect<stream, stream>(dft16.sig_o[ii], dft16_o[ii].in[0]);
        } // ii
    }
};
