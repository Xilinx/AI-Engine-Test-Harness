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

#ifndef _TEST_HARNESS_DDS_MIXER_HPP_
#define _TEST_HARNESS_DDS_MIXER_HPP_

/*
DDS Mixer.
This file exists to capture the definition of the dds_mixer kernel class.
The class definition holds defensive checks on parameter range and other
legality.
The constructor definition is held in this class because this class must be
accessible to graph level aie compilation.
The main runtime ddsMix function is captured elsewhere as it contains aie
intrinsics which are not included in aie graph level
compilation.
*/

/* Coding conventions
   TT_      template type suffix
   TP_      template parameter suffix
*/

/* Design Notes
*/

#include "device_defs.h"
#include <adf.h>
#include "fir_utils.hpp"
#include "dds_mixer_traits.hpp"
#include <vector>
#include "dds_luts.h"
#include "dds_luts_floats.h"
//#define _DSPLIB_DDS_MIXER_HPP_DEBUG_

using namespace adf;
namespace xf {
namespace dsp {
namespace aie {
namespace mixer {
namespace dds_mixer {

//-----------------------------------------------------------------------------------------------------
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API = IO_API::WINDOW,
          unsigned int TP_SC_MODE = USE_INBUILT_SINCOS,
          unsigned int TP_NUM_LUTS = 1,
          unsigned int TP_RND = 0,
          unsigned int TP_SAT = 1>
class kernelDdsMixerClass {
   public:
    // 0 = rnd_floor, 1 = rnd_ceil, 2 = rnd_pos_inf, 3 = rnd_neg_inf, 4 = rnd_sym_inf, 5 = rnd_sym_zero, 6 =
    // rnd_conv_even, 7 = rnd_conv_odd
    static constexpr unsigned int kRoundMode = 0; // mode used in point designs.
    static constexpr unsigned int m_kNumLanes = ddsMulVecScalarLanes<TT_DATA, USE_INBUILT_SINCOS>();
    static constexpr unsigned int m_kDOutEachLoop = m_kNumLanes;
#if __SUPPORTS_CFLOAT__ == 1
    static constexpr unsigned int m_kDdsShift = std::is_same<TT_DATA, cfloat>::value ? 0 : 15;
    static constexpr unsigned int m_kMixerShift = std::is_same<TT_DATA, cfloat>::value ? 0 : 16;
#else
    static constexpr unsigned int m_kDdsShift = 15;
    static constexpr unsigned int m_kMixerShift = 16;
#endif
    static constexpr unsigned int m_kLoopCount = (TP_INPUT_WINDOW_VSIZE / m_kDOutEachLoop);
    static constexpr unsigned int m_kNumMixerInputs =
        (TP_MIXER_MODE == MIXER_MODE_2) ? 2 : (TP_MIXER_MODE == MIXER_MODE_1) ? 1 : 0;

    static constexpr unsigned int minAccumBits = std::is_same<TT_DATA, cint16>::value ? 32 : 64;
    using T_DDS_TYPE = cint16; // true for TT_DATA=cint16 or cint32. Cfloat is handled by a specialization of this
                               // class.

    static_assert(TP_RND >= ROUND_MIN && TP_RND <= ROUND_MAX, "ERROR: TP_RND is out of the supported range.");
    static_assert(TP_SAT >= SAT_MODE_MIN && TP_SAT <= SAT_MODE_MAX, "ERROR: TP_SAT is out of supported range");
    static_assert(TP_SAT != 2, "ERROR: TP_SAT is invalid. Valid values of TP_SAT are 0, 1, and 3");
    static_assert(TP_MIXER_MODE <= 2, "ERROR: DDS Mixer Mode must be 0, 1 or 2. ");
    static_assert(fnEnumType<TT_DATA>() != enumUnknownType,
                  "ERROR: DDS Mixer TT_DATA is not a supported type (Must be cint16).");
    static_assert(
        (TP_INPUT_WINDOW_VSIZE % m_kDOutEachLoop) == 0,
        "ERROR: DDS Mixer TP_INPUT_WINDOW_VSIZE must be a multiple of m_kNumLanes: 8 for cint16 and 4 for cint32.");
    static_assert(fnEnumType<TT_DATA>() != enumCint32 || TP_MIXER_MODE != MIXER_MODE_0,
                  "ERROR: cint32 is not support for DDS output.");
    unsigned int m_phaseIndex = 0;
    unsigned int m_perCyclePhaseInc;
    alignas(32) T_DDS_TYPE m_phRot[m_kNumLanes];

