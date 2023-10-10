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

#ifndef _TEST_HARNESS_DDS_MIXER_TRAITS_HPP_
#define _TEST_HARNESS_DDS_MIXER_TRAITS_HPP_

#ifndef INLINE_DECL
#define INLINE_DECL inline __attribute__((always_inline))
#endif
#ifndef NOINLINE_DECL
#define NOINLINE_DECL inline __attribute__((noinline))
#endif

#define USE_INBUILT_SINCOS 0
#define USE_LUT_SINCOS 1
/*
This file contains sets of overloaded, templatized and specialized templatized functions which
encapsulate properties of the intrinsics used by the main kernal class. Specifically,
this file does not contain any vector types or intrinsics since it is required for construction
and therefore must be suitable for the aie compiler graph-level compilation.
*/

namespace xf {
namespace dsp {
namespace aie {
namespace mixer {
namespace dds_mixer {

template <typename TT_DATA, unsigned int tp_sincos_mode>
constexpr int ddsMulVecScalarLanes() {
    return 0;
} // effectively an error trap

template <>
constexpr int ddsMulVecScalarLanes<cint16, USE_INBUILT_SINCOS>() {
    return 8;
};
template <>
constexpr int ddsMulVecScalarLanes<cint32, USE_INBUILT_SINCOS>() {
    return 4;
};
template <>
constexpr int ddsMulVecScalarLanes<cfloat, USE_INBUILT_SINCOS>() {
    return 4;
};

#if __SUPPORTS_CFLOAT__ == 1
template <>
constexpr int ddsMulVecScalarLanes<cint16, USE_LUT_SINCOS>() {
    return 8;
};
template <>
constexpr int ddsMulVecScalarLanes<cint32, USE_LUT_SINCOS>() {
    return 4;
};
template <>
constexpr int ddsMulVecScalarLanes<cfloat, USE_LUT_SINCOS>() {
    return 2;
};
#else
template <>
constexpr int ddsMulVecScalarLanes<cint16, USE_LUT_SINCOS>() {
    return 8;
};
template <>
constexpr int ddsMulVecScalarLanes<cint32, USE_LUT_SINCOS>() {
    return 8;
};
template <>
constexpr int ddsMulVecScalarLanes<cfloat, USE_LUT_SINCOS>() {
    return 2;
};
#endif

/**

Base IO_API interface struct for shared functions across specialisations
*/
template <typename TT_DATA, typename PortType>
struct T_IFbase {
    template <unsigned int VECTOR_LEN>
    auto static INLINE_DECL port_readincr(PortType* in);

    template <typename OutDType>
    void static INLINE_DECL port_writeincr(PortType* out, OutDType data);
};
// mode 0 - no inputs, dds_only
template <typename TT_DATA, typename InPortType, unsigned int numMixerIn>
struct T_inputIF : T_IFbase<TT_DATA, InPortType> {};

// Mixer mode 1
template <typename TT_DATA, typename InPortType>
struct T_inputIF<TT_DATA, InPortType, 1> : T_IFbase<TT_DATA, InPortType> {
    InPortType* __restrict inPort;   // can be a window or a stream
    T_inputIF(){};                   // default constructor do nothing
    T_inputIF(InPortType* _inPort) { // todo - use references rather than pointers.
        inPort = _inPort;
    }
};

// Mixer mode 2
template <typename TT_DATA, typename InPortType>
struct T_inputIF<TT_DATA, InPortType, 2> : T_IFbase<TT_DATA, InPortType> {
    InPortType* __restrict inPortA;                         // can be a window or a stream
    InPortType* __restrict inPortB;                         // can be a window or a stream
    T_inputIF(){};                                          // default constructor do nothing
    T_inputIF(InPortType* _inPortA, InPortType* _inPortB) { // todo - use references rather than pointers.
        inPortA = _inPortA;
        inPortB = _inPortB;
    }
};

// mode 0,1,2 - single output (stream or window)
template <typename TT_DATA, typename OutPortType>
struct T_outputIF : T_IFbase<TT_DATA, OutPortType> { // inheritance

    OutPortType* __restrict outPort;
    T_outputIF(){};
    T_outputIF(OutPortType* _outPort) { outPort = _outPort; }
};

// Functions to support defensive checks
enum { enumUnknownType = 0, enumCint16, enumCint32, enumCfloat };
template <typename TT_DATA>
INLINE_DECL constexpr unsigned int fnEnumType() {
    return enumUnknownType;
}; // returns 0 as default. This can be trapped as an error;
template <>
INLINE_DECL constexpr unsigned int fnEnumType<cint16>() {
    return enumCint16;
};
template <>
INLINE_DECL constexpr unsigned int fnEnumType<cint32>() {
    return enumCint32;
};
#if __SUPPORTS_CFLOAT__ == 1
template <>
INLINE_DECL constexpr unsigned int fnEnumType<cfloat>() {
    return enumCfloat;
};
#endif

enum IO_API { WINDOW = 0, STREAM = 1 };
}
}
}
}
}
#endif // _TEST_HARNESS_DDS_MIXER_TRAITS_HPP_
