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

#ifndef _TEST_HARNESS_MATRIX_MULT_HPP_
#define _TEST_HARNESS_MATRIX_MULT_HPP_
/*
Matrix Multiply Definition

The file holds the definition of the Matrix Multiply kernel class.

*/

/* Coding conventions
  TT_      template type suffix
  TP_      template parameter suffix
*/

#include <adf.h>

#include <assert.h>
#include <array>
#include <cstdint>
#include <type_traits>

#include "fir_utils.hpp"
#include "matrix_mult_traits.hpp"
// added for stubs in hw flow
#include "matrix_mult_tiler.hpp"
#include "matrix_mult_untiler.hpp"

// CEIL rounds x up to the next multiple of y, which may be x itself.
#define CEIL(x, y) (((x + y - 1) / y) * y)
// Whichever type has the largest size (will be more complicated in future)
//#ifndef GET_TT_OUT
//#define GET_TT_OUT(A,B) std::conditional_t<(sizeof(B) > sizeof(A)),B, A>
//#endif //GET_TT_OUT
#ifndef ROW_MAJOR
#define ROW_MAJOR 0
#endif // ROW_MAJOR
#ifndef COL_MAJOR
#define COL_MAJOR 1
#endif // COL_MAJOR

namespace xf {
namespace dsp {
namespace aie {
namespace blas {
namespace matrix_mult {

// TO BE MOVED:
// IF input type
struct no_port {};
template <typename T_A, typename T_B>
struct accType {
    using type = cacc48;
};
template <typename T_A, typename T_B>
struct outType {
    using type = cint16;
};
template <>
struct accType<int16, int16> {
    using type = acc48;
};
template <>
struct outType<int16, int16> {
    using type = int16;
};
template <>
struct accType<int16, cint16> {
    using type = cacc48;
};
template <>
struct outType<int16, cint16> {
    using type = cint16;
};
template <>
struct accType<int16, cint32> {
    using type = cacc80;
};
template <>
struct outType<int16, cint32> {
    using type = cint32;
};
template <>
struct accType<int16, int32> {
    using type = acc80;
};
template <>
struct outType<int16, int32> {
    using type = int32;
};

template <>
struct accType<cint16, int16> {
    using type = cacc48;
};
template <>
struct outType<cint16, int16> {
    using type = cint16;
};
template <>
struct accType<cint16, cint16> {
    using type = cacc48;
};
template <>
struct outType<cint16, cint16> {
    using type = cint16;
};
template <>
struct accType<cint16, int32> {
    using type = cacc80;
};
template <>
struct outType<cint16, int32> {
    using type = cint32;
};
template <>
struct accType<cint16, cint32> {
    using type = cacc80;
};
template <>
struct outType<cint16, cint32> {
    using type = cint32;
};

template <>
struct accType<int32, int16> {
    using type = acc80;
};
template <>
struct outType<int32, int16> {
    using type = int32;
};
template <>
struct accType<int32, cint16> {
    using type = cacc80;
};
template <>
struct outType<int32, cint16> {
    using type = cint32;
};
template <>
struct accType<int32, int32> {
    using type = acc80;
};
template <>
struct outType<int32, int32> {
    using type = int32;
};
template <>
struct accType<int32, cint32> {
    using type = cacc80;
};
template <>
struct outType<int32, cint32> {
    using type = cint32;
};

template <>
struct accType<cint32, int16> {
    using type = cacc80;
};
template <>
struct outType<cint32, int16> {
    using type = cint32;
};
template <>
struct accType<cint32, cint16> {
    using type = cacc80;
};
template <>
struct outType<cint32, cint16> {
    using type = cint32;
};
template <>
struct accType<cint32, int32> {
    using type = cacc80;
};
template <>
struct outType<cint32, int32> {
    using type = cint32;
};
template <>
struct accType<cint32, cint32> {
    using type = cacc80;
};
template <>
struct outType<cint32, cint32> {
    using type = cint32;
};

template <>
struct accType<float, float> {
    using type = accfloat;
};
template <>
struct outType<float, float> {
    using type = float;
};
template <>
struct accType<cfloat, float> {
    using type = caccfloat;
};
template <>
struct outType<cfloat, float> {
    using type = cfloat;
};
template <>
struct accType<float, cfloat> {
    using type = caccfloat;
};
template <>
struct outType<float, cfloat> {
    using type = cfloat;
};
template <>
struct accType<cfloat, cfloat> {
    using type = caccfloat;
};
template <>
struct outType<cfloat, cfloat> {
    using type = cfloat;
};

template <typename T_D_A, typename T_D_B>
using accType_t = typename accType<T_D_A, T_D_B>::type;
template <typename T_D_A, typename T_D_B>
using outType_t = typename outType<T_D_A, T_D_B>::type;

template <bool T_CASC_IN, typename T_D_A, typename T_D_B>
struct T_inputIF {
    void* __restrict inWindowA;
    void* __restrict inWindowB;
    typename std::conditional<T_CASC_IN == CASC_IN_FALSE, no_port, input_stream<accType_t<T_D_A, T_D_B> > >::type*
        inCascade;
};

// IF output type
template <bool T_CASC_OUT, typename T_D_A, typename T_D_B>
struct T_outputIF {
    typename std::conditional<T_CASC_OUT == CASC_OUT_FALSE,
                              outType_t<T_D_A, T_D_B>,
                              output_stream<accType_t<T_D_A, T_D_B> > >::type* __restrict outWindow;
};

// TODO: Move this into a common dsp::aie namespace.
// Functions to support defensive checks
enum { enumUnknownType = 0, enumInt16, enumCint16, enumInt32, enumCint32, enumFloat, enumCfloat };
// function to return an enumeration of the data or coefficient type
template <typename TT_INPUT>
INLINE_DECL constexpr unsigned int fnEnumType() {
    return enumUnknownType;
}; // returns 0 as default. This can be trapped as an error;
template <>
INLINE_DECL constexpr unsigned int fnEnumType<int16>() {
    return enumInt16;
};
template <>
INLINE_DECL constexpr unsigned int fnEnumType<cint16>() {
    return enumCint16;
};
template <>
INLINE_DECL constexpr unsigned int fnEnumType<int32>() {
    return enumInt32;
};
template <>
INLINE_DECL constexpr unsigned int fnEnumType<cint32>() {
    return enumCint32;
};
template <>
INLINE_DECL constexpr unsigned int fnEnumType<float>() {
    return enumFloat;
};
template <>
INLINE_DECL constexpr unsigned int fnEnumType<cfloat>() {
    return enumCfloat;
};

//-----------------------------------------------------------------------------------------------------
template <typename TT_DATA_A,
          typename TT_DATA_B,
          unsigned int TP_DIM_A,
          unsigned int TP_DIM_AB,
          unsigned int TP_DIM_B,
          unsigned int TP_SHIFT,
          unsigned int TP_RND,
          unsigned int TP_DIM_A_LEADING = ROW_MAJOR,
          unsigned int TP_DIM_B_LEADING = COL_MAJOR,
          unsigned int TP_DIM_OUT_LEADING = ROW_MAJOR,
          unsigned int TP_INPUT_WINDOW_VSIZE_A = TP_DIM_A* TP_DIM_AB,
          unsigned int TP_INPUT_WINDOW_VSIZE_B = TP_DIM_B* TP_DIM_AB,
          bool TP_CASC_IN = CASC_IN_FALSE,
          bool TP_CASC_OUT = CASC_OUT_FALSE,
          unsigned int TP_DIM_A_RANGE = TP_DIM_A,
          unsigned int TP_DIM_AB_RANGE = TP_DIM_AB,
          unsigned int TP_DIM_B_RANGE = TP_DIM_B,
          unsigned int TP_KERNEL_POSITION = 0,
          unsigned int TP_CASC_LEN = 1>
class kernelMatMultClass {
   protected:
    // Members defined here can be changed in derived classes to support customer inheritance.
    using TT_OUT = outType_t<TT_DATA_A, TT_DATA_B>;
    // These will actually be the result of a constexpr function, depending on
    // how well the B data would fit into the A data. Or how awkward it is to
    // load the data due to LEADING_DIM.
    using m_tZbuff = TT_DATA_B;
    using m_tXbuff = TT_DATA_A;

