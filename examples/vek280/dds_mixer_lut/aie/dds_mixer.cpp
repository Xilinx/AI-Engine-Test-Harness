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

/*
DDS Mixer code.
This file captures the body of run-time code for the kernal class.

Coding conventions
  TT_      template type suffix
  TP_      template parameter suffix
*/

#include <adf.h>
#include "device_defs.h"

#define __AIE_API_USE_NATIVE_1024B_VECTOR__
#include "aie_api/aie_adf.hpp"

#include "dds_mixer_traits.hpp"
#include "dds_mixer.hpp"
#include "dds_mixer_utils.hpp"
#include "kernel_api_utils.hpp"
#include "aie_api/utils.hpp"

namespace xf {
namespace dsp {
namespace aie {
namespace mixer {
namespace dds_mixer {

// aie_api is external to xf::dsp::aie namespace
namespace aie = ::aie;

template <unsigned int TP_SHIFT, unsigned int TP_API, unsigned int numLanes, typename TT_DATA, typename T_acc>
INLINE_DECL void ddsWriteOp(aie::vector<TT_DATA, numLanes>* __restrict(&outPtr),
                            output_stream<TT_DATA>* __restrict(&outStrm),
                            T_acc(&accOut)) {
    if
        constexpr(std::is_same<TT_DATA, cint16>::value || std::is_same<TT_DATA, cint32>::value) {
            if
                constexpr(TP_API == 0) { *outPtr++ = accOut.template to_vector<TT_DATA>(TP_SHIFT); }
            else {
                writeincr<aie_stream_resource_out::a, TT_DATA, numLanes>(outStrm,
                                                                         accOut.template to_vector<TT_DATA>(TP_SHIFT));
            }
        }
    else {
        if
            constexpr(TP_API == 0) { *outPtr++ = accOut; }
        else {
            writeincr<aie_stream_resource_out::a, TT_DATA, numLanes>(outStrm, accOut);
        }
    }
}

template <unsigned int TP_API, unsigned int numLanes, typename TT_DATA>
INLINE_DECL void ddsReadIp(aie::vector<TT_DATA, numLanes>* __restrict(&in0Ptr),
                           input_stream<TT_DATA>* __restrict(&inStrm),
                           aie::vector<TT_DATA, numLanes>& inData) {
    if
        constexpr(TP_API == 0) { inData = *in0Ptr++; }
    else {
        inData = readincr_v<numLanes, aie_stream_resource_in::a>(inStrm);
    }
}
//==============================================================================
// integer specializations

// Constructor to populate m_phRot array
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
kernelDdsMixerClass<TT_DATA, TP_INPUT_WINDOW_VSIZE, TP_MIXER_MODE, TP_API, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>::
    kernelDdsMixerClass(uint32_t phaseInc) {
    m_phaseIndex = 0;
    m_perCyclePhaseInc = phaseInc * m_kNumLanes;
    for (unsigned int i = 0; i < m_kNumLanes; i++) {
#if __SINCOS_IN_HW__ == 1
        m_phRot[i] = aie::sincos_complex(i * phaseInc);
#endif
    }
}

// Constructor to populate m_phRot array
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
kernelDdsMixerClass<TT_DATA,
                    TP_INPUT_WINDOW_VSIZE,
                    TP_MIXER_MODE,
                    TP_API,
                    USE_LUT_SINCOS,
                    TP_NUM_LUTS,
                    TP_RND,
                    TP_SAT>::kernelDdsMixerClass(uint32_t phaseInc) {
    if
        constexpr(!((std::is_same<TT_DATA, cint16>::value) || (std::is_same<TT_DATA, cint32>::value))) {
#if __SUPPORTS_CFLOAT__ == 1
            m_phaseIndex = 0;
            set_rnd(rnd_sym_inf); // round to symmetric infinity
            // set_rnd_mode<TP_RND>();
            // set_sat_mode<TP_SAT>();
            aie::vector<TT_DATA, m_kNumLanes> sincosVal[TP_NUM_LUTS];
            aie::vector<TT_DATA, m_kNumLanes> tempOut;

            sincosLUT[0] = (TT_DATA*)sincosLUTFloat1;
            if
                constexpr(TP_NUM_LUTS > 1) { sincosLUT[1] = (TT_DATA*)sincosLUTFloat2; }
            if
                constexpr(TP_NUM_LUTS > 2) { sincosLUT[2] = (TT_DATA*)sincosLUTFloat3; }
            for (unsigned int t = 0; t < TP_NUM_LUTS; t++) {
                for (unsigned int i = 0; i < m_kNumLanes; i++) {
                    sincosVal[t][i] = sincosLUT[t][((i * phaseInc) >> (32 - 10 * (t + 1))) & 0x000003FF];
                }
            }

            if
                constexpr(TP_NUM_LUTS == 1) { tempOut = sincosVal[0]; }
            else if
                constexpr(TP_NUM_LUTS == 2) { tempOut = aie::mul(sincosVal[0], sincosVal[1]); }
            else if
                constexpr(TP_NUM_LUTS == 3) {
                    tempOut = aie::mul(sincosVal[0], sincosVal[1]);
                    tempOut = aie::mul(tempOut, sincosVal[2]);
                }

            for (int i = 0; i < m_kNumLanes; i++) {
                m_phRotSmall[i] = tempOut[i];
            }

            m_perCyclePhaseInc = phaseInc * m_kNumLanes * m_kNumLanes;
            const int m_bFanStepSize = phaseInc * m_kNumLanes;
            for (unsigned int t = 0; t < TP_NUM_LUTS; t++) {
                for (unsigned int i = 0; i < m_kNumLanes; i++) {
                    sincosVal[t][i] = sincosLUT[t][((i * m_bFanStepSize) >> (32 - 10 * (t + 1))) & 0x000003FF];
                }
            }

            if
                constexpr(TP_NUM_LUTS == 1) { tempOut = sincosVal[0]; }
            else if
                constexpr(TP_NUM_LUTS == 2) { tempOut = aie::mul(sincosVal[0], sincosVal[1]); }
            else if
                constexpr(TP_NUM_LUTS == 3) {
                    tempOut = aie::mul(sincosVal[0], sincosVal[1]);
                    tempOut = aie::mul(tempOut, sincosVal[2]);
                }

            for (int i = 0; i < m_kNumLanes; i++) {
                m_phRotBig[i] = tempOut[i];
            }

#endif // SUPPORTS_CFLOAT
        }
    else {
        m_phaseIndex = 0;
        set_rnd(rnd_sym_inf); // round to symmetric infinity
        // set_sat_mode<TP_SAT>();
        using T_LUT_TYPE = cint32;
        aie::vector<T_LUT_TYPE, m_kNumLanes> sincosVal[TP_NUM_LUTS];
        ::aie::accum<cacc64, m_kNumLanes> tempVal;
        aie::vector<TT_DATA, m_kNumLanes> tempOut;

        sincosLUT[0] = (T_LUT_TYPE*)sincosLUTCoarse32;
        if
            constexpr(TP_NUM_LUTS > 1) { sincosLUT[1] = (T_LUT_TYPE*)sincosLUTFine32; }
        if
            constexpr(TP_NUM_LUTS > 2) { sincosLUT[2] = (T_LUT_TYPE*)sincosLUTFiner32; }
        for (unsigned int t = 0; t < TP_NUM_LUTS; t++) {
            for (unsigned int i = 0; i < m_kNumLanes; i++) {
                sincosVal[t][i] = sincosLUT[t][((i * phaseInc) >> (32 - 10 * (t + 1))) & 0x000003FF];
            }
        }

        const int maxProdBits = 63;
        constexpr int shiftAmt = maxProdBits - (sizeof(TT_DATA) / 2 * 8);
        if
            constexpr(TP_NUM_LUTS == 1) {
                if
                    constexpr(std::is_same<TT_DATA, cint32>()) { tempOut = sincosVal[0]; }
                else {
                    tempVal.from_vector(sincosVal[0], 0);
                    tempOut = tempVal.template to_vector<TT_DATA>(16);
                }
            }
        else if
            constexpr(TP_NUM_LUTS == 2) {
                tempVal = aie::mul(sincosVal[0], sincosVal[1]);
                tempOut = tempVal.template to_vector<TT_DATA>(shiftAmt);
            }
        else if
            constexpr(TP_NUM_LUTS == 3) {
                tempVal = aie::mul(sincosVal[0], sincosVal[1]);
                tempOut = tempVal.template to_vector<TT_DATA>(31);
                tempVal = aie::mul(tempOut, sincosVal[2]);
                tempOut = tempVal.template to_vector<TT_DATA>(shiftAmt);
            }

        for (int i = 0; i < m_kNumLanes; i++) {
            m_phRotSmall[i] = tempOut[i];
        }

        m_perCyclePhaseInc = phaseInc * m_kNumLanes * m_kNumLanes;
        const int m_bFanStepSize = phaseInc * m_kNumLanes;
        for (unsigned int t = 0; t < TP_NUM_LUTS; t++) {
            for (unsigned int i = 0; i < m_kNumLanes; i++) {
                sincosVal[t][i] = sincosLUT[t][((i * m_bFanStepSize) >> (32 - 10 * (t + 1))) & 0x000003FF];
            }
        }

        if
            constexpr(TP_NUM_LUTS == 1) {
                if
                    constexpr(std::is_same<TT_DATA, cint32>()) { tempOut = sincosVal[0]; }
                else {
                    tempVal.from_vector(sincosVal[0], 0);
                    tempOut = tempVal.template to_vector<TT_DATA>(16);
                }
            }
        else if
            constexpr(TP_NUM_LUTS == 2) {
                tempVal = aie::mul(sincosVal[0], sincosVal[1]);
                tempOut = tempVal.template to_vector<TT_DATA>(shiftAmt);
            }
        else if
            constexpr(TP_NUM_LUTS == 3) {
                tempVal = aie::mul(sincosVal[0], sincosVal[1]);
                tempOut = tempVal.template to_vector<TT_DATA>(31);
                tempVal = aie::mul(tempOut, sincosVal[2]);
                tempOut = tempVal.template to_vector<TT_DATA>(shiftAmt);
            }

        for (int i = 0; i < m_kNumLanes; i++) {
            m_phRotBig[i] = tempOut[i];
        }
    }
}
// Constructor to populate m_phRot array with an initial offset
// making this overload so that the default case doesn't get extra penalty of additions and sets with trivial value of
// 0.
// This enables SSR DDS
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
kernelDdsMixerClass<TT_DATA, TP_INPUT_WINDOW_VSIZE, TP_MIXER_MODE, TP_API, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>::
    kernelDdsMixerClass(uint32_t phaseInc, uint32_t initialPhaseOffset)
    : kernelDdsMixerClass(phaseInc) {
    // initialise phase accumulator index to offset.
    // enhancement? if initialPhaseOffset was a template argument, then we could just set this at initialisation without
    // penalty
    m_phaseIndex = initialPhaseOffset;
}
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
kernelDdsMixerClass<TT_DATA,
                    TP_INPUT_WINDOW_VSIZE,
                    TP_MIXER_MODE,
                    TP_API,
                    USE_LUT_SINCOS,
                    TP_NUM_LUTS,
                    TP_RND,
                    TP_SAT>::kernelDdsMixerClass(uint32_t phaseInc, uint32_t initialPhaseOffset)
    : kernelDdsMixerClass(phaseInc) {
    // initialise phase accumulator index to offset.
    // enhancement? if initialPhaseOffset was a template argument, then we could just set this at initialisation without
    // penalty
    m_phaseIndex = initialPhaseOffset;
}
// DDS_Mixer run-time function
// Overload for TP_MIXER_MODE=2
//----------------------------------------------------------------------------------------------------------------------
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
INLINE_DECL void
kernelDdsMixerClass<TT_DATA, TP_INPUT_WINDOW_VSIZE, TP_MIXER_MODE, TP_API, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>::
    ddsKernel(void* __restrict in0, void* __restrict in1, void* __restrict out0) {
#if __SINCOS_IN_HW__ == 1
    aie::vector<TT_DATA, m_kNumLanes> mixer_vdata;
    input_stream<TT_DATA>* __restrict in0Strm = (input_stream<TT_DATA>*)in0;
    input_stream<TT_DATA>* __restrict in1Strm = (input_stream<TT_DATA>*)in1;
    output_stream<TT_DATA>* __restrict outStrm = (output_stream<TT_DATA>*)out0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict in0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)in0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict in1Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)in1;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict out0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)out0;
    aie::vector<TT_DATA, m_kNumLanes> ddsOutConj;
    aie::vector<T_DDS_TYPE, m_kNumLanes> dds_out;
    aie::vector<T_DDS_TYPE, m_kNumLanes> rot_vec; // load rotation values in rot_vec
    set_sat_mode<TP_SAT>();
    set_rnd_mode<TP_RND>();
    // pointer ww to m_phRot array
    const aie::vector<T_DDS_TYPE, m_kNumLanes>* ww = (const aie::vector<T_DDS_TYPE, m_kNumLanes>*)m_phRot;
    // AIE API declarations
    // only ever use index 0 dds_out, but phrasing this as a vector seems to help the compiler pipeline a lot better.
    using T_accint16Vect0 = typename ::aie::accum<cacc48, m_kNumLanes> chess_storage(bm0);
    using T_accint32Vect0 = typename ::aie::accum<cacc48, m_kNumLanes> chess_storage(aml0);
    using T_accVect0 =
        typename std::conditional<std::is_same<TT_DATA, cint16>::value, T_accint16Vect0, T_accint32Vect0>::type;
    using T_accint16Vect1 = typename ::aie::accum<cacc48, m_kNumLanes> chess_storage(bm1);
    using T_accint32Vect1 = typename ::aie::accum<cacc48, m_kNumLanes> chess_storage(aml1);
    using T_accVect1 =
        typename std::conditional<std::is_same<TT_DATA, cint16>::value, T_accint16Vect1, T_accint32Vect1>::type;
    using T_accint16Vect2 = typename ::aie::accum<cacc48, m_kNumLanes> chess_storage(bm2);
    using T_accint32Vect2 = typename ::aie::accum<cacc80, m_kNumLanes> chess_storage(bm2);
    using T_accVect2 =
        typename std::conditional<std::is_same<TT_DATA, cint16>::value, T_accint16Vect2, T_accint32Vect2>::type;
    T_accVect0 dds_acc;
    T_accVect2 mixer_acc;
    T_accVect1 dds_conj_acc;
    rot_vec = *(ww + 0);
    for (unsigned l = 0; l < m_kLoopCount; ++l) chess_prepare_for_pipelining chess_loop_count(m_kLoopCount) {
#if __SINCOS_IN_HW__ == 1
            dds_out[0] = aie::sincos_complex(m_phaseIndex);
#endif
            m_phaseIndex += m_perCyclePhaseInc;
            dds_acc = aie::mul(rot_vec, dds_out[0]); // per sample dds output
            ddsReadIp<TP_API, m_kNumLanes, TT_DATA>(in0Ptr, in0Strm, mixer_vdata);
            mixer_acc = aie::mul(mixer_vdata, dds_acc.template to_vector<TT_DATA>(this->m_kDdsShift));
            dds_conj_acc =
                aie::mul(aie::op_conj(rot_vec), aie::op_conj(dds_out[0])); // What?? Why not conjugate dds_acc?
            ddsReadIp<TP_API, m_kNumLanes, TT_DATA>(in1Ptr, in1Strm, mixer_vdata);
            mixer_acc = aie::mac(mixer_acc, mixer_vdata, dds_conj_acc.template to_vector<TT_DATA>(this->m_kDdsShift));
            ddsWriteOp<m_kMixerShift, TP_API, m_kNumLanes>(out0Ptr, outStrm, mixer_acc);
        }
#endif
};

// DDS_Mixer run-time function - USE_LUT_SINCOS
// Overload for TP_MIXER_MODE=2
//----------------------------------------------------------------------------------------------------------------------
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
INLINE_DECL void kernelDdsMixerClass<TT_DATA,
                                     TP_INPUT_WINDOW_VSIZE,
                                     TP_MIXER_MODE,
                                     TP_API,
                                     USE_LUT_SINCOS,
                                     TP_NUM_LUTS,
                                     TP_RND,
                                     TP_SAT>::ddsKernel(void* __restrict in0,
                                                        void* __restrict in1,
                                                        void* __restrict out0) {
    aie::vector<TT_DATA, m_kNumLanes> mixer_vdata;
    input_stream<TT_DATA>* __restrict in0Strm = (input_stream<TT_DATA>*)in0;
    input_stream<TT_DATA>* __restrict in1Strm = (input_stream<TT_DATA>*)in1;
    output_stream<TT_DATA>* __restrict outStrm = (output_stream<TT_DATA>*)out0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict in0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)in0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict in1Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)in1;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict out0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)out0;
    aie::vector<TT_DATA, m_kNumLanes> ddsOutConj;
    set_sat_mode<TP_SAT>();
    using T_accint16Vect0 = typename ::aie::accum<cacc32, m_kNumLanes>;
    using T_accint32Vect0 = typename ::aie::accum<cacc64, m_kNumLanes>;
    using T_accVect0 =
        typename std::conditional<std::is_same<TT_DATA, cint16>::value, T_accint16Vect0, T_accint32Vect0>::type;
    using T_accint16Vect1 = typename ::aie::accum<cacc32, m_kNumLanes>;
    using T_accint32Vect1 = typename ::aie::accum<cacc64, m_kNumLanes>;
    using T_accVect1 =
        typename std::conditional<std::is_same<TT_DATA, cint16>::value, T_accint16Vect1, T_accint32Vect1>::type;
    using T_accint16Vect2 = typename ::aie::accum<cacc32, m_kNumLanes>;
    using T_accint32Vect2 = typename ::aie::accum<cacc64, m_kNumLanes>;
    using T_accVect2 =
        typename std::conditional<std::is_same<TT_DATA, cint16>::value, T_accint16Vect2, T_accint32Vect2>::type;
    T_accVect0 dds_acc;
    T_accVect2 mixer_acc;
    T_accVect1 dds_conj_acc;
    const aie::vector<TT_DATA, m_kNumLanes>* sFan = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRotSmall;
    const aie::vector<TT_DATA, m_kNumLanes>* bFan = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRotBig;
    aie::vector<TT_DATA, m_kNumLanes> sRotVec;
    aie::vector<TT_DATA, m_kNumLanes> sRotVecConj;
    aie::vector<TT_DATA, m_kNumLanes> bRotVec;
    ::aie::accum<cacc64, 4> ddsOutLUT;
    TT_DATA sincos;
    aie::vector<TT_DATA, m_kNumLanes> ddsOutInter;
    sRotVec = *(sFan + 0);
    bRotVec = *(bFan + 0);
    set_rnd_mode<rnd_sym_inf>(); // round to symmetric infinity
    for (unsigned l = 0; l < m_kLoopCount; ++l) chess_prepare_for_pipelining chess_loop_count(m_kLoopCount) {
            ddsOutLUT =
                computeDDSOut<TP_NUM_LUTS, m_kLUTRes, m_kLUTMask, m_kNumLanes, TT_DATA>(m_phaseIndex, sincosLUT);
            sincos = ddsOutLUT.template to_vector<TT_DATA>(m_kLUTShift)[0];
            dds_acc = aie::mul(sincos, bRotVec);
            ddsOutInter = dds_acc.template to_vector<TT_DATA>(m_kDdsShift);
#pragma unroll m_kNumLanes
            for (unsigned cyc = 0; cyc < m_kNumLanes; cyc++) { // outer loop that executes 8 cycles worth of processing
                // create a per sample dds output using the per cycle dds value
                // and the per sample m_phRot array values
                dds_acc = aie::mul(sRotVec, ddsOutInter[cyc]);
                ddsReadIp<TP_API, m_kNumLanes, TT_DATA>(in0Ptr, in0Strm, mixer_vdata);
                mixer_acc = aie::mul(mixer_vdata, dds_acc.template to_vector<TT_DATA>(m_kDdsShift));
                ddsOutConj = aie::conj(dds_acc.template to_vector<TT_DATA>(m_kDdsShift));
                ddsReadIp<TP_API, m_kNumLanes, TT_DATA>(in1Ptr, in1Strm, mixer_vdata);
                mixer_acc = aie::mac(mixer_acc, mixer_vdata, ddsOutConj);
                ddsWriteOp<m_kMixerShift, TP_API, m_kNumLanes>(out0Ptr, outStrm, mixer_acc);
            }
            m_phaseIndex += m_perCyclePhaseInc;
        }
};

