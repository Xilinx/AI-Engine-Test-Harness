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
#include "include.h"
#include "kernels.h"

using namespace adf;

// REQUIRED: Include the test harness header file
#include "vck190_test_harness_graph.hpp"

class clipped : public adf::graph {
   private:
    kernel interpolator;
    kernel classify;
    kernel clip;

   public:
    // Declaring PLIO objects
    adf::input_plio in;
    adf::output_plio out;

    clipped() {
        // kernel instantiation
        interpolator = adf::kernel::create(fir_27t_sym_hb_2i);
        clip = adf::kernel::create(polar_clip);
        classify = adf::kernel::create(classifier);

        adf::source(interpolator) = "kernels/interpolators/hb27_2i.cc";
        adf::source(clip) = "kernels/polar_clip.cpp";
        adf::source(classify) = "kernels/classifiers/classify.cc";

        // REQUIRED: Declare PLIOs using one of the predefined names, and using the predefined width of 128 bits
        in = input_plio::create("PLIO_01_TO_AIE", adf::plio_128_bits, "input.txt");
        out = output_plio::create("PLIO_01_FROM_AIE", adf::plio_128_bits, "output.txt");

        // Window connection between port 'in' and input port of the kernel
        connect<window<INTERPOLATOR27_INPUT_BLOCK_SIZE, INTERPOLATOR27_INPUT_MARGIN> >(in.out[0], interpolator.in[0]);

        connect<window<POLAR_CLIP_INPUT_BLOCK_SIZE>, stream>(interpolator.out[0], clip.in[0]);

        // stream connection between kernels
        connect<stream>(clip.out[0], classify.in[0]);

        // window connection between output port of the kernel and port 'out'
        connect<window<CLASSIFIER_OUTPUT_BLOCK_SIZE> >(classify.out[0], out.in[0]);

        std::vector<std::string> myheaders;
        myheaders.push_back("include.h");

        adf::headers(interpolator) = myheaders;
        adf::headers(classify) = myheaders;

        // Specifying core usage fraction for a kernel
        runtime<ratio>(interpolator) = 0.8;
        runtime<ratio>(clip) = 0.8;
        runtime<ratio>(classify) = 0.8;
    };
};
