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
#include "m16_ssr8_dft_graph.h"
#include "m16_ssr8_filterbank_graph.h"
#include "vck190_test_harness_port_name.hpp"

using namespace adf;

// REQUIRED: Include the test harness header file
#include "vck190_test_harness_graph.hpp"

// ------------------------------------------------------------
// Top Level Graph
// ------------------------------------------------------------

class m16_ssr8_graph : public graph {
   public:
    std::array<input_plio, 8> dft_i;
    std::array<output_plio, 8> dft_o;
    std::array<input_plio, 8> fbank_i;
    std::array<output_plio, 8> fbank_o;

    // Position filterbank to the right side of DFT:
    m16_ssr8_dft_graph dut_dft;
    m16_ssr8_filterbank_graph dut_fbank;

    // Constructor:
    m16_ssr8_graph(void) : dut_dft(), dut_fbank() {
        // Connections:
        for (int ii = 0; ii < 8; ii++) {
            // Note: There are no filenames on these ::create() routines because we either connect these up to
            //       the actual PL HLS blocks in 'system.cfg' or External Traffic Generators for debug
            std::string plio_fbank_i = vck190_test_harness::in_names[16 + ii * 2];
            std::string plio_fbank_o = vck190_test_harness::out_names[16 + ii * 2 + 1];
            std::string plio_dft_i = vck190_test_harness::in_names[ii * 2];
            std::string plio_dft_o = vck190_test_harness::out_names[ii * 2 + 1];

            std::string fname_fbank_i = "data/fir_i_" + std::to_string(ii) + ".txt";
            std::string fname_fbank_o = "data/fir_o_" + std::to_string(ii) + ".txt";
            std::string fname_dft_i = "data/dft_i_" + std::to_string(ii) + ".txt";
            std::string fname_dft_o = "data/dft_o_" + std::to_string(ii) + ".txt";
            fbank_i[ii] = input_plio::create(plio_fbank_i, plio_128_bits, fname_fbank_i);
            fbank_o[ii] = output_plio::create(plio_fbank_o, plio_128_bits, fname_fbank_o);
            dft_i[ii] = input_plio::create(plio_dft_i, plio_128_bits, fname_dft_i);
            dft_o[ii] = output_plio::create(plio_dft_o, plio_128_bits, fname_dft_o);

            connect<stream>(fbank_i[ii].out[0], dut_fbank.sig_i[ii]);
            connect<stream>(dut_fbank.sig_o[ii], fbank_o[ii].in[0]);
            connect<stream>(dft_i[ii].out[0], dut_dft.sig_i[ii]);
            connect<stream>(dut_dft.sig_o[ii], dft_o[ii].in[0]);
        }
    }
};