// Overload for TP_MIXER_MODE = 1  (DDS PLUS 1 data input MIXER MODE)
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
INLINE_DECL void
kernelDdsMixerClass<TT_DATA, TP_INPUT_WINDOW_VSIZE, TP_MIXER_MODE, TP_API, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>::
    ddsKernel(void* __restrict in0, void* __restrict out0) {
#if __SINCOS_IN_HW__ == 1
    aie::vector<TT_DATA, m_kNumLanes> mixer_vdata;
    input_stream<TT_DATA>* __restrict inStrm = (input_stream<TT_DATA>*)in0;
    output_stream<TT_DATA>* __restrict outStrm = (output_stream<TT_DATA>*)out0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict in0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)in0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict out0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)out0;
    aie::vector<T_DDS_TYPE, m_kNumLanes> dds_out;
    aie::vector<T_DDS_TYPE, m_kNumLanes> rot_vec;
    set_sat_mode<TP_SAT>();
    set_rnd_mode<TP_RND>();
    // pointer ww to m_phRot array as data type v8cint16
    const aie::vector<T_DDS_TYPE, m_kNumLanes>* ww = (const aie::vector<T_DDS_TYPE, m_kNumLanes>*)m_phRot;
    rot_vec = *(ww + 0);
    // dds_out is storage for sine/cos value generated from scalar function sincos, once per cycle
    using T_accint16Vect0 = typename ::aie::accum<cacc48, m_kNumLanes> chess_storage(bm0);
    using T_accint32Vect0 = typename ::aie::accum<cacc48, m_kNumLanes> chess_storage(aml0);
    using T_accVect0 =
        typename std::conditional<std::is_same<TT_DATA, cint16>::value, T_accint16Vect0, T_accint32Vect0>::type;
    using T_accint16Vect2 = typename ::aie::accum<cacc48, m_kNumLanes> chess_storage(bm2);
    using T_accint32Vect2 = typename ::aie::accum<cacc80, m_kNumLanes> chess_storage(bm2);
    using T_accVect2 =
        typename std::conditional<std::is_same<TT_DATA, cint16>::value, T_accint16Vect2, T_accint32Vect2>::type;
    T_accVect0 dds_acc; // Note literal cacc48. This specialization is for int types and dds out for int is always
                        // cint16
    T_accVect2 mixer_acc;
    // set_rnd_mode<TP_RND>();
    for (unsigned op = 0; op < m_kLoopCount; ++op) chess_prepare_for_pipelining chess_loop_count(m_kLoopCount) {
#if __SINCOS_IN_HW__ == 1
            dds_out[0] = aie::sincos_complex(m_phaseIndex);
#endif
            m_phaseIndex += m_perCyclePhaseInc;
            dds_acc = aie::mul(rot_vec, dds_out[0]); // per sample dds output
            ddsReadIp<TP_API, m_kNumLanes, TT_DATA>(in0Ptr, inStrm, mixer_vdata);
            mixer_acc = aie::mul(mixer_vdata, dds_acc.template to_vector<TT_DATA>(m_kDdsShift));
            ddsWriteOp<m_kDdsShift, TP_API, m_kNumLanes>(out0Ptr, outStrm, mixer_acc);
        }
