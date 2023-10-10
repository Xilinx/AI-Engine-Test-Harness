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

#ifndef _TEST_HARNESS_MATRIX_MULT_TRAITS_HPP_
#define _TEST_HARNESS_MATRIX_MULT_TRAITS_HPP_

#ifndef INLINE_DECL
#define INLINE_DECL inline __attribute__((always_inline))
#endif
#ifndef NOINLINE_DECL
#define NOINLINE_DECL inline __attribute__((noinline))
#endif

namespace xf {
namespace dsp {
namespace aie {
namespace blas {
namespace matrix_mult {
/*
Asymmetrical Interpolation FIR traits.
This file contains sets of overloaded, templatized and specialized templatized functions which
encapsulate properties of the intrinsics used by the main kernal class. Specifically,
this file does not contain any vector types or intrinsics since it is required for construction
and therefore must be suitable for the aie compiler graph-level compilation.
*/

// The following is a set of type-specialized functions which return the number of accumulator registers
// available in the processor. Since these may be 384 or 768 bit registers the number could vary by type.
template <typename TT_DATA_A, typename TT_DATA_B>
unsigned int fnAccRegsMatMult() {
    return 0;
}; // default error trap
template <>
INLINE_DECL constexpr unsigned int fnAccRegsMatMult<int16, int16>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnAccRegsMatMult<cint16, int16>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnAccRegsMatMult<cint16, cint16>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnAccRegsMatMult<int32, int16>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnAccRegsMatMult<int32, int32>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnAccRegsMatMult<cint32, int16>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnAccRegsMatMult<cint32, cint16>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnAccRegsMatMult<cint32, int32>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnAccRegsMatMult<cint32, cint32>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnAccRegsMatMult<float, float>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnAccRegsMatMult<cfloat, float>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnAccRegsMatMult<cfloat, cfloat>() {
    return 4;
};

// function to return the number of lanes for a type combo
// The default is effectively an error trap, but adding an error message to a constexpr return results in a warning.
template <typename TT_DATA_A, typename TT_DATA_B>
INLINE_DECL constexpr unsigned int fnNumLanesMatMult() {
    return 0;
};
template <>
INLINE_DECL constexpr unsigned int fnNumLanesMatMult<int16, int16>() {
    return 16;
};
template <>
INLINE_DECL constexpr unsigned int fnNumLanesMatMult<cint16, int16>() {
    return 8;
};
template <>
INLINE_DECL constexpr unsigned int fnNumLanesMatMult<cint16, cint16>() {
    return 8;
};
template <>
INLINE_DECL constexpr unsigned int fnNumLanesMatMult<int32, int16>() {
    return 8;
};
template <>
INLINE_DECL constexpr unsigned int fnNumLanesMatMult<int32, int32>() {
    return 8;
};
template <>
INLINE_DECL constexpr unsigned int fnNumLanesMatMult<cint32, int16>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnNumLanesMatMult<cint32, cint16>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnNumLanesMatMult<cint32, int32>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnNumLanesMatMult<cint32, cint32>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnNumLanesMatMult<float, float>() {
    return 8;
};
template <>
INLINE_DECL constexpr unsigned int fnNumLanesMatMult<cfloat, float>() {
    return 4;
};
template <>
INLINE_DECL constexpr unsigned int fnNumLanesMatMult<cfloat, cfloat>() {
    return 4;
};

// Function to return the lowest common multiple of two numbers
// A full implementation of this would entail prime factor decomposition, but here
// The maximum integer size is 16, so a simpler brute force method will do.
template <typename TT_DATA_A, typename TT_DATA_B, unsigned int TP_FACTOR>
INLINE_DECL constexpr unsigned int fnLCMMatMult() {
    return ((fnNumLanesMatMult<TT_DATA_A, TT_DATA_B>() == 2)
                ? ((TP_FACTOR % 2 == 0) ? TP_FACTOR : (TP_FACTOR * 2))
                : (fnNumLanesMatMult<TT_DATA_A, TT_DATA_B>() == 4)
                      ? ((TP_FACTOR % 4 == 0) ? TP_FACTOR : ((TP_FACTOR % 2 == 0) ? (TP_FACTOR * 2) : (TP_FACTOR * 4)))
                      : (fnNumLanesMatMult<TT_DATA_A, TT_DATA_B>() == 8)
                            ? ((TP_FACTOR % 8 == 0)
                                   ? TP_FACTOR
                                   : ((TP_FACTOR % 4 == 0) ? (TP_FACTOR * 2)
                                                           : ((TP_FACTOR % 2 == 0) ? (TP_FACTOR * 4) : TP_FACTOR * 8)))
                            : 0);
};

// function to return the number of samples in an output vector for a type combo
template <typename TT_DATA_A, typename TT_DATA_B>
INLINE_DECL constexpr unsigned int fnVOutSizeMatMult() {
    return fnNumLanesMatMult<TT_DATA_A, TT_DATA_B>();
};
}
}
}
}
}

#endif // _TEST_HARNESS_MATRIX_MULT_TRAITS_HPP_
