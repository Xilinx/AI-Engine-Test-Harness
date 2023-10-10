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

#ifndef _TEST_HARNESS_DDS_MIXER_LUT_GRAPH_HPP_
#define _TEST_HARNESS_DDS_MIXER_LUT_GRAPH_HPP_
/*
The file captures the definition of the 'L2' graph level class for
the DDS_MIXER library element.
*/

#include <adf.h>
#include <vector>
#include <tuple>

#include "dds_mixer.hpp"

alignas(32) extern const cint32_t sincosLUTCoarse32[1024];
alignas(32) extern const cint32_t sincosLUTFine32[1024];
#if __SUPPORTS_CFLOAT__ == 1
alignas(32) extern const cfloat sincosLUTFloat1[1024];
alignas(32) extern const cfloat sincosLUTFloat2[1024];
alignas(32) extern const cfloat sincosLUTFloat3[1024];
#endif

namespace xf {
namespace dsp {
namespace aie {
namespace mixer {
namespace dds_mixer {
using namespace adf;

/**
 * @addtogroup dds_graph DDS / Mixer
 *
 * DDS contains a DDS and Mixer solution.
 *
 */

//--------------------------------------------------------------------------------------------------
// dds_mixer_lut_graph template
//--------------------------------------------------------------------------------------------------
/**
 * @brief dds_mixer operates in 3 modes: \n
 *      **Mixer Mode 0:** \n
 *                       This is dds mode only. The library element has a single output window,
 *                       which is written to with the sin/cos components corresponding to the
 *                       programmed phase increment. \n
 *      **Mixer Mode 1:** \n
 *                       This is dds plus mixer for a single data input port. \n Each data input
 *                       sample is complex multiplied with the corresponding dds sample, to
 *                       create a modulated signal that is written to the output window. \n
 *      **Mixer Mode 2:** \n
 *                       This is a special configuration for symmetrical carriers and two data
 *                       input ports. \n Each data sample of the first input is complex multiplied
 *                       with the corresponding dds sample to create a modulated signal. \n
 * These are the templates to configure the dds_mixer class. \n
 *
 * @ingroup dds_graph
 *
 * @tparam TT_DATA describes the type of individual data samples input to and
 *         output from the dds_mixer function. This is a typename and must be one
 *         of the following: \n
 *         AIE1 : cint16, cint32, cfloat
 *         AIE2 : cint16, cint32
 * @tparam TP_MIXER_MODE describes the mode of operation of the dds_mixer.  \n
 *         The values supported are: \n
 *         0 (dds only mode), \n 1 (dds plus single data channel mixer),  \n
 *         2 (dds plus two data channel mixer for symmetrical carriers)
 * @tparam TP_SFDR specifies the expected Spurious Free Dynamic Range that the useR expects from the generated  \n
 *         design. There are three distinct implementations available that offer a tradeoff between SFDR and
 *performance. \n
 *         The SFDR levels are at ~60dB, ~120dB, and ~180dB. The maximum theoretical SFDR is 96dB for cint16, 180dB for
 *cint32 and 138dB for cfloat.
 * @tparam TP_API specifies if the input/output interface should be buffer-based or stream-based.  \n
 *         The values supported are 0 (buffer API) or 1 (stream API).
 * @tparam TP_INPUT_WINDOW_VSIZE describes the number of samples in the input/output buffer API \n
 *          or number of samples to process per iteration. DDS Mixer TP_INPUT_WINDOW_VSIZE must be a multiple of data
 *output in each loop. \n
 *          For AIE1, this has to be 64 for cint16, 16 for cint32, 4 for cfloat. For AIE2, this has to be 256 for cint16
 *and 64 for cint32.
 * @tparam TP_SSR specifies the super sample rate, ie how much data input/output in parallel for a single channel.  \n
 *         There will be a TP_SSR number of kernels, with a TP_SSR number of each port used on the interface. \n
 *         A default value of 1 corresponds to the typical single kernel case.
 **/
template <typename TT_DATA,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_SFDR = 90,
          unsigned int TP_API = IO_API::WINDOW,
          unsigned int TP_INPUT_WINDOW_VSIZE = 256,
          unsigned int TP_SSR = 1>
class dds_mixer_lut_graph : public graph {
   private:
   public:
    parameter ddsLut1;
    parameter ddsLut2;
    parameter ddsLut3;
#if __SUPPORTS_CFLOAT__ == 0
    static_assert(!(std::is_same<TT_DATA, cfloat>::value), "AIE2 does not support cfloat data type");
    static_assert(!(TP_MIXER_MODE == 2 && TP_API == 1),
                  "AIE2 does not have two input streams support, so mixer mode 2 is not supported with TP_API = 1");
#endif
    static_assert(TP_SSR > 0, "ERROR: Invalid SSR value, must be a value greater than 0.\n");
    static_assert(!(TP_SFDR >= 96 && std::is_same<TT_DATA, cint16>()),
                  "ERROR: Maximum SFDR with a cint16 data type is limited to 96dB. Please switch to cint32 data type "
                  "to enable higher SFDR.");
    static_assert(TP_SFDR <= 180, "ERROR: Maximum SFDR possible is 180dB.");
    static_assert(TP_INPUT_WINDOW_VSIZE % TP_SSR == 0,
                  "ERROR: Unsupported frame size. TP_INPUT_WINDOW_VSIZE must be divisible by TP_SSR");