#endif
};

// Overload for TP_MIXER_MODE = 1  (DDS PLUS 1 data input MIXER MODE - USE_LUT_SINCOS)
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
INLINE_DECL void kernelDdsMixerClass<TT_DATA,
                                     TP_INPUT_WINDOW_VSIZE,
                                     TP_MIXER_MODE,
                                     TP_API,
                                     USE_LUT_SINCOS,
                                     TP_NUM_LUTS,
                                     TP_RND,
                                     TP_SAT>::ddsKernel(void* __restrict in0, void* __restrict out0) {
    aie::vector<TT_DATA, m_kNumLanes> mixer_vdata;
    input_stream<TT_DATA>* __restrict inStrm = (input_stream<TT_DATA>*)in0;
    output_stream<TT_DATA>* __restrict outStrm = (output_stream<TT_DATA>*)out0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict in0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)in0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict out0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)out0;
    set_sat_mode<TP_SAT>();
    using T_accint16Vect0 = typename ::aie::accum<cacc32, m_kNumLanes>;
    using T_accint32Vect0 = typename ::aie::accum<cacc64, m_kNumLanes>;
    using T_accVect0 =
        typename std::conditional<std::is_same<TT_DATA, cint16>::value, T_accint16Vect0, T_accint32Vect0>::type;
    using T_accint16Vect2 = typename ::aie::accum<cacc32, m_kNumLanes>;
    using T_accint32Vect2 = typename ::aie::accum<cacc64, m_kNumLanes>;
    using T_accVect2 =
        typename std::conditional<std::is_same<TT_DATA, cint16>::value, T_accint16Vect2, T_accint32Vect2>::type;
    T_accVect0 dds_acc; // Note literal cacc48. This specialization is for int types and dds out for int is always
                        // cint16
    T_accVect2 mixer_acc;
    const aie::vector<TT_DATA, m_kNumLanes>* sFan = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRotSmall;
    const aie::vector<TT_DATA, m_kNumLanes>* bFan = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRotBig;
    aie::vector<TT_DATA, m_kNumLanes> sRotVec;
    aie::vector<TT_DATA, m_kNumLanes> bRotVec;
    ::aie::accum<cacc64, 4> ddsOutLUT;
    TT_DATA sincos;
    aie::vector<TT_DATA, m_kNumLanes> ddsOutInter;
    sRotVec = *(sFan + 0);
    bRotVec = *(bFan + 0);
    set_rnd_mode<rnd_sym_inf>(); // round to symmetric infinity
    for (unsigned op = 0; op < m_kLoopCount; ++op) chess_prepare_for_pipelining chess_loop_count(m_kLoopCount) {
            ddsOutLUT =
                computeDDSOut<TP_NUM_LUTS, m_kLUTRes, m_kLUTMask, m_kNumLanes, TT_DATA>(m_phaseIndex, sincosLUT);
            sincos = ddsOutLUT.template to_vector<TT_DATA>(m_kLUTShift)[0];
            dds_acc = aie::mul(sincos, bRotVec);
            ddsOutInter = dds_acc.template to_vector<TT_DATA>(m_kDdsShift);
#pragma unroll m_kNumLanes
            for (unsigned cyc = 0; cyc < m_kNumLanes; cyc++) { // outer loop that executes 8 cycles worth of processing
                dds_acc = aie::mul(sRotVec, ddsOutInter[cyc]);
                ddsReadIp<TP_API, m_kNumLanes, TT_DATA>(in0Ptr, inStrm, mixer_vdata);
                mixer_acc = aie::mul(mixer_vdata, dds_acc.template to_vector<TT_DATA>(m_kDdsShift));
                ddsWriteOp<m_kDdsShift, TP_API, m_kNumLanes>(out0Ptr, outStrm, mixer_acc);
            }
            m_phaseIndex += m_perCyclePhaseInc;
        }
};