   private:
    static_assert((((TP_CASC_LEN == 1) && (TP_KERNEL_POSITION == 0)) &&
                   ((TP_DIM_A_RANGE == TP_DIM_A) && (TP_DIM_B_RANGE == TP_DIM_B) && (TP_DIM_AB_RANGE == TP_DIM_AB))),
                  "ERROR: Cascading/Tiling is not currently available. ");

    static_assert(((TP_INPUT_WINDOW_VSIZE_A == (TP_DIM_A * TP_DIM_AB)) &&
                   (TP_INPUT_WINDOW_VSIZE_B == (TP_DIM_B * TP_DIM_AB))),
                  "ERROR: Batch window processing is not currently available. ");

    // static_assert((
    //    (TP_DIM_A_LEADING == ROW_MAJOR) &&
    //    (TP_DIM_B_LEADING == COL_MAJOR) &&
    //    (TP_DIM_OUT_LEADING == ROW_MAJOR)),
    //    "ERROR: Other memory storage options are currently unavailable. Please transpose input/output matrices to
    //    achieve desired alignment.");

    // Not sure exactly - other __restrictions will hit first
    static const int TP_DIM_MIN = 4;
    // Parameter value defensive and legality checks
    static_assert(TP_DIM_A_RANGE* TP_DIM_AB_RANGE* TP_DIM_B_RANGE >= TP_DIM_MIN,
                  "ERROR: Illegal combination of design matrices and cascade length, resulting in kernel matrice sizes "
                  "below minimum required value. Should have at least TP_DIM_MIN macs. ");
    static_assert(TP_SHIFT >= SHIFT_MIN && TP_SHIFT <= SHIFT_MAX, "ERROR: SHIFT is out of the supported range.");
    static_assert(TP_RND >= ROUND_MIN && TP_RND <= ROUND_MAX, "ERROR: RND is out of the supported range.");
    static_assert((TP_INPUT_WINDOW_VSIZE_A % (TP_DIM_A * TP_DIM_AB)) == 0,
                  "ERROR: TP_INPUT_WINDOW_VSIZE_A must be an integer multiple of TP_DIM_A*TP_DIM_AB.");
    static_assert((TP_INPUT_WINDOW_VSIZE_B % (TP_DIM_B * TP_DIM_AB)) == 0,
                  "ERROR: TP_INPUT_WINDOW_VSIZE_B must be an integer multiple of TP_DIM_B*TP_DIM_AB.");
    static_assert((TP_INPUT_WINDOW_VSIZE_A * sizeof(TT_DATA_A)) <= 32768,
                  "ERROR: TP_INPUT_WINDOW_VSIZE_A must fit within a data memory bank of 32kB.");
    static_assert((TP_INPUT_WINDOW_VSIZE_B * sizeof(TT_DATA_B)) <= 32768,
                  "ERROR: TP_INPUT_WINDOW_VSIZE_B must fit within a data memory bank of 32kB.");
    static_assert(((TP_INPUT_WINDOW_VSIZE_A / TP_DIM_AB) * (TP_INPUT_WINDOW_VSIZE_B / TP_DIM_AB) * sizeof(TT_OUT)) <=
                      32768,
                  "ERROR: Output matrix must fit within a data memory bank of 32kB.");
    static_assert(!(std::is_same<TT_DATA_A, cfloat>::value || std::is_same<TT_DATA_A, float>::value) || (TP_SHIFT == 0),
                  "ERROR: TP_SHIFT cannot be performed for TT_DATA=cfloat, so must be set to 0"); // only necessary to
                                                                                                  // check TT_DATA_A as
                                                                                                  // TT_DATA_B will also
                                                                                                  // be float or integer
                                                                                                  // to match TT_DATA_A.

