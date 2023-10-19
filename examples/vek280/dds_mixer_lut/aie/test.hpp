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
#include "utils.hpp"

#include "uut_config.h"
#include "uut_static_config.h"

#define Q(x) #x
#define QUOTE(x) Q(x)

#ifndef UUT_GRAPH
#define UUT_GRAPH dds_mixer_lut_graph
#endif

#include QUOTE(UUT_GRAPH.hpp)
using namespace adf;

// REQUIRED: Include the test harness header file
#include "test_harness_graph.hpp"

namespace xf {
namespace dsp {
namespace aie {
namespace testcase {

class test_graph : public graph {
   private:
   public:
    std::array<input_plio, P_SSR> in1;
    std::array<input_plio, P_SSR> in2;
    std::array<output_plio, P_SSR> out;

    static constexpr unsigned int phaseInc = DDS_PHASE_INC; // single sample phase increment
    static constexpr unsigned int initialPhaseOffset = INITIAL_DDS_OFFSET;
    // Constructor
    test_graph() {
        printf("========================\n");
        printf("TEST.HPP STARTED\n");
        printf("========================\n");
        printf("== UUT Graph Class: ");
        printf(QUOTE(UUT_GRAPH));
        printf("\n");
        printf("========================\n");
        if (MIXER_MODE != 0) {
            printf("Input samples      = %d \n", INPUT_SAMPLES);
        }
        printf("Output samples     = %d \n", OUTPUT_SAMPLES);
        printf("Data type          = ");
        printf(QUOTE(DATA_TYPE));
        printf("\n");
        printf("MIXER_MODE         = %d \n", MIXER_MODE);
        printf("SFDR               = %d \n", SFDR);
        printf("P_API              = %d \n", P_API);
        printf("INPUT_WINDOW_VSIZE = %d \n", INPUT_WINDOW_VSIZE);
        printf("P_SSR              = %d \n", P_SSR);

        printf("Mixer Mode      = %d \n", MIXER_MODE);
        if (MIXER_MODE == 0) printf(" ( DDS Only Mode ) \n");
        if (MIXER_MODE == 1) printf(" ( DDS Plus Mixer (1 data input) Mode ) \n");
        if (MIXER_MODE == 2) printf(" ( DDS Plus Symmetrical Mixer (2 data input) Mode ) \n");

        printf("phaseInc is %u or 0x%08X \n", phaseInc, phaseInc);
        printf("initialPhaseOffset is %u or 0x%08X \n", initialPhaseOffset, initialPhaseOffset);

        namespace dsplib = xf::dsp::aie;
        dsplib::mixer::dds_mixer::UUT_GRAPH<DATA_TYPE, MIXER_MODE, SFDR, P_API, INPUT_WINDOW_VSIZE, P_SSR> ddsGraph(
            phaseInc, initialPhaseOffset);

        for (unsigned int i = 0; i < P_SSR; ++i) {
            std::string filenameOut = QUOTE(OUTPUT_FILE);
            std::string filenameIn1 = QUOTE(INPUT_FILE);
            std::string filenameIn2 = QUOTE(INPUT_FILE2);

#if (MIXER_MODE == 2 || MIXER_MODE == 1)
            in1[i] = input_plio::create("PLIO_01_TO_AIE", adf::plio_128_bits, filenameIn1);
            connect<>(in1[i].out[0], ddsGraph.in1[i]);
            printf("Connecting ddsGraph.in1[%d] to %s\n", i, filenameIn1.c_str());
#endif
#if (MIXER_MODE == 2)
            in2[i] = input_plio::create("PLIO_03_TO_AIE", adf::plio_128_bits, filenameIn2);
            connect<>(in2[i].out[0], ddsGraph.in2[i]);
            printf("Connecting ddsGraph.in2[%d] to %s\n", i, filenameIn2.c_str());
#endif

            out[i] = output_plio::create("PLIO_02_FROM_AIE", adf::plio_128_bits, filenameOut);
            connect<>(ddsGraph.out[i], out[i].in[0]);
            printf("Connecting ddsGraph.out[%d] to %s\n", i, filenameOut.c_str());
        }

        printf("TEST.HPP COMPLETED\n");
        printf("========================\n");
    };
};
}
}
}
};