// Overload for TP_MIXER_MODE = 0  (DDS ONLY MODE)
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
INLINE_DECL void
kernelDdsMixerClass<TT_DATA, TP_INPUT_WINDOW_VSIZE, TP_MIXER_MODE, TP_API, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>::
    ddsKernel(void* __restrict out0) {
    output_stream<TT_DATA>* __restrict outStrm = (output_stream<TT_DATA>*)out0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict out0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)out0;
    ::aie::accum<cacc48, m_kNumLanes> dds_acc;
    set_sat_mode<TP_SAT>();
    set_rnd_mode<TP_RND>();
    // m_phRot would ideally be constructor created and constant-persistant after that, hence stored in a register,
    // but kernels lose their registers between calls, so m_phRot has to be retrieved from memory.
    // This indirection of creating a pointer to m_phRot then loading that pointer into rot_vec appears to be
    // means to ensure that m_phRot is in memory (heap).
    const aie::vector<T_DDS_TYPE, m_kNumLanes>* ww = (const aie::vector<T_DDS_TYPE, m_kNumLanes>*)m_phRot;
    aie::vector<T_DDS_TYPE, m_kNumLanes> rot_vec;
    aie::vector<T_DDS_TYPE, m_kNumLanes> dds_out;
    rot_vec = *(ww + 0);
    for (unsigned op = 0; op < m_kLoopCount; ++op) chess_prepare_for_pipelining chess_loop_count(m_kLoopCount) {
#if __SINCOS_IN_HW__ == 1
            dds_out[0] = aie::sincos_complex(m_phaseIndex);
#endif
            m_phaseIndex += m_perCyclePhaseInc;
            dds_acc = aie::mul(rot_vec, dds_out[0]);
            ddsWriteOp<m_kDdsShift, TP_API, m_kNumLanes>(out0Ptr, outStrm, dds_acc);
        }
};