    static constexpr unsigned int m_kArch = 0; // no other arch right now

    // Only one implementation (with an old name rigt now)
    void matMult_impl1(T_inputIF<TP_CASC_IN, TT_DATA_A, TT_DATA_B> inInterface,
                       T_outputIF<TP_CASC_OUT, TT_DATA_A, TT_DATA_B> outInterface); // Each phase is calculated in turn
                                                                                    // which avoids need for multiple
                                                                                    // accumulators, but requires data
                                                                                    // reloading.

   public:
    // Access function for AIE Synthesizer
    unsigned int get_m_kArch() { return m_kArch; };

    struct tilingStruct {
        unsigned int Atile;
        unsigned int ABtile;
        unsigned int Btile;
    };

    static INLINE_DECL constexpr tilingStruct getTilingScheme() {
        using A = TT_DATA_A;
        using B = TT_DATA_B;
        // needs to be compatible with c++14 -> so just use plain ifs
        // 16b x 16b
        if (std::is_same<A, int16>::value && std::is_same<B, int16>::value) {
            return {4, 4, 4};
        }
        // 32b x 16b
        if ((std::is_same<A, cint16>::value || std::is_same<A, int32>::value) && std::is_same<B, int16>::value) {
            return {4, 4, 2};
        }
        // 16b x 32b
        if (std::is_same<A, int16>::value && (std::is_same<B, cint16>::value || std::is_same<B, int32>::value)) {
            return {4, 2, 2};
        }
        // 32b x 32b
        if (((std::is_same<A, cint16>::value || std::is_same<A, int32>::value) &&
             (std::is_same<B, cint16>::value || std::is_same<B, int32>::value)) ||
            std::is_same<A, float>::value && std::is_same<B, float>::value) {
            return {4, 4, 2};
        }
        // 64b x 16b
        if (std::is_same<A, cint32>::value && std::is_same<B, int16>::value) {
            return {2, 4, 2};
        }
        // 16b x 64b
        if (std::is_same<A, int16>::value && std::is_same<B, cint32>::value) {
            return {2, 4, 2}; // 4, 4, 2 is also ok
        }
        // 64b x 32b
        if (std::is_same<A, cint32>::value && (std::is_same<B, cint16>::value || std::is_same<B, int32>::value)) {
            return {2, 2, 2}; // 2, 4, 2 is also ok
        }
        // 32b x 64b
        if ((std::is_same<A, cint16>::value || std::is_same<A, int32>::value) && std::is_same<B, cint32>::value) {
            return {2, 2, 2};
        }
        // 64b x 64b
        if (std::is_same<A, cint32>::value && std::is_same<B, cint32>::value) {
            return {2, 2, 2};
        }
        // Mixed Floats
        if ((std::is_same<A, cfloat>::value && std::is_same<B, float>::value) ||
            (std::is_same<A, float>::value && std::is_same<B, cfloat>::value)) {
            return {2, 4, 2}; // 2, 2, 2 is also ok
        }
        // cfloats
        if (std::is_same<A, cfloat>::value && std::is_same<B, cfloat>::value) {
            return {4, 2, 2};
        }
    };
    // Putting this into a function so that the static assert error message includes the value of the tiling scheme.
    template <unsigned Atile, unsigned ABtile, unsigned Btile>
    static bool constexpr tilingSchemeMultiples() {
        static_assert(TP_DIM_A % Atile == 0, "Error: TP_DIM_A is not a multiple of the tiling scheme.");
        static_assert(TP_DIM_B % Btile == 0, "Error: TP_DIM_B is not a multiple of the tiling scheme.");
        static_assert(TP_DIM_AB % ABtile == 0, "Error: TP_DIM_AB is not a multiple of the tiling scheme.");
        return (TP_DIM_A % Atile == 0) && (TP_DIM_B % Btile == 0) && (TP_DIM_AB % ABtile == 0);
    }
    static constexpr tilingStruct tilingScheme = getTilingScheme();
    static_assert(tilingSchemeMultiples<tilingScheme.Atile, tilingScheme.ABtile, tilingScheme.Btile>(),
                  "Error: Dimensions are not multiples of tiling scheme.");

