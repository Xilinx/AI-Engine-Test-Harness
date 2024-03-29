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
#include "matrix_mult_graph.hpp"

using namespace adf;

// REQUIRED: Include the test harness header file
#include "test_harness_graph.hpp"

#define GEMM_SIZE 32
#define SPLIT 8
#define CASC_LN 4
#define N_SAMPLES 1
#define DIM_A (GEMM_SIZE / (SPLIT))
#define DIM_AB (GEMM_SIZE)
#define DIM_B (GEMM_SIZE / SPLIT)
#define ITER_CNT 1
#define GRAPH_ITER_CNT (SPLIT * ITER_CNT)

#define WINDOW_SIZE (DIM_A * DIM_AB * N_SAMPLES)

extern int base_col, base_row;

class GeMM : public adf::graph {
   public:
    input_plio matA_inp[CASC_LN];
    input_plio matB_inp[(SPLIT * CASC_LN)];
    output_plio matC_out[SPLIT];

    GeMM() {
        // GeMM Graph Declarations...
        xf::dsp::aie::blas::matrix_mult::matrix_mult_graph<int16, int16, DIM_A, DIM_AB, DIM_B, 0, 0, ROW_MAJOR,
                                                           ROW_MAJOR, ROW_MAJOR, 0, 0, 0, WINDOW_SIZE, WINDOW_SIZE,
                                                           CASC_LN>
            mmult[SPLIT];

        // Mat A PLIO node names...
        for (int j = 0; j < CASC_LN; ++j) {
            std::string matA_plioOut = test_harness::in_names[j];

            std::string matA_Out_file = "data/a" + std::to_string(0) + "_casc" + std::to_string(j) + ".txt";

            matA_inp[j] = input_plio::create(matA_plioOut, plio_128_bits, matA_Out_file);
        }

        for (int i = 0; i < SPLIT; ++i) {
            // CASC_LN No. of kernels will be created...
            adf::kernel* mmult_kernels = mmult[i].getKernels();

            for (int j = 0; j < CASC_LN; ++j) {
                // Mat B PLIO node names...
                std::string matB_plioOut = test_harness::in_names[CASC_LN + i * CASC_LN + j];

                std::string matB_Out_file = "data/b" + std::to_string(i) + "_casc" + std::to_string(j) + ".txt";

                matB_inp[(i * CASC_LN) + j] = input_plio::create(matB_plioOut, plio_128_bits, matB_Out_file);
            }

            // Mat C PLIO node names...
            std::string matC_plioOut = test_harness::out_names[i];

            std::string matC_Out_file = "data/c" + std::to_string(i) + ".txt";

            // Creating PLIO nodes...
            matC_out[i] = output_plio::create(matC_plioOut, plio_128_bits, matC_Out_file);

            // Connecting PLIO Nodes...
            for (int k = 0; k < CASC_LN; ++k) {
                // Setting runtime ratio...
                adf::runtime<ratio>(mmult_kernels[k]) = 0.8;

                // Connecting port IO nodes...
                adf::connect<>(matA_inp[k].out[0], mmult[i].inA[k]);
                adf::connect<>(matB_inp[(i * CASC_LN) + k].out[0], mmult[i].inB[k]);
            }

            // Connecting port IO nodes...
            adf::connect<>(mmult[i].out, matC_out[i].in[0]);
        }
    }
};