// Overload for TP_MIXER_MODE = 0  (DDS ONLY MODE - USE_LUT_SINCOS)
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
INLINE_DECL void kernelDdsMixerClass<TT_DATA,
                                     TP_INPUT_WINDOW_VSIZE,
                                     TP_MIXER_MODE,
                                     TP_API,
                                     USE_LUT_SINCOS,
                                     TP_NUM_LUTS,
                                     TP_RND,
                                     TP_SAT>::ddsKernel(void* __restrict out0) {
    output_stream<TT_DATA>* __restrict outStrm = (output_stream<TT_DATA>*)out0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict out0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)out0;

    using accType = typename tAccBaseType<TT_DATA, TT_DATA>::type;
    ::aie::accum<accType, m_kNumLanes> dds_acc;

    set_sat_mode<TP_SAT>();
    set_rnd_mode<rnd_sym_inf>(); // round to symmetric infinity
    const aie::vector<TT_DATA, m_kNumLanes>* sFan = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRotSmall;
    const aie::vector<TT_DATA, m_kNumLanes>* bFan = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRotBig;
    aie::vector<TT_DATA, m_kNumLanes> sRotVec;
    aie::vector<TT_DATA, m_kNumLanes> bRotVec;
    aie::vector<TT_DATA, m_kNumLanes> ddsOutInter;
    ::aie::accum<cacc64, 4> ddsOutLUT;
    TT_DATA sincos;
    sRotVec = *(sFan + 0);
    bRotVec = *(bFan + 0);

    for (unsigned op = 0; op < m_kLoopCount; ++op) chess_prepare_for_pipelining chess_loop_count(m_kLoopCount) {
            ddsOutLUT =
                computeDDSOut<TP_NUM_LUTS, m_kLUTRes, m_kLUTMask, m_kNumLanes, TT_DATA>(m_phaseIndex, sincosLUT);
            sincos = ddsOutLUT.template to_vector<TT_DATA>(m_kLUTShift)[0];
            dds_acc = aie::mul(sincos, bRotVec);
            ddsOutInter = dds_acc.template to_vector<TT_DATA>(m_kDdsShift);
#pragma unroll m_kNumLanes
            for (unsigned cyc = 0; cyc < m_kNumLanes; cyc++) { // outer loop that executes 8 cycles worth of processing
                // create a per sample dds output using the per cycle dds value
                // and the per sample m_phRot array values
                dds_acc = aie::mul(sRotVec, ddsOutInter[cyc]);
                ddsWriteOp<m_kDdsShift, TP_API, m_kNumLanes>(out0Ptr, outStrm, dds_acc);
            }
            m_phaseIndex += m_perCyclePhaseInc;
        }
};
#if __SUPPORTS_CFLOAT__ == 1
//-----------------------------------
// cfloat specializations
// Constructor to populate m_phRot array
template <unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
kernelDdsMixerClass<cfloat,
                    TP_INPUT_WINDOW_VSIZE,
                    TP_MIXER_MODE,
                    TP_API,
                    USE_INBUILT_SINCOS,
                    TP_NUM_LUTS,
                    TP_RND,
                    TP_SAT>::kernelDdsMixerClass(uint32_t phaseInc) {
    m_phaseIndex = 0;
    cint16 phRotInt16;
    // Calculate the phase increment for each clock cycle ( = per sample inc * input vector size)
    m_perCyclePhaseInc = phaseInc * m_kNumLanes;

    // calculate per-lane offset angle (as Cartesian)
    for (unsigned int i = 0; i < m_kNumLanes; i++) {
        phRotInt16 = aie::sincos_complex(i * phaseInc);
        m_phRot[i].real = ((float)phRotInt16.real) / (float(1 << 15));
        m_phRot[i].imag = ((float)phRotInt16.imag) / (float(1 << 15));
    }
}

template <unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
kernelDdsMixerClass<cfloat, TP_INPUT_WINDOW_VSIZE, TP_MIXER_MODE, TP_API, USE_LUT_SINCOS, TP_NUM_LUTS, TP_RND, TP_SAT>::
    kernelDdsMixerClass(uint32_t phaseInc) {
    m_phaseIndex = 0;
    set_rnd(rnd_sym_inf); // round to symmetric infinity
    aie::vector<cfloat, m_kNumLanes> sincosVal[TP_NUM_LUTS];
    aie::vector<cfloat, m_kNumLanes> tempVal;
    aie::vector<cfloat, m_kNumLanes> tempOut;

    sincosLUT[0] = (cfloat*)sincosLUTFloat1;
    if
        constexpr(TP_NUM_LUTS > 1) { sincosLUT[1] = (cfloat*)sincosLUTFloat2; }
    if
        constexpr(TP_NUM_LUTS > 2) { sincosLUT[2] = (cfloat*)sincosLUTFloat3; }
    for (unsigned int t = 0; t < TP_NUM_LUTS; t++) {
        for (unsigned int i = 0; i < m_kNumLanes; i++) {
            sincosVal[t][i] = sincosLUT[t][((i * phaseInc) >> (32 - 10 * (t + 1))) & 0x000003FF];
        }
    }

    if
        constexpr(TP_NUM_LUTS == 1) { tempOut = sincosVal[0]; }
    else if
        constexpr(TP_NUM_LUTS == 2) { tempOut = aie::mul(sincosVal[0], sincosVal[1]); }
    else if
        constexpr(TP_NUM_LUTS == 3) {
            tempOut = aie::mul(sincosVal[0], sincosVal[1]);
            tempOut = aie::mul(tempOut, sincosVal[2]);
        }

    for (int i = 0; i < m_kNumLanes; i++) {
        m_phRotSmall[i] = tempOut[i];
    }

    m_perCyclePhaseInc = phaseInc * m_kNumLanes * m_kNumLanes;
    const int m_bFanStepSize = phaseInc * m_kNumLanes;
    for (unsigned int t = 0; t < TP_NUM_LUTS; t++) {
        for (unsigned int i = 0; i < m_kNumLanes; i++) {
            sincosVal[t][i] = sincosLUT[t][((i * m_bFanStepSize) >> (32 - 10 * (t + 1))) & 0x000003FF];
        }
    }

    if
        constexpr(TP_NUM_LUTS == 1) { tempOut = sincosVal[0]; }
    else if
        constexpr(TP_NUM_LUTS == 2) { tempOut = aie::mul(sincosVal[0], sincosVal[1]); }
    else if
        constexpr(TP_NUM_LUTS == 3) {
            tempOut = aie::mul(sincosVal[0], sincosVal[1]);
            tempOut = aie::mul(tempOut, sincosVal[2]);
        }

    for (int i = 0; i < m_kNumLanes; i++) {
        m_phRotBig[i] = tempOut[i];
    }
}
// Constructor to populate m_phRot array with an initial offset
// making this overload so that the default case doesn't get extra penalty of additions and sets with trivial value of
// 0.
// This enables SSR DDS
template <unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
kernelDdsMixerClass<cfloat,
                    TP_INPUT_WINDOW_VSIZE,
                    TP_MIXER_MODE,
                    TP_API,
                    USE_INBUILT_SINCOS,
                    TP_NUM_LUTS,
                    TP_RND,
                    TP_SAT>::kernelDdsMixerClass(uint32_t phaseInc, uint32_t initialPhaseOffset)
    : kernelDdsMixerClass(phaseInc) {
    // initialise phase accumulator index to offset.
    // enhancement? if initialPhaseOffset was a template argument, then we could just set this at initialisation without
    // penalty
    m_phaseIndex = initialPhaseOffset;
}
// Constructor to populate m_phRot array with an initial offset
// making this overload so that the default case doesn't get extra penalty of additions and sets with trivial value of
// 0.
// This enables SSR DDS
template <unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
kernelDdsMixerClass<cfloat, TP_INPUT_WINDOW_VSIZE, TP_MIXER_MODE, TP_API, USE_LUT_SINCOS, TP_NUM_LUTS, TP_RND, TP_SAT>::
    kernelDdsMixerClass(uint32_t phaseInc, uint32_t initialPhaseOffset)
    : kernelDdsMixerClass(phaseInc) {
    // initialise phase accumulator index to offset.
    // enhancement? if initialPhaseOffset was a template argument, then we could just set this at initialisation without
    // penalty
    m_phaseIndex = initialPhaseOffset;
}