    // Constructor
    kernelMatMultClass(){};

    // FIR
    void matMultKernel(T_inputIF<TP_CASC_IN, TT_DATA_A, TT_DATA_B> inInterface,
                       T_outputIF<TP_CASC_OUT, TT_DATA_A, TT_DATA_B> outInterface);
};

//-----------------------------------------------------------------------------------------------------
// Cascade layer class and specializations

//-----------------------------------------------------------------------------------------------------
// This is the main declaration of the matrix_mult class, and is also used for the Standalone kernel specialization with
// no cascade ports, a single input and no reload
template <typename TT_DATA_A,
          typename TT_DATA_B,
          unsigned int TP_DIM_A,
          unsigned int TP_DIM_AB,
          unsigned int TP_DIM_B,
          unsigned int TP_SHIFT,
          unsigned int TP_RND,
          unsigned int TP_DIM_A_LEADING = ROW_MAJOR,
          unsigned int TP_DIM_B_LEADING = COL_MAJOR,
          unsigned int TP_DIM_OUT_LEADING = ROW_MAJOR,
          unsigned int TP_INPUT_WINDOW_VSIZE_A = TP_DIM_A* TP_DIM_AB,
          unsigned int TP_INPUT_WINDOW_VSIZE_B = TP_DIM_B* TP_DIM_AB,
          bool TP_CASC_IN = CASC_IN_FALSE,
          bool TP_CASC_OUT = CASC_OUT_FALSE,
          unsigned int TP_DIM_A_RANGE = TP_DIM_A,
          unsigned int TP_DIM_AB_RANGE = TP_DIM_AB,
          unsigned int TP_DIM_B_RANGE = TP_DIM_B,
          unsigned int TP_KERNEL_POSITION = 0,
          unsigned int TP_CASC_LEN = 1>
class matrix_mult : public kernelMatMultClass<TT_DATA_A,
                                              TT_DATA_B,
                                              TP_DIM_A,
                                              TP_DIM_AB,
                                              TP_DIM_B,
                                              TP_SHIFT,
                                              TP_RND,
                                              TP_DIM_A_LEADING,
                                              TP_DIM_B_LEADING,
                                              TP_DIM_OUT_LEADING,
                                              TP_INPUT_WINDOW_VSIZE_A,
                                              TP_INPUT_WINDOW_VSIZE_B,
                                              TP_CASC_IN,
                                              TP_CASC_OUT,
                                              TP_DIM_A,
                                              TP_DIM_AB,
                                              TP_DIM_B,
                                              TP_KERNEL_POSITION,
                                              TP_CASC_LEN> {
   private:
   public:
    // Constructor calls base class constructor (neither of which have anything to do)
    matrix_mult()
        : kernelMatMultClass<TT_DATA_A,
                             TT_DATA_B,
                             TP_DIM_A,
                             TP_DIM_AB,
                             TP_DIM_B,
                             TP_SHIFT,
                             TP_RND,
                             TP_DIM_A_LEADING,
                             TP_DIM_B_LEADING,
                             TP_DIM_OUT_LEADING,
                             TP_INPUT_WINDOW_VSIZE_A,
                             TP_INPUT_WINDOW_VSIZE_B,
                             TP_CASC_IN,
                             TP_CASC_OUT,
                             TP_DIM_A,
                             TP_DIM_AB,
                             TP_DIM_B,
                             TP_KERNEL_POSITION,
                             TP_CASC_LEN>() {}

    // Register Kernel Class
    static void registerKernelClass() { REGISTER_FUNCTION(matrix_mult::matMult); }

    // FIR
    void matMult(input_buffer<TT_DATA_A>& __restrict inWindowA,
                 input_buffer<TT_DATA_B>& __restrict inWindowB,
                 output_buffer<outType_t<TT_DATA_A, TT_DATA_B> >& __restrict outWindow);
};

//-----------------------------------------------------------------------------------------------------
// Partially specialized classes for cascaded interface (final kernel in cascade), single input, no reload
template <typename TT_DATA_A,
          typename TT_DATA_B,
          unsigned int TP_DIM_A,
          unsigned int TP_DIM_AB,
          unsigned int TP_DIM_B,
          unsigned int TP_SHIFT,
          unsigned int TP_RND,
          unsigned int TP_DIM_A_LEADING,
          unsigned int TP_DIM_B_LEADING,
          unsigned int TP_DIM_OUT_LEADING,
          unsigned int TP_INPUT_WINDOW_VSIZE_A,
          unsigned int TP_INPUT_WINDOW_VSIZE_B,
          unsigned int TP_DIM_A_RANGE,
          unsigned int TP_DIM_AB_RANGE,
          unsigned int TP_DIM_B_RANGE,
          unsigned int TP_KERNEL_POSITION,
          unsigned int TP_CASC_LEN>
class matrix_mult<TT_DATA_A,
                  TT_DATA_B,
                  TP_DIM_A,
                  TP_DIM_AB,
                  TP_DIM_B,
                  TP_SHIFT,
                  TP_RND,
                  TP_DIM_A_LEADING,
                  TP_DIM_B_LEADING,
                  TP_DIM_OUT_LEADING,
                  TP_INPUT_WINDOW_VSIZE_A,
                  TP_INPUT_WINDOW_VSIZE_B,
                  CASC_IN_TRUE,
                  CASC_OUT_FALSE,
                  TP_DIM_A_RANGE,
                  TP_DIM_AB_RANGE,
                  TP_DIM_B_RANGE,
                  TP_KERNEL_POSITION,
                  TP_CASC_LEN> : public kernelMatMultClass<TT_DATA_A,
                                                           TT_DATA_B,
                                                           TP_DIM_A,
                                                           TP_DIM_AB,
                                                           TP_DIM_B,
                                                           TP_SHIFT,
                                                           TP_RND,
                                                           TP_DIM_A_LEADING,
                                                           TP_DIM_B_LEADING,
                                                           TP_DIM_OUT_LEADING,
                                                           TP_INPUT_WINDOW_VSIZE_A,
                                                           TP_INPUT_WINDOW_VSIZE_B,
                                                           CASC_IN_TRUE,
                                                           CASC_OUT_FALSE,
                                                           TP_DIM_A_RANGE,
                                                           TP_DIM_AB_RANGE,
                                                           TP_DIM_B_RANGE,
                                                           TP_KERNEL_POSITION,
                                                           TP_CASC_LEN> {
   private:
   public:
    // Constructor (does nothing and neither does the base class, but this is left as a placeholder)
    matrix_mult()
        : kernelMatMultClass<TT_DATA_A,
                             TT_DATA_B,
                             TP_DIM_A,
                             TP_DIM_AB,
                             TP_DIM_B,
                             TP_SHIFT,
                             TP_RND,
                             TP_DIM_A_LEADING,
                             TP_DIM_B_LEADING,
                             TP_DIM_OUT_LEADING,
                             TP_INPUT_WINDOW_VSIZE_A,
                             TP_INPUT_WINDOW_VSIZE_B,
                             CASC_IN_TRUE,
                             CASC_OUT_FALSE,
                             TP_DIM_A_RANGE,
                             TP_DIM_AB_RANGE,
                             TP_DIM_B_RANGE,
                             TP_KERNEL_POSITION,
                             TP_CASC_LEN>() {}

    // Register Kernel Class
    static void registerKernelClass() { REGISTER_FUNCTION(matrix_mult::matMult); }

    // FIR
    void matMult(input_buffer<TT_DATA_A>& __restrict inWindowA,
                 input_buffer<TT_DATA_B>& __restrict inWindowB,
                 input_stream<accType_t<TT_DATA_A, TT_DATA_B> >* inCascade,
                 output_buffer<outType_t<TT_DATA_A, TT_DATA_B> >& __restrict outWindow);
};

//-----------------------------------------------------------------------------------------------------
// Partially specialized classes for cascaded interface (First kernel in cascade), single input, no reload
template <typename TT_DATA_A,
          typename TT_DATA_B,
          unsigned int TP_DIM_A,
          unsigned int TP_DIM_AB,
          unsigned int TP_DIM_B,
          unsigned int TP_SHIFT,
          unsigned int TP_RND,
          unsigned int TP_DIM_A_LEADING,
          unsigned int TP_DIM_B_LEADING,
          unsigned int TP_DIM_OUT_LEADING,
          unsigned int TP_INPUT_WINDOW_VSIZE_A,
          unsigned int TP_INPUT_WINDOW_VSIZE_B,
          unsigned int TP_DIM_A_RANGE,
          unsigned int TP_DIM_AB_RANGE,
          unsigned int TP_DIM_B_RANGE,
          unsigned int TP_KERNEL_POSITION,
          unsigned int TP_CASC_LEN>
class matrix_mult<TT_DATA_A,
                  TT_DATA_B,
                  TP_DIM_A,
                  TP_DIM_AB,
                  TP_DIM_B,
                  TP_SHIFT,
                  TP_RND,
                  TP_DIM_A_LEADING,
                  TP_DIM_B_LEADING,
                  TP_DIM_OUT_LEADING,
                  TP_INPUT_WINDOW_VSIZE_A,
                  TP_INPUT_WINDOW_VSIZE_B,
                  CASC_IN_FALSE,
                  CASC_OUT_TRUE,
                  TP_DIM_A_RANGE,
                  TP_DIM_AB_RANGE,
                  TP_DIM_B_RANGE,
                  TP_KERNEL_POSITION,
                  TP_CASC_LEN> : public kernelMatMultClass<TT_DATA_A,
                                                           TT_DATA_B,
                                                           TP_DIM_A,
                                                           TP_DIM_AB,
                                                           TP_DIM_B,
                                                           TP_SHIFT,
                                                           TP_RND,
                                                           TP_DIM_A_LEADING,
                                                           TP_DIM_B_LEADING,
                                                           TP_DIM_OUT_LEADING,
                                                           TP_INPUT_WINDOW_VSIZE_A,
                                                           TP_INPUT_WINDOW_VSIZE_B,
                                                           CASC_IN_FALSE,
                                                           CASC_OUT_TRUE,
                                                           TP_DIM_A_RANGE,
                                                           TP_DIM_AB_RANGE,
                                                           TP_DIM_B_RANGE,
                                                           TP_KERNEL_POSITION,
                                                           TP_CASC_LEN> {
   private:
   public:
    // Constructor
    matrix_mult()
        : kernelMatMultClass<TT_DATA_A,
                             TT_DATA_B,
                             TP_DIM_A,
                             TP_DIM_AB,
                             TP_DIM_B,
                             TP_SHIFT,
                             TP_RND,
                             TP_DIM_A_LEADING,
                             TP_DIM_B_LEADING,
                             TP_DIM_OUT_LEADING,
                             TP_INPUT_WINDOW_VSIZE_A,
                             TP_INPUT_WINDOW_VSIZE_B,
                             CASC_IN_FALSE,
                             CASC_OUT_TRUE,
                             TP_DIM_A_RANGE,
                             TP_DIM_AB_RANGE,
                             TP_DIM_B_RANGE,
                             TP_KERNEL_POSITION,
                             TP_CASC_LEN>() {}

    // Register Kernel Class
    static void registerKernelClass() { REGISTER_FUNCTION(matrix_mult::matMult); }

    // FIR
    void matMult(input_buffer<TT_DATA_A>& __restrict inWindowA,
                 input_buffer<TT_DATA_B>& __restrict inWindowB,
                 output_stream<accType_t<TT_DATA_A, TT_DATA_B> >* outCascade);
};

//-----------------------------------------------------------------------------------------------------
// Partially specialized classes for cascaded interface (middle kernels in cascade), single input, no reload
template <typename TT_DATA_A,
          typename TT_DATA_B,
          unsigned int TP_DIM_A,
          unsigned int TP_DIM_AB,
          unsigned int TP_DIM_B,
          unsigned int TP_SHIFT,
          unsigned int TP_RND,
          unsigned int TP_DIM_A_LEADING,
          unsigned int TP_DIM_B_LEADING,
          unsigned int TP_DIM_OUT_LEADING,
          unsigned int TP_INPUT_WINDOW_VSIZE_A,
          unsigned int TP_INPUT_WINDOW_VSIZE_B,
          unsigned int TP_DIM_A_RANGE,
          unsigned int TP_DIM_AB_RANGE,
          unsigned int TP_DIM_B_RANGE,
          unsigned int TP_KERNEL_POSITION,
          unsigned int TP_CASC_LEN>
class matrix_mult<TT_DATA_A,
                  TT_DATA_B,
                  TP_DIM_A,
                  TP_DIM_AB,
                  TP_DIM_B,
                  TP_SHIFT,
                  TP_RND,
                  TP_DIM_A_LEADING,
                  TP_DIM_B_LEADING,
                  TP_DIM_OUT_LEADING,
                  TP_INPUT_WINDOW_VSIZE_A,
                  TP_INPUT_WINDOW_VSIZE_B,
                  CASC_IN_TRUE,
                  CASC_OUT_TRUE,
                  TP_DIM_A_RANGE,
                  TP_DIM_AB_RANGE,
                  TP_DIM_B_RANGE,
                  TP_KERNEL_POSITION,
                  TP_CASC_LEN> : public kernelMatMultClass<TT_DATA_A,
                                                           TT_DATA_B,
                                                           TP_DIM_A,
                                                           TP_DIM_AB,
                                                           TP_DIM_B,
                                                           TP_SHIFT,
                                                           TP_RND,
                                                           TP_DIM_A_LEADING,
                                                           TP_DIM_B_LEADING,
                                                           TP_DIM_OUT_LEADING,
                                                           TP_INPUT_WINDOW_VSIZE_A,
                                                           TP_INPUT_WINDOW_VSIZE_B,
                                                           CASC_IN_TRUE,
                                                           CASC_OUT_TRUE,
                                                           TP_DIM_A_RANGE,
                                                           TP_DIM_AB_RANGE,
                                                           TP_DIM_B_RANGE,
                                                           TP_KERNEL_POSITION,
                                                           TP_CASC_LEN> {
   private:
   public:
    // Constructor
    matrix_mult()
        : kernelMatMultClass<TT_DATA_A,
                             TT_DATA_B,
                             TP_DIM_A,
                             TP_DIM_AB,
                             TP_DIM_B,
                             TP_SHIFT,
                             TP_RND,
                             TP_DIM_A_LEADING,
                             TP_DIM_B_LEADING,
                             TP_DIM_OUT_LEADING,
                             TP_INPUT_WINDOW_VSIZE_A,
                             TP_INPUT_WINDOW_VSIZE_B,
                             CASC_IN_TRUE,
                             CASC_OUT_TRUE,
                             TP_DIM_A_RANGE,
                             TP_DIM_AB_RANGE,
                             TP_DIM_B_RANGE,
                             TP_KERNEL_POSITION,
                             TP_CASC_LEN>() {}

    // Register Kernel Class
    static void registerKernelClass() { REGISTER_FUNCTION(matrix_mult::matMult); }

    // FIR
    void matMult(input_buffer<TT_DATA_A>& __restrict inWindowA,
                 input_buffer<TT_DATA_B>& __restrict inWindowB,
                 input_stream<accType_t<TT_DATA_A, TT_DATA_B> >* inCascade,
                 output_stream<accType_t<TT_DATA_A, TT_DATA_B> >* outCascade);
};
}
}
}
}
}

#endif // _TEST_HARNESS_MATRIX_MULT_HPP_