    template <typename direction>
    using portArray = std::array<port<direction>, TP_SSR>;

    /**
     * The input data to the function. When in TP_API=WINDOW, the port is a window of
     * samples of TT_DATA type. The number of samples in the window is
     * described by TP_INPUT_WINDOW_VSIZE.
     **/
    portArray<input> in1;
    portArray<input> in2;
    /**
     * An output port of TT_DATA type. When in TP_API=WINDOW, the port is a window of TP_INPUT_WINDOW_VSIZE samples.
     **/
    portArray<output> out;

    /**
     * kernel instance used to set constraints - getKernels function returns a pointer to this.
    **/
    kernel m_ddsKernel[TP_SSR];

    /**
     * Access function for getting kernel - useful for setting runtime ratio,
     * location constraints, fifo_depth (for stream), etc.
    **/
    kernel* getKernels() { return m_ddsKernel; };
    static constexpr unsigned int KINPUT_WINDOW_VSIZE = TP_INPUT_WINDOW_VSIZE / TP_SSR;

    template <unsigned int TP_NUM_TABLES = 1>
    using kernelClass = dds_mixer<TT_DATA, KINPUT_WINDOW_VSIZE, TP_MIXER_MODE, TP_API, USE_LUT_SINCOS, TP_NUM_TABLES>;

    /**
     * @brief This is the constructor function for the dds_mixer graph.
     * @param[in] phaseInc specifies the phase increment between samples.
     *            Input value 2^31 corresponds to Pi (i.e. 180').
     * @param[in] initialPhaseOffset specifies the initial value of the phase accumulator, creating a phase offset.
     *                                 Input value 2^31 corresponds to Pi (i.e. 180').
     **/
    dds_mixer_lut_graph(const uint32_t phaseInc, const uint32_t initialPhaseOffset = 0) {
        if
            constexpr(std::is_same<TT_DATA, cint16>::value || std::is_same<TT_DATA, cint32>::value) {
                ddsLut1 = parameter::array(sincosLUTCoarse32);
                ddsLut2 = parameter::array(sincosLUTFine32);
                ddsLut3 = parameter::array(sincosLUTFiner32);
            }
        else {
#if __SUPPORTS_CFLOAT__ == 1
            ddsLut1 = parameter::array(sincosLUTFloat1);
            ddsLut2 = parameter::array(sincosLUTFloat2);
            ddsLut3 = parameter::array(sincosLUTFloat3);
#endif
        }
        static constexpr unsigned int t_kNumLUTs = TP_SFDR <= 60 ? 1 : TP_SFDR <= 120 ? 2 : 3;
        for (unsigned int ssrIdx = 0; ssrIdx < TP_SSR; ssrIdx++) {
            m_ddsKernel[ssrIdx] = kernel::create_object<kernelClass<t_kNumLUTs> >(
                uint32_t(phaseInc * TP_SSR), uint32_t(initialPhaseOffset + phaseInc * ssrIdx));
            if
                constexpr(TP_MIXER_MODE == 1 || TP_MIXER_MODE == 2) {
                    if
                        constexpr(TP_API == 0) {
                            connect<>(in1[ssrIdx], m_ddsKernel[ssrIdx].in[0]);
                            dimensions(m_ddsKernel[ssrIdx].in[0]) = {KINPUT_WINDOW_VSIZE};
                        }
                    else {
                        connect<stream>(in1[ssrIdx], m_ddsKernel[ssrIdx].in[0]);
                    }
                }
            if
                constexpr(TP_MIXER_MODE == 2) {
                    if
                        constexpr(TP_API == 0) {
                            connect<>(in2[ssrIdx], m_ddsKernel[ssrIdx].in[1]);
                            dimensions(m_ddsKernel[ssrIdx].in[1]) = {KINPUT_WINDOW_VSIZE};
                        }
                    else {
                        connect<stream>(in2[ssrIdx], m_ddsKernel[ssrIdx].in[1]);
                    }
                }

            connect<>(ddsLut1, m_ddsKernel[ssrIdx]);
            if
                constexpr(t_kNumLUTs > 1) { connect<>(ddsLut2, m_ddsKernel[ssrIdx]); }
            if
                constexpr(t_kNumLUTs > 2) { connect<>(ddsLut3, m_ddsKernel[ssrIdx]); }
            if
                constexpr(TP_API == 0) {
                    connect<>(m_ddsKernel[ssrIdx].out[0], out[ssrIdx]);
                    dimensions(m_ddsKernel[ssrIdx].out[0]) = {KINPUT_WINDOW_VSIZE};
                }
            else {
                connect<stream>(m_ddsKernel[ssrIdx].out[0], out[ssrIdx]);
            }
            // Specify mapping constraints
            runtime<ratio>(m_ddsKernel[ssrIdx]) = 0.8;
            // Source files
            source(m_ddsKernel[ssrIdx]) = "dds_mixer.cpp";
        }
    }
};
}
}
}
}
} // namespace braces
#endif //_TEST_HARNESS_DDS_MIXER_LUT_GRAPH_HPP_