// DDS_Mixer run-time function
// Overload for TP_MIXER_MODE=2
//----------------------------------------------------------------------------------------------------------------------
template <unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
INLINE_DECL void kernelDdsMixerClass<cfloat,
                                     TP_INPUT_WINDOW_VSIZE,
                                     TP_MIXER_MODE,
                                     TP_API,
                                     USE_INBUILT_SINCOS,
                                     TP_NUM_LUTS,
                                     TP_RND,
                                     TP_SAT>::ddsKernel(void* __restrict in0,
                                                        void* __restrict in1,
                                                        void* __restrict out0) {
    using TT_DATA = cfloat;
    using T_DDS_TYPE = cfloat;
    const aie::vector<TT_DATA, m_kNumLanes>* ww = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRot;
    constexpr int kUnrollFactor = 2; // optimizes microcode.
    // only ever use index 0 dds_out, but phrasing this as a vector seems to help the compiler pipeline a lot better.
    cint16 ddsOutInt;
    cfloat ddsOutFloat;
    aie::vector<TT_DATA, m_kNumLanes>
        dds_out; // phrased as a vector even though it is scalar, since this optimizes far better
    aie::vector<TT_DATA, m_kNumLanes> mixer_vdata;
    aie::vector<TT_DATA, m_kNumLanes> dds_acc;
    aie::vector<TT_DATA, m_kNumLanes> dds_conj_acc;
    aie::vector<TT_DATA, m_kNumLanes> mixer_acc;
    aie::vector<TT_DATA, m_kNumLanes> mixer2;
    aie::vector<TT_DATA, m_kNumLanes> rot_vec; // load rotation values in rot_vec

    input_stream<TT_DATA>* __restrict in0Strm = (input_stream<TT_DATA>*)in0;
    input_stream<TT_DATA>* __restrict in1Strm = (input_stream<TT_DATA>*)in1;
    output_stream<TT_DATA>* __restrict outStrm = (output_stream<TT_DATA>*)out0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict in0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)in0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict in1Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)in1;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict out0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)out0;

    rot_vec = *(ww + 0);
    set_sat_mode<TP_SAT>();

    for (unsigned l = 0; l < m_kLoopCount / kUnrollFactor; ++l)
        chess_prepare_for_pipelining chess_loop_count(m_kLoopCount / kUnrollFactor) {
#pragma unroll(kUnrollFactor)
            for (int k = 0; k < kUnrollFactor; k++) {
                ddsOutInt = aie::sincos_complex(m_phaseIndex);
                ddsOutFloat.real = aie::to_float(ddsOutInt.real, 15);
                ddsOutFloat.imag = aie::to_float(ddsOutInt.imag, 15);
                dds_out[0] = ddsOutFloat;
                m_phaseIndex += m_perCyclePhaseInc;
                dds_acc = aie::mul(rot_vec, dds_out[0]);
                ddsReadIp<TP_API, m_kNumLanes, TT_DATA>(in0Ptr, in0Strm, mixer_vdata);
                mixer_acc = aie::mul(mixer_vdata, dds_acc);
                dds_conj_acc = aie::mul(aie::op_conj(rot_vec), aie::op_conj(dds_out[0]));
                ddsReadIp<TP_API, m_kNumLanes, TT_DATA>(in1Ptr, in1Strm, mixer_vdata);
                mixer2 = aie::mul(mixer_vdata, dds_conj_acc);
                mixer_acc = aie::add(mixer_acc, mixer2);
                ddsWriteOp<0, TP_API, m_kNumLanes>(out0Ptr, outStrm, mixer_acc);
            }
        }
};

// Overload for TP_MIXER_MODE=2 - Using LUT Implementation
template <unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
INLINE_DECL void
kernelDdsMixerClass<cfloat, TP_INPUT_WINDOW_VSIZE, TP_MIXER_MODE, TP_API, USE_LUT_SINCOS, TP_NUM_LUTS, TP_RND, TP_SAT>::
    ddsKernel(void* __restrict in0, void* __restrict in1, void* __restrict out0) {
    using TT_DATA = cfloat;
    aie::vector<TT_DATA, m_kNumLanes> mixerIpData;
    input_stream<TT_DATA>* __restrict in0Strm = (input_stream<TT_DATA>*)in0;
    input_stream<TT_DATA>* __restrict in1Strm = (input_stream<TT_DATA>*)in1;
    output_stream<TT_DATA>* __restrict outStrm = (output_stream<TT_DATA>*)out0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict in0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)in0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict in1Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)in1;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict out0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)out0;
    aie::vector<TT_DATA, m_kNumLanes> ddsOutConj;
    set_sat_mode<TP_SAT>();
    aie::vector<TT_DATA, m_kNumLanes> dds_acc;
    aie::vector<TT_DATA, m_kNumLanes> mixerAcc;
    aie::vector<TT_DATA, m_kNumLanes> mixerAccInter;
    const aie::vector<TT_DATA, m_kNumLanes>* sFan = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRotSmall;
    const aie::vector<TT_DATA, m_kNumLanes>* bFan = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRotBig;
    aie::vector<TT_DATA, m_kNumLanes> sRotVec;
    aie::vector<TT_DATA, m_kNumLanes> sRotVecConj;
    aie::vector<TT_DATA, m_kNumLanes> bRotVec;
    TT_DATA sincos;
    aie::vector<TT_DATA, m_kNumLanes> ddsOutInter;
    sRotVec = *(sFan + 0);
    bRotVec = *(bFan + 0);
    for (unsigned l = 0; l < m_kLoopCount; ++l) chess_prepare_for_pipelining chess_loop_count(m_kLoopCount) {
            sincos =
                computeDDSOutFloat<TP_NUM_LUTS, m_kLUTRes, m_kLUTMask, m_kNumLanes, TT_DATA>(m_phaseIndex, sincosLUT);
            ddsOutInter = aie::mul(sincos, bRotVec);
#pragma unroll m_kNumLanes
            for (unsigned cyc = 0; cyc < m_kNumLanes; cyc++) {
                dds_acc = aie::mul(sRotVec, ddsOutInter[cyc]);
                ddsReadIp<TP_API, m_kNumLanes, TT_DATA>(in0Ptr, in0Strm, mixerIpData);
                mixerAcc = aie::mul(mixerIpData, dds_acc);
                ddsOutConj = aie::conj(dds_acc);
                ddsReadIp<TP_API, m_kNumLanes, TT_DATA>(in1Ptr, in1Strm, mixerIpData);
                mixerAccInter = aie::mul(mixerIpData, ddsOutConj);
                mixerAcc = aie::add(mixerAcc, mixerAccInter);
                ddsWriteOp<m_kMixerShift, TP_API, m_kNumLanes>(out0Ptr, outStrm, mixerAcc);
            }
            m_phaseIndex += m_perCyclePhaseInc;
        }
};