    // Constructor - use aie_api so definition within kernel scope
    kernelDdsMixerClass(unsigned int phaseInc);
    // Constructor with initialOffset
    kernelDdsMixerClass(unsigned int phaseInc, unsigned int initialPhaseOffset);

    // DDS Kernel
    // Use overoads rather than class specialisations to keep hiearchy a bit more simple
    // and avoid requirement of dependant names with this->variable.
    // mixer mode 2
    void ddsKernel(void* __restrict in0, void* __restrict in1, void* __restrict out0);
    // mixer mode 1
    void ddsKernel(void* __restrict in0, void* __restrict out0);
    // mixer mode 0
    void ddsKernel(void* __restrict out0);
};

//-----------------------------------------------------------------------------------------------------
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
class kernelDdsMixerClass<TT_DATA,
                          TP_INPUT_WINDOW_VSIZE,
                          TP_MIXER_MODE,
                          TP_API,
                          USE_LUT_SINCOS,
                          TP_NUM_LUTS,
                          TP_RND,
                          TP_SAT> {
   public:
    // 0 = rnd_floor, 1 = rnd_ceil, 2 = rnd_pos_inf, 3 = rnd_neg_inf, 4 = rnd_sym_inf, 5 = rnd_sym_zero, 6 =
    // rnd_conv_even, 7 = rnd_conv_odd
    static constexpr unsigned int kRoundMode = 0; // mode used in point designs.
    static constexpr unsigned int m_kNumLanes = ddsMulVecScalarLanes<TT_DATA, USE_LUT_SINCOS>();
    // todo, make these parameters in future release.

    static constexpr unsigned int m_kDOutEachLoop = m_kNumLanes * m_kNumLanes;
    static constexpr int maxProdBits = TP_NUM_LUTS == 1 ? 32 : 63;
    static constexpr unsigned int m_kLUTShift = maxProdBits - (sizeof(TT_DATA) / 2 * 8);
    static constexpr unsigned int m_kDdsShift = sizeof(TT_DATA) / 2 * 8 - 1;
    static constexpr unsigned int m_kMixerShift = sizeof(TT_DATA) / 2 * 8;
    static constexpr unsigned int m_kLUTRes = 10;
    static constexpr unsigned int m_kLUTMask = ((1 << m_kLUTRes) - 1) << (32 - m_kLUTRes);
    alignas(32) cint32* sincosLUT[TP_NUM_LUTS];

    static constexpr unsigned int m_kLoopCount = (TP_INPUT_WINDOW_VSIZE / m_kDOutEachLoop);
    static constexpr unsigned int m_kNumMixerInputs =
        (TP_MIXER_MODE == MIXER_MODE_2) ? 2 : (TP_MIXER_MODE == MIXER_MODE_1) ? 1 : 0;

    static constexpr unsigned int minAccumBits = std::is_same<TT_DATA, cint16>::value ? 32 : 64;
    using T_DDS_TYPE = cint16; // true for TT_DATA=cint16 or cint32. Cfloat is handled by a specialization of this
                               // class.

    static_assert(TP_MIXER_MODE <= 2, "ERROR: DDS Mixer Mode must be 0, 1 or 2. ");
    static_assert(fnEnumType<TT_DATA>() != enumUnknownType,
                  "ERROR: DDS Mixer TT_DATA is not a supported type (Must be cint16).");
    static_assert((TP_INPUT_WINDOW_VSIZE % m_kDOutEachLoop) == 0,
                  "ERROR: DDS Mixer TP_INPUT_WINDOW_VSIZE must be a multiple of data output in each loop. For AIE1, "
                  "this has to be 64 for cint16, 16 for cint32. For AIE2, this has to be 256 for cint16 and 64 for "
                  "cint32.");

    unsigned int m_phaseIndex = 0;
    unsigned int m_perCyclePhaseInc;
    alignas(32) TT_DATA m_phRotSmall[m_kNumLanes];
    alignas(32) TT_DATA m_phRotBig[m_kNumLanes];

    // Constructor - use aie_api so definition within kernel scope
    kernelDdsMixerClass(unsigned int phaseInc);
    // Constructor with initialOffset
    kernelDdsMixerClass(unsigned int phaseInc, unsigned int initialPhaseOffset);

    // DDS Kernel
    // Use overoads rather than class specialisations to keep hiearchy a bit more simple
    // and avoid requirement of dependant names with this->variable.
    // mixer mode 2
    void ddsKernel(void* __restrict in0, void* __restrict in1, void* __restrict out0);
    // mixer mode 1
    void ddsKernel(void* __restrict in0, void* __restrict out0);
    // mixer mode 0
    void ddsKernel(void* __restrict out0);
};

#if __SUPPORTS_CFLOAT__ == 1
template <unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
class kernelDdsMixerClass<cfloat,
                          TP_INPUT_WINDOW_VSIZE,
                          TP_MIXER_MODE,
                          TP_API,
                          USE_INBUILT_SINCOS,
                          TP_NUM_LUTS,
                          TP_RND,
                          TP_SAT> {
   public:
    using TT_DATA = cfloat;
    static constexpr unsigned int kRoundMode = 0; // mode used in point designs.
    static constexpr unsigned int m_kNumLanes =
        ddsMulVecScalarLanes<cfloat, USE_INBUILT_SINCOS>();                // todo - calculate for other types
    static constexpr unsigned int m_kSamplesInReg = 1024 / sizeof(cfloat); // 16, but this phrasing explains more.
    // todo, make these parameters in future release.
    static constexpr unsigned int m_kDdsShift = std::is_same<TT_DATA, cfloat>::value ? 0 : 15;
    static constexpr unsigned int m_kMixerShift = std::is_same<TT_DATA, cfloat>::value ? 0 : 16;
    static constexpr unsigned int m_kDOutEachLoop = m_kNumLanes;
    static constexpr unsigned int m_kLoopCount = (TP_INPUT_WINDOW_VSIZE / m_kDOutEachLoop);
    static constexpr unsigned int m_kNumMixerInputs =
        (TP_MIXER_MODE == MIXER_MODE_2) ? 2 : (TP_MIXER_MODE == MIXER_MODE_1) ? 1 : 0;
    static constexpr float m_kInt2floatScale = (float)1.0 / (float)(1 << 15);

    static_assert(TP_MIXER_MODE <= 2, "ERROR: DDS Mixer Mode must be 0, 1 or 2. ");
    static_assert(fnEnumType<TT_DATA>() != enumUnknownType,
                  "ERROR: DDS Mixer TT_DATA is not a supported type (Must be cint16).");
    static_assert(
        (TP_INPUT_WINDOW_VSIZE % m_kDOutEachLoop) == 0,
        "ERROR: DDS Mixer TP_INPUT_WINDOW_VSIZE must be a multiple of m_kNumLanes: 8 for cint16 and 4 for cint32.");

    using T_inType = typename std::conditional<(TP_API == WINDOW), input_buffer<TT_DATA>, input_stream<TT_DATA> >::type;
    using T_outType =
        typename std::conditional<(TP_API == WINDOW), output_buffer<TT_DATA>, output_stream<TT_DATA> >::type;
    using T_inIF = T_inputIF<TT_DATA, T_inType, m_kNumMixerInputs>;
    using T_outIF = T_outputIF<TT_DATA, T_outType>;

    // Keeps track of where we are in sincos curve, incremented by phaseIncr; initial value set in constructor
    unsigned int m_phaseIndex = 0;
    unsigned int m_perCyclePhaseInc;
    alignas(32) TT_DATA m_phRot[m_kNumLanes];

    // Constructor - use aie_api so definition within kernel scope
    kernelDdsMixerClass(unsigned int phaseInc);
    // Constructor with initialOffset
    kernelDdsMixerClass(unsigned int phaseInc, unsigned int initialPhaseOffset);

    // for ddsKernel overloads
    using T_inIF_mm2 = T_inputIF<TT_DATA, T_inType, 2>;
    using T_inIF_mm1 = T_inputIF<TT_DATA, T_inType, 1>;

    // DDS Kernel
    // Use overoads rather than class specialisations to keep hiearchy a bit more simple
    // and avoid requirement of dependant names with this->variable.
    // mixer mode 2
    void ddsKernel(void* __restrict in0, void* __restrict in1, void* __restrict out0);
    // mixer mode 1
    void ddsKernel(void* __restrict in0, void* __restrict out0);
    // mixer mode 0
    void ddsKernel(void* __restrict out0);
};

template <unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
class kernelDdsMixerClass<cfloat,
                          TP_INPUT_WINDOW_VSIZE,
                          TP_MIXER_MODE,
                          TP_API,
                          USE_LUT_SINCOS,
                          TP_NUM_LUTS,
                          TP_RND,
                          TP_SAT> {
   public:
    using TT_DATA = cfloat;
    static constexpr unsigned int kRoundMode = 0; // mode used in point designs.
    static constexpr unsigned int m_kNumLanes = ddsMulVecScalarLanes<cfloat, USE_LUT_SINCOS>();
    static constexpr unsigned int m_kSamplesInReg = 1024 / sizeof(cfloat);
    // todo, make these parameters in future release.
    static constexpr unsigned int m_kDdsShift = 0;
    static constexpr unsigned int m_kMixerShift = 0;
    static constexpr unsigned int m_kDOutEachLoop = m_kNumLanes * m_kNumLanes;
    static constexpr unsigned int m_kLoopCount = (TP_INPUT_WINDOW_VSIZE / m_kDOutEachLoop);
    static constexpr unsigned int m_kNumMixerInputs =
        (TP_MIXER_MODE == MIXER_MODE_2) ? 2 : (TP_MIXER_MODE == MIXER_MODE_1) ? 1 : 0;
    static constexpr float m_kInt2floatScale = (float)1.0 / (float)(1 << 15);

    static_assert(TP_MIXER_MODE <= 2, "ERROR: DDS Mixer Mode must be 0, 1 or 2. ");
    static_assert(fnEnumType<TT_DATA>() != enumUnknownType,
                  "ERROR: DDS Mixer TT_DATA is not a supported type (Must be cint16).");
    static_assert((TP_INPUT_WINDOW_VSIZE % m_kDOutEachLoop) == 0,
                  "ERROR: DDS Mixer TP_INPUT_WINDOW_VSIZE must be a multiple of data output in each loop. For AIE1, "
                  "this has to be 64 for cint16, 16 for cint32. For AIE2, this has to be 256 for cint16 and 64 for "
                  "cint32.");

    using T_inType = typename std::conditional<(TP_API == WINDOW), input_buffer<TT_DATA>, input_stream<TT_DATA> >::type;
    using T_outType =
        typename std::conditional<(TP_API == WINDOW), output_buffer<TT_DATA>, output_stream<TT_DATA> >::type;
    using T_inIF = T_inputIF<TT_DATA, T_inType, m_kNumMixerInputs>;
    using T_outIF = T_outputIF<TT_DATA, T_outType>;

    // Keeps track of where we are in sincos curve, incremented by phaseIncr; initial value set in constructor
    unsigned int m_phaseIndex = 0;
    unsigned int m_perCyclePhaseInc;
    static constexpr unsigned int m_kLUTRes = 10;
    static constexpr unsigned int m_kLUTMask = ((1 << m_kLUTRes) - 1) << (32 - m_kLUTRes);
    alignas(32) cfloat* sincosLUT[TP_NUM_LUTS];
    alignas(32) TT_DATA m_phRotSmall[m_kNumLanes];
    alignas(32) TT_DATA m_phRotBig[m_kNumLanes];

    // Constructor - use aie_api so definition within kernel scope
    kernelDdsMixerClass(unsigned int phaseInc);
    // Constructor with initialOffset
    kernelDdsMixerClass(unsigned int phaseInc, unsigned int initialPhaseOffset);

    // for ddsKernel overloads
    using T_inIF_mm2 = T_inputIF<TT_DATA, T_inType, 2>;
    using T_inIF_mm1 = T_inputIF<TT_DATA, T_inType, 1>;

    // DDS Kernel
    // Use overoads rather than class specialisations to keep hiearchy a bit more simple
    // and avoid requirement of dependant names with this->variable.
    // mixer mode 2
    void ddsKernel(void* __restrict in0, void* __restrict in1, void* __restrict out0);
    // mixer mode 1
    void ddsKernel(void* __restrict in0, void* __restrict out0);
    // mixer mode 0
    void ddsKernel(void* __restrict out0);
};
#endif

//-----------------------------------------------------------------------------------------------------
// Default specialization of kernel entry class, also for MIXER_MODE=2
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_MIXER_MODE,
          unsigned int TP_API = IO_API::WINDOW,
          unsigned int TP_SC_MODE = USE_LUT_SINCOS,
          unsigned int TP_NUM_LUTS = 1,
          unsigned int TP_RND = 6,
          unsigned int TP_SAT = 1>
class dds_mixer : public kernelDdsMixerClass<TT_DATA,
                                             TP_INPUT_WINDOW_VSIZE,
                                             TP_MIXER_MODE,
                                             TP_API,
                                             TP_SC_MODE,
                                             TP_NUM_LUTS,
                                             TP_RND,
                                             TP_SAT> {
   public:
    // Help the compiler deal with dependant names
    using baseClass = kernelDdsMixerClass<TT_DATA,
                                          TP_INPUT_WINDOW_VSIZE,
                                          TP_MIXER_MODE,
                                          TP_API,
                                          TP_SC_MODE,
                                          TP_NUM_LUTS,
                                          TP_RND,
                                          TP_SAT>;
    using thisClass =
        dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, TP_MIXER_MODE, TP_API, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>;

    // Constructor
    dds_mixer(unsigned int phaseInc) : baseClass(phaseInc) {}

    // Constructor with phaseOffset - for SSR
    dds_mixer(unsigned int phaseInc, unsigned int initialPhaseOffset) : baseClass(phaseInc, initialPhaseOffset) {}

    // Register Kernel Class
    static void registerKernelClass() { REGISTER_FUNCTION(dds_mixer::ddsMix); }

    // dds
    void ddsMix(input_buffer<TT_DATA>& __restrict inWindowA,
                input_buffer<TT_DATA>& __restrict inWindowB,
                output_buffer<TT_DATA>& __restrict outWindow);
};

// Specialization for MODE=2, stream IO
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
class dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_2, 1, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>
    : public kernelDdsMixerClass<TT_DATA,
                                 TP_INPUT_WINDOW_VSIZE,
                                 MIXER_MODE_2,
                                 1,
                                 TP_SC_MODE,
                                 TP_NUM_LUTS,
                                 TP_RND,
                                 TP_SAT> {
   public:
    static constexpr unsigned int TP_MIXER_MODE = MIXER_MODE_2;
    static constexpr unsigned int TP_API = 1; // we are om a specialization
    // Help the compiler deal with dependant names
    using baseClass = kernelDdsMixerClass<TT_DATA,
                                          TP_INPUT_WINDOW_VSIZE,
                                          TP_MIXER_MODE,
                                          TP_API,
                                          TP_SC_MODE,
                                          TP_NUM_LUTS,
                                          TP_RND,
                                          TP_SAT>;
    using thisClass =
        dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, TP_MIXER_MODE, TP_API, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>;

    // Constructor
    dds_mixer(unsigned int phaseInc) : baseClass(phaseInc) {}

    // Constructor with phaseOffset - for SSR
    dds_mixer(unsigned int phaseInc, unsigned int initialPhaseOffset) : baseClass(phaseInc, initialPhaseOffset) {}

    // Register Kernel Class
    static void registerKernelClass() { REGISTER_FUNCTION(dds_mixer::ddsMix); }

    // dds
    void ddsMix(input_stream<TT_DATA>* __restrict inWindowA,
                input_stream<TT_DATA>* __restrict inWindowB,
                output_stream<TT_DATA>* __restrict outWindow);
};

// Specialization for mixer_mode = 1, buffer IO
//===============
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_API,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
class dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_1, TP_API, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>
    : public kernelDdsMixerClass<TT_DATA,
                                 TP_INPUT_WINDOW_VSIZE,
                                 MIXER_MODE_1,
                                 TP_API,
                                 TP_SC_MODE,
                                 TP_NUM_LUTS,
                                 TP_RND,
                                 TP_SAT> {
   private:
   public:
    // Help the compiler deal with dependant names
    using baseClass = kernelDdsMixerClass<TT_DATA,
                                          TP_INPUT_WINDOW_VSIZE,
                                          MIXER_MODE_1,
                                          TP_API,
                                          TP_SC_MODE,
                                          TP_NUM_LUTS,
                                          TP_RND,
                                          TP_SAT>;
    using thisClass =
        dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_1, TP_API, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>;

    // Constructor
    dds_mixer(unsigned int phaseInc) : baseClass(phaseInc) {}

    // Constructor with phaseOffset - for SSR
    dds_mixer(unsigned int phaseInc, unsigned int initialPhaseOffset) : baseClass(phaseInc, initialPhaseOffset) {}

    // Register Kernel Class
    static void registerKernelClass() { REGISTER_FUNCTION(dds_mixer::ddsMix); }

    // dds
    void ddsMix(input_buffer<TT_DATA>& __restrict inWindowA, output_buffer<TT_DATA>& __restrict outWindow);
};

// Specialization for mixer_mode = 1, stream IO
//===============
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
class dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_1, 1, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>
    : public kernelDdsMixerClass<TT_DATA,
                                 TP_INPUT_WINDOW_VSIZE,
                                 MIXER_MODE_1,
                                 1,
                                 TP_SC_MODE,
                                 TP_NUM_LUTS,
                                 TP_RND,
                                 TP_SAT> {
   private:
   public:
    // Help the compiler deal with dependant names
    using baseClass =
        kernelDdsMixerClass<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_1, 1, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>;
    using thisClass =
        dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_1, 1, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>;

    // Constructor
    dds_mixer(unsigned int phaseInc) : baseClass(phaseInc) {}

    // Constructor with phaseOffset - for SSR
    dds_mixer(unsigned int phaseInc, unsigned int initialPhaseOffset) : baseClass(phaseInc, initialPhaseOffset) {}

    // Register Kernel Class
    static void registerKernelClass() { REGISTER_FUNCTION(dds_mixer::ddsMix); }

    // dds
    void ddsMix(input_stream<TT_DATA>* __restrict inWindowA, output_stream<TT_DATA>* __restrict outWindow);
};

// Specialization for mixer_mode = 0
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_API,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
class dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_0, TP_API, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>
    : public kernelDdsMixerClass<TT_DATA,
                                 TP_INPUT_WINDOW_VSIZE,
                                 MIXER_MODE_0,
                                 TP_API,
                                 TP_SC_MODE,
                                 TP_NUM_LUTS,
                                 TP_RND,
                                 TP_SAT> {
   private:
   public:
    // Help the compiler deal with dependant names
    using baseClass = kernelDdsMixerClass<TT_DATA,
                                          TP_INPUT_WINDOW_VSIZE,
                                          MIXER_MODE_0,
                                          TP_API,
                                          TP_SC_MODE,
                                          TP_NUM_LUTS,
                                          TP_RND,
                                          TP_SAT>;
    using thisClass =
        dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_0, TP_API, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>;

    // Constructor
    dds_mixer(unsigned int phaseInc) : baseClass(phaseInc) {}

    // Constructor with phaseOffset - for SSR
    dds_mixer(unsigned int phaseInc, unsigned int initialPhaseOffset) : baseClass(phaseInc, initialPhaseOffset) {}

    // Register Kernel Class
    static void registerKernelClass() { REGISTER_FUNCTION(dds_mixer::ddsMix); }

    // dds
    void ddsMix(output_buffer<TT_DATA>& __restrict outWindow);
};

// Specialization for mixer_mode = 0 stream IO
template <typename TT_DATA,
          unsigned int TP_INPUT_WINDOW_VSIZE,
          unsigned int TP_SC_MODE,
          unsigned int TP_NUM_LUTS,
          unsigned int TP_RND,
          unsigned int TP_SAT>
class dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_0, 1, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>
    : public kernelDdsMixerClass<TT_DATA,
                                 TP_INPUT_WINDOW_VSIZE,
                                 MIXER_MODE_0,
                                 1,
                                 TP_SC_MODE,
                                 TP_NUM_LUTS,
                                 TP_RND,
                                 TP_SAT> {
   private:
   public:
    // Help the compiler deal with dependant names
    using baseClass =
        kernelDdsMixerClass<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_0, 1, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>;
    using thisClass =
        dds_mixer<TT_DATA, TP_INPUT_WINDOW_VSIZE, MIXER_MODE_0, 1, TP_SC_MODE, TP_NUM_LUTS, TP_RND, TP_SAT>;

    // Constructor
    dds_mixer(unsigned int phaseInc) : baseClass(phaseInc) {}

    // Constructor with phaseOffset - for SSR
    dds_mixer(unsigned int phaseInc, unsigned int initialPhaseOffset) : baseClass(phaseInc, initialPhaseOffset) {}

    // Register Kernel Class
    static void registerKernelClass() { REGISTER_FUNCTION(dds_mixer::ddsMix); }

    // dds
    void ddsMix(output_stream<TT_DATA>* __restrict outWindow);
};
}
}
}
}
}

#endif // _TEST_HARNESS_DDS_MIXER_HPP_
