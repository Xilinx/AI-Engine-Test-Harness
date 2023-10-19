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

#ifndef _TEST_HARNESS_DDS_MIXER_UTILS_HPP_
#define _TEST_HARNESS_DDS_MIXER_UTILS_HPP_

/*
DDS Mixer Utilities
This file contains sets of overloaded, templatized and specialized templatized functions for use
by the main kernel class and run-time function. These functions are separate from the traits file
because they are purely for kernel use, not graph level compilation.
*/

#include <stdio.h>
#include <adf.h>
#include "dds_mixer.hpp"
#include "aie_api/aie_adf.hpp"

namespace xf {
namespace dsp {
namespace aie {
namespace mixer {
namespace dds_mixer {

template <typename TT_DATA, typename PortType>
template <unsigned int VECTOR_LEN>
auto INLINE_DECL T_IFbase<TT_DATA, PortType>::port_readincr(PortType* in) {
    // check which IO API we should use
    if
        constexpr(std::is_same_v<PortType, input_window<TT_DATA> >) { // todo - check if we should use is_convertable or
                                                                      // layout_compatible to handle inout_window or
                                                                      // something.
            return window_readincr_v<VECTOR_LEN>(in);
        }
    else if
        constexpr(std::is_same_v<PortType, input_stream<TT_DATA> >) {
            // stream
            return readincr_v<VECTOR_LEN>(in);
        }
}

// this should also work for cascade
template <typename TT_DATA, typename PortType>
template <typename OutDType>
void INLINE_DECL T_IFbase<TT_DATA, PortType>::port_writeincr(PortType* out, OutDType data) {
    // check which IO API we should use
    if
        constexpr(std::is_same_v<PortType, output_window<TT_DATA> >) { // todo - check if we should use is_convertable
                                                                       // or layout_compatible to handle inout_window or
                                                                       // something.
            return window_writeincr(out, data);
        }
    else if
        constexpr(std::is_same_v<PortType, output_stream<TT_DATA> >) {
            // stream
            return writeincr(out, data);
        }
}

template <unsigned int numTables, unsigned int tableRes, unsigned int lutMask, unsigned int numLanes, typename TT_DATA>
auto INLINE_DECL computeDDSOut(unsigned int m_phaseIndex, cint32** sincosLUT) {
    ::aie::accum<cacc64, 4> ddsOutLUT;
    ::aie::vector<cint32, 4> sincosVal[numTables];
#pragma unroll numTables
    for (int i = 0; i < numTables; i++) {
        sincosVal[i][0] = sincosLUT[i][(m_phaseIndex >> (32 - 10 * (i + 1))) & 0x000003FF];
    }
    if
        constexpr(numTables == 1) {
            ddsOutLUT.from_vector(sincosVal[0], 0);
            return ddsOutLUT;
        }
    else if
        constexpr(numTables == 2) { return ::aie::mul(sincosVal[0], sincosVal[1]); }
    else if
        constexpr(numTables == 3) {
            ::aie::accum<cacc64, 4> ddsAccInter;
            ddsAccInter = ::aie::mul(sincosVal[0], sincosVal[1]);
            ddsOutLUT = ::aie::mul(sincosVal[2], ddsAccInter.template to_vector<TT_DATA>(31));
            return ddsOutLUT;
        }
};

#
template <unsigned int numTables, unsigned int tableRes, unsigned int lutMask, unsigned int numLanes, typename TT_DATA>
auto INLINE_DECL computeDDSOutFloat(unsigned int m_phaseIndex, cfloat** sincosLUT) {
#if __SUPPORTS_CFLOAT__ == 1
    ::aie::vector<cfloat, 4> ddsOutLUT;
    ::aie::vector<cfloat, 4> sincosVal[numTables];
#pragma unroll numTables
    for (int i = 0; i < numTables; i++) {
        sincosVal[i][0] = sincosLUT[i][(m_phaseIndex >> (32 - 10 * (i + 1))) & 0x000003FF];
    }
    if
        constexpr(numTables == 1) { return sincosVal[0].get(0); }
    else if
        constexpr(numTables == 2) {
            ddsOutLUT = ::aie::mul(sincosVal[0], sincosVal[1]);
            return ddsOutLUT.get(0);
        }
    else if
        constexpr(numTables == 3) {
            ::aie::vector<cfloat, 4> ddsAccInter;
            ddsAccInter = ::aie::mul(sincosVal[0], sincosVal[1]);
            ddsOutLUT = ::aie::mul(sincosVal[2], ddsAccInter);
            return ddsOutLUT.get(0);
        }
#endif
};
}
}
}
}
}

#endif // _TEST_HARNESS_DDS_MIXER_UTILS_HPP_