// Overload for TP_MIXER_MODE = 1
template <unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
INLINE_DECL void kernelDdsMixerClass<cfloat,
                                     TP_INPUT_WINDOW_VSIZE,
                                     TP_MIXER_MODE,
                                     TP_API,
                                     USE_INBUILT_SINCOS,
                                     TP_NUM_LUTS,
                                     TP_RND,
                                     TP_SAT>::ddsKernel(void* __restrict in0, void* __restrict out0) {
    using TT_DATA = cfloat;
    using T_DDS_TYPE = cfloat;

    const aie::vector<TT_DATA, m_kNumLanes>* ww = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRot;
    constexpr int kUnrollFactor = 2; // optimizes microcode.

    cint16 ddsOutInt;
    cfloat ddsOutFloat;
    aie::vector<TT_DATA, m_kNumLanes> dds_out;
    aie::vector<TT_DATA, m_kNumLanes> dds_acc;
    aie::vector<TT_DATA, m_kNumLanes> mixer_vdata;
    aie::vector<TT_DATA, m_kNumLanes>
        mixer_acc; // force dds_acc and mixer acc to separate registers to avoid dependency?
    aie::vector<TT_DATA, m_kNumLanes> rot_vec;
    input_stream<TT_DATA>* __restrict inStrm = (input_stream<TT_DATA>*)in0;
    output_stream<TT_DATA>* __restrict outStrm = (output_stream<TT_DATA>*)out0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict in0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)in0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict out0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)out0;

    rot_vec = *(ww + 0);

    set_rnd_mode<TP_RND>(); // Despite being a float specialization, the DDS output is cint16, so saturation and
                            // rounding are relevant.
    set_sat_mode<TP_SAT>();

    for (unsigned l = 0; l < m_kLoopCount / kUnrollFactor; ++l)
        chess_prepare_for_pipelining chess_loop_count(m_kLoopCount / kUnrollFactor) {
#pragma unroll(kUnrollFactor)
            for (int k = 0; k < kUnrollFactor; k++) {
                ddsOutInt = aie::sincos_complex(m_phaseIndex);
                ddsOutFloat.real = aie::to_float(ddsOutInt.real, 15);
                ddsOutFloat.imag = aie::to_float(ddsOutInt.imag, 15);
                dds_out[0] = ddsOutFloat;
                m_phaseIndex += m_perCyclePhaseInc;
                dds_acc = aie::mul(rot_vec, dds_out[0]);
                ddsReadIp<TP_API, m_kNumLanes, TT_DATA>(in0Ptr, inStrm, mixer_vdata);
                mixer_acc = aie::mul(mixer_vdata, dds_acc);
                ddsWriteOp<m_kDdsShift, TP_API, m_kNumLanes>(out0Ptr, outStrm, mixer_acc);
            }
        }
};

// Overload for TP_MIXER_MODE = 1 - Using LUT implementation
template <unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
INLINE_DECL void
kernelDdsMixerClass<cfloat, TP_INPUT_WINDOW_VSIZE, TP_MIXER_MODE, TP_API, USE_LUT_SINCOS, TP_NUM_LUTS, TP_RND, TP_SAT>::
    ddsKernel(void* __restrict in0, void* __restrict out0) {
    using TT_DATA = cfloat;
    aie::vector<TT_DATA, m_kNumLanes> mixer_vdata;
    input_stream<TT_DATA>* __restrict inStrm = (input_stream<TT_DATA>*)in0;
    output_stream<TT_DATA>* __restrict outStrm = (output_stream<TT_DATA>*)out0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict in0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)in0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict out0Ptr = (aie::vector<TT_DATA, m_kNumLanes>*)out0;
    aie::vector<TT_DATA, m_kNumLanes> dds_acc;
    aie::vector<TT_DATA, m_kNumLanes> ddsAccInter;
    aie::vector<TT_DATA, m_kNumLanes> mixer_acc;
    const aie::vector<TT_DATA, m_kNumLanes>* sFan = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRotSmall;
    const aie::vector<TT_DATA, m_kNumLanes>* bFan = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRotBig;
    aie::vector<TT_DATA, m_kNumLanes> sRotVec;
    aie::vector<TT_DATA, m_kNumLanes> bRotVec;
    TT_DATA sincos;
    sRotVec = *(sFan + 0);
    bRotVec = *(bFan + 0);
    set_sat_mode<TP_SAT>();
    set_rnd_mode<TP_RND>();

    for (unsigned op = 0; op < m_kLoopCount; ++op) chess_prepare_for_pipelining chess_loop_count(m_kLoopCount) {
            sincos =
                computeDDSOutFloat<TP_NUM_LUTS, m_kLUTRes, m_kLUTMask, m_kNumLanes, TT_DATA>(m_phaseIndex, sincosLUT);
            ddsAccInter = aie::mul(sincos, bRotVec);
#pragma unroll m_kNumLanes
            for (unsigned cyc = 0; cyc < m_kNumLanes; cyc++) {
                dds_acc = aie::mul(sRotVec, ddsAccInter[cyc]);
                ddsReadIp<TP_API, m_kNumLanes, TT_DATA>(in0Ptr, inStrm, mixer_vdata);
                mixer_acc = aie::mul(mixer_vdata, dds_acc);
                ddsWriteOp<m_kDdsShift, TP_API, m_kNumLanes>(out0Ptr, outStrm, mixer_acc);
            }
            m_phaseIndex += m_perCyclePhaseInc;
        }
};

// Overload for TP_MIXER_MODE = 0  (DDS ONLY MODE)
template <unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
INLINE_DECL void kernelDdsMixerClass<cfloat,
                                     TP_INPUT_WINDOW_VSIZE,
                                     TP_MIXER_MODE,
                                     TP_API,
                                     USE_INBUILT_SINCOS,
                                     TP_NUM_LUTS,
                                     TP_RND,
                                     TP_SAT>::ddsKernel(void* __restrict out0) {
    using TT_DATA = cfloat;
    using T_DDS_TYPE = cfloat;

    output_stream<cfloat>* __restrict outStrm = (output_stream<cfloat>*)out0;
    aie::vector<TT_DATA, m_kNumLanes>* __restrict outPtr = (aie::vector<TT_DATA, m_kNumLanes>*)out0;

    // m_phRot would ideally be constructor created and constant-persistant after that, hence stored in a register,
    // but kernels lose their registers between calls, so m_phRot has to be retrieved from memory.
    // This indirection of creating a pointer to m_phRot then loading that pointer into rot_vec appears to be
    // means to ensure that m_phRot is in memory (heap).
    const aie::vector<TT_DATA, m_kNumLanes>* ww = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRot;
    constexpr int kUnrollFactor = 2; // optimizes microcode.
    unsigned int* phaseIndexPtr = (unsigned int*)&m_phaseIndex;
    cint16 ddsOutInt;
    cfloat ddsOutFloat;
    aie::vector<TT_DATA, m_kNumLanes> dds_out;
    using T_accVect = typename ::aie::vector<cfloat, m_kNumLanes>;
    T_accVect dds_acc;
    aie::vector<TT_DATA, m_kNumLanes> rot_vec;

    // load rotation values in rot_vec
    rot_vec = *(ww + 0);
    m_phaseIndex = *phaseIndexPtr;
    set_sat_mode<TP_SAT>(); // Despite being a float specialization, the DDS output is cint16, so saturation and
                            // rounding are relevant.
    set_rnd_mode<TP_RND>();

    for (unsigned op = 0; op < m_kLoopCount / kUnrollFactor; ++op)
        chess_prepare_for_pipelining chess_loop_count(m_kLoopCount / kUnrollFactor) {
#pragma unroll(kUnrollFactor)
            for (int k = 0; k < kUnrollFactor; k++) {
                ddsOutInt = aie::sincos_complex(m_phaseIndex);
                ddsOutFloat.real = aie::to_float(ddsOutInt.real, 15);
                ddsOutFloat.imag = aie::to_float(ddsOutInt.imag, 15);
                dds_out[0] = ddsOutFloat;
                m_phaseIndex += m_perCyclePhaseInc;
                dds_acc = aie::mul(rot_vec, dds_out[0]); // per sample dds output
                ddsWriteOp<m_kDdsShift, TP_API, m_kNumLanes>(outPtr, outStrm, dds_acc);
            }
        }
    *phaseIndexPtr = m_phaseIndex; // ensure persistence
};

// Overload for TP_MIXER_MODE = 0 - Using LUT Implementation
template <unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
INLINE_DECL void
kernelDdsMixerClass<cfloat, TP_INPUT_WINDOW_VSIZE, TP_MIXER_MODE, TP_API, USE_LUT_SINCOS, TP_NUM_LUTS, TP_RND, TP_SAT>::
    ddsKernel(void* __restrict out0) {
    using TT_DATA = cfloat;
    output_stream<cfloat>* __restrict outStrm = (output_stream<cfloat>*)out0;
    aie::vector<cfloat, m_kNumLanes>* __restrict out0Ptr = (aie::vector<cfloat, m_kNumLanes>*)out0;
    aie::vector<cfloat, m_kNumLanes> dds_acc;
    set_sat_mode<TP_SAT>();
    const aie::vector<TT_DATA, m_kNumLanes>* sFan = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRotSmall;
    const aie::vector<TT_DATA, m_kNumLanes>* bFan = (const aie::vector<TT_DATA, m_kNumLanes>*)m_phRotBig;
    aie::vector<TT_DATA, m_kNumLanes> sRotVec;
    aie::vector<TT_DATA, m_kNumLanes> bRotVec;
    ::aie::vector<cfloat, m_kNumLanes> ddsOutInter;
    ::aie::vector<cfloat, m_kNumLanes> ddsOutLUT;
    TT_DATA sincos;
    sRotVec = *(sFan + 0);
    bRotVec = *(bFan + 0);
    for (unsigned op = 0; op < m_kLoopCount; ++op) chess_prepare_for_pipelining chess_loop_count(m_kLoopCount) {
            sincos =
                computeDDSOutFloat<TP_NUM_LUTS, m_kLUTRes, m_kLUTMask, m_kNumLanes, TT_DATA>(m_phaseIndex, sincosLUT);
            ddsOutInter = aie::mul(sincos, bRotVec);
#pragma unroll m_kNumLanes
            for (unsigned cyc = 0; cyc < m_kNumLanes; cyc++) { // outer loop that executes 8 cycles worth of processing
                // create a per sample dds output using the per cycle dds value
                // and the per sample m_phRot array values
                dds_acc = aie::mul(sRotVec, ddsOutInter[cyc]);
                ddsWriteOp<m_kDdsShift, TP_API, m_kNumLanes>(out0Ptr, outStrm, dds_acc);
            }
            m_phaseIndex += m_perCyclePhaseInc;
        }
};
#endif

// Entry level class

// DDS_MIXER function (MIXER_MODE_2)
//-----------------------------------------------------------------------------------------------------
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
NOINLINE_DECL void
dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, TP_MIXER_MODE, TP_API, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>::ddsMix(
    input_buffer<TT_DATA>& __restrict inWindowA,
    input_buffer<TT_DATA>& __restrict inWindowB,
    output_buffer<TT_DATA>& __restrict outWindow) {
    void* in0Ptr = inWindowA.data();
    void* in1Ptr = inWindowB.data();
    void* outPtr = outWindow.data();

    this->ddsKernel(in0Ptr, in1Ptr, outPtr);
};

template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
NOINLINE_DECL void
dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_2, 1, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>::ddsMix(
    input_stream<TT_DATA>* __restrict inWindowA,
    input_stream<TT_DATA>* __restrict inWindowB,
    output_stream<TT_DATA>* __restrict outWindow) {
    this->ddsKernel((void*)inWindowA, (void*)inWindowB, (void*)outWindow);
};

// DDS_MIXER function (MIXER_MODE_1)
//-----------------------------------------------------------------------------------------------------
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_API,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
NOINLINE_DECL void
dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_1, TP_API, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>::ddsMix(
    input_buffer<TT_DATA>& __restrict inWindowA, output_buffer<TT_DATA>& __restrict outWindow) {
    void* inPtr = inWindowA.data();
    void* outPtr = outWindow.data();
    this->ddsKernel(inPtr, outPtr);
};

template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
NOINLINE_DECL void
dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_1, 1, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>::ddsMix(
    input_stream<TT_DATA>* __restrict inWindowA, output_stream<TT_DATA>* __restrict outWindow) {
    this->ddsKernel((void*)inWindowA, (void*)outWindow);
};

// DDS_MIXER function (MIXER_MODE_0)
//-----------------------------------------------------------------------------------------------------
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_API,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
NOINLINE_DECL void
dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_0, TP_API, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>::ddsMix(
    output_buffer<TT_DATA>& __restrict outWindow) {
    void* outPtr = outWindow.data();
    this->ddsKernel(outPtr);
};

template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
NOINLINE_DECL void
dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_0, 1, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>::ddsMix(
    output_stream<TT_DATA>* __restrict outWindow) {
    this->ddsKernel((void*)outWindow);
};
}
}
}
}
}
