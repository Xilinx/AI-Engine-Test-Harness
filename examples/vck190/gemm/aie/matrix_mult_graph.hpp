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

#ifndef _TEST_HARNESS_MATRIX_MULT_GRAPH_HPP_
#define _TEST_HARNESS_MATRIX_MULT_GRAPH_HPP_

// This file holds the definition of the matrix mult graph class
/**
 * @file matrix_mult_graph.hpp
 *
 **/

#include <adf.h>
#include <vector>
#include "graph_utils.hpp"

#include "matrix_mult.hpp"
#include "matrix_mult_untiler.hpp"
#include "matrix_mult_tiler.hpp"

namespace xf {
namespace dsp {
namespace aie {
namespace blas {
namespace matrix_mult {

using namespace adf;

/**
 * @defgroup gemm_graph GEneral Matrix Multiply (GEMM)
 *
 * Matrix Multiply/GEMM (GEneral Matrix Multiply) solution.
 *
 */

/**
 * @brief matrix_mult performs a GEneral Matrix Multiply (GEMM), taking two input matrices of configurable dimensions
 *and data type.
 *
 * These are the templates to configure the Matrix Multiply graph class.
 *
 * @ingroup gemm_graph
 *
 * @tparam TT_DATA_A describes the type of individual data samples input of
 *         Matrix A to the gemm function. This is a typename and must be one
 *         of the following: \n
 *         int16, cint16, int32, cint32, float, cfloat.
 * @tparam TT_DATA_B describes the type of individual data samples input of
 *         Matrix B to the gemm function. This is a typename and must be one
 *         of the following: \n
 *         int16, cint16, int32, cint32, float, cfloat.
 *         The following rules apply:
 *         - must be an integer type if TT_DATA_A is an integer type
 *         - must be a float type if TT_DATA_A is a float type.
 * @tparam TP_DIM_A is an unsigned integer which describes the number of elements
 *          along the unique dimension (rows) of Matrix A.
 * @tparam TP_DIM_AB is an unsigned integer which describes the number of elements
 *          along the common dimension of Matrix A (columns) and Matrix B (rows).
 * @tparam TP_DIM_B is an unsigned integer which describes the number of elements
 *          along the unique dimension (columns) of Matrix B.
 * @tparam TP_SHIFT describes power of 2 shift down applied to the accumulation of
 *         product terms before each output. TP_SHIFT must be in the range 0 to 61.
 * @tparam TP_RND describes the selection of rounding to be applied during the
 *         shift down stage of processing. Although, TP_RND accepts unsignedinteger values
 *         descriptive macros are recommended where
 *         - rnd_floor      = Truncate LSB, always round down (towards negative infinity).
 *         - rnd_ceil       = Always round up (towards positive infinity).
 *         - rnd_sym_floor  = Truncate LSB, always round towards 0.
 *         - rnd_sym_ceil   = Always round up towards infinity.
 *         - rnd_pos_inf    = Round halfway towards positive infinity.
 *         - rnd_neg_inf    = Round halfway towards negative infinity.
 *         - rnd_sym_inf    = Round halfway towards infinity (away from zero).
 *         - rnd_sym_zero   = Round halfway towards zero (away from infinity).
 *         - rnd_conv_even  = Round halfway towards nearest even number.
 *         - rnd_conv_odd   = Round halfway towards nearest odd number. \n
 *         No rounding is performed on ceil or floor mode variants. \n
 *         Other modes round to the nearest integer. They differ only in how
 *         they round for values of 0.5. \n
 *         Note: Rounding modes ``rnd_sym_floor`` and ``rnd_sym_ceil`` are only supported on AIE-ML device. \n
 * @tparam TP_DIM_A_LEADING describes the scheme in which the data should be stored
 *         in memory. ROW_MAJOR = 0, COL_MAJOR = 1. Note, a COL_MAJOR matrix can be
 *         transposed to become a ROW_MAJOR matrix.
 * @tparam TP_DIM_B_LEADING describes the scheme in which the data should be stored
 *         in memory. ROW_MAJOR = 0, COL_MAJOR = 1.
 * @tparam TP_DIM_OUT_LEADING describes the scheme in which the data should be stored
 *         in memory. ROW_MAJOR = 0, COL_MAJOR = 1.
 * @tparam TP_ADD_TILING_A describes wether or not to add an additional kernel to
 *          rearrange the matrix samples into their required position. \n Setting this
 *          option to 0 indicates that the re-arrangement will be done externally to
 *          the AIE matrix multiply graph.
 * @tparam TP_ADD_TILING_B describes wether or not to add an additional kernel to
 *          rearrange the matrix samples into their required position. \n Setting this
 *          option to 0 indicates that the re-arrangement will be done externally to
 *          the AIE matrix multiply graph.
 * @tparam TP_ADD_DETILING_OUT describes wether or not to add an additional kernel to
 *          rearrange the matrix samples into their required position. \n Setting this
 *          option to 0 indicates that the re-arrangement will be done externally to
 *          the AIE matrix multiply graph.
 * @tparam TP_INPUT_WINDOW_VSIZE_A describes the number of samples in the window API
 *         used for input to Matrix A. \n It must be of size TP_DIM_A*TP_DIM_AB*N.
 *         Typical use has N=1, however N>1 can be utilised to minimise overhead of
 *         window API. \n This parameter is optional and has a default value of
 *         TP_DIM_A*TP_DIM_AB (N=1).
 * @tparam TP_INPUT_WINDOW_VSIZE_B describes the number of samples in the window API
 *         used for input to Matrix B. \n It must be of size TP_DIM_B*TP_DIM_AB*M.
 *         Typical use has M=1, however M>1 can be utilised to minimise overhead of
 *         window API. \n This parameter is optional and has a default value of
 *         TP_DIM_B*TP_DIM_AB (M=1). \n
 *         Note, the output window will be of size:
 *           (TP_INPUT_WINDOW_VSIZE_A/TP_DIM_AB * TP_INPUT_WINDOW_VSIZE_B/TP_DIM_AB).
 *          When N and M is 1, output window size will be TP_DIM_A * TP_DIM_B.
 * @tparam TP_CASC_LEN describes the number of AIE Tiles to split the GEMM operation into. \n
 *         TP_CASC_LEN splits the operation over TP_DIM_AB, where each kernel
 *         utilises the cascade stream to pass partial accumulation results to
 *         the next kernel. In effect, dot(A,B) + C. \n
 *         Note, it is also possible to tile the operation over multiple AIE tiles
 *         by instantiating multiple GEMM graphs with smaller dimensions.
 *
**/

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
          unsigned int TP_ADD_TILING_A = 1,
          unsigned int TP_ADD_TILING_B = 1,
          unsigned int TP_ADD_DETILING_OUT = 1,
          unsigned int TP_INPUT_WINDOW_VSIZE_A = TP_DIM_A* TP_DIM_AB,
          unsigned int TP_INPUT_WINDOW_VSIZE_B = TP_DIM_B* TP_DIM_AB,
          unsigned int TP_CASC_LEN = 1>
class matrix_mult_graph : public graph {
   public:
    /**
     * The input A data to the function. This input is a window of samples of
     * TT_DATA_A type. The number of samples in the window is
     * described by TP_INPUT_WINDOW_VSIZE_A, which is
     * derived from TP_DIM_A, TP_DIM_AB.
     **/
    port<input> inA[TP_CASC_LEN];

    /**
     * The input B data to the function. This input is a window of samples of
     * TT_DATA_B type. The number of samples in the window is
     * described by TP_INPUT_WINDOW_VSIZE_B, which is
     * derived from TP_DIM_AB and TP_DIM_B.
     **/
    port<input> inB[TP_CASC_LEN];

    /**
     * A window API of
     * TP_INPUT_WINDOW_VSIZE_A/TP_DIM_AB * TP_INPUT_WINDOW_VSIZE_B/TP_DIM_AB samples,
     * or simply TP_DIM_A * TP_DIM_B samples of a derived output type.
     **/
    port<output> out;

    /**
     * The array of kernels that will be created and mapped onto AIE tiles.
     * Number of kernels (``TP_CASC_LEN``) will be connected with each other by cascade interface.
     **/
    kernel m_MatmultKernels[TP_CASC_LEN];

    /**
     * The kernel that that will be created when output tiling is enabled (``TP_ADD_DETILING_OUT = 1``).
     **/
    kernel untiler;

    /**
     * The array of kernels that will be created when tiling on input A is enabled (``TP_ADD_TILING_A = 1``).
     * Kernels will pre-process and sent the data through cascade interface to corresponding: ``m_MatmultKernels``.
     **/
    kernel tilerA[TP_CASC_LEN];

    /**
     * The array of kernels that will be created when tiling on input A is enabled (``TP_ADD_TILING_A = 1``).
     * Kernels will pre-process and sent the data through cascade interface to corresponding: ``m_MatmultKernels``.
     **/
    kernel tilerB[TP_CASC_LEN];

    /**
     * Access function to get pointer to kernel (or first kernel in a chained configuration).
     **/

    kernel* getKernels() { return m_MatmultKernels; };

    // Empty type for a fallback to avoid redundant instantiations from the compiler in x86
    struct no_kernel {};
    // TODO: Have the first or last kernel take the remainder. ie DIM_AB=15 and CASC =2; should be one kernel of 8 and
    // one kernel of 6, where we round by tilingScheme.ABtile.
    static_assert(TP_DIM_AB % TP_CASC_LEN == 0, "TP_DIM_AB needs to be a multiple of TP_CASC_LEN");
    template <bool cascIn, bool cascOut>
    using matMultCasc = matrix_mult<TT_DATA_A,
                                    TT_DATA_B,
                                    TP_DIM_A,
                                    (TP_DIM_AB / TP_CASC_LEN),
                                    TP_DIM_B,
                                    TP_SHIFT,
                                    TP_RND,
                                    TP_DIM_A_LEADING,
                                    TP_DIM_B_LEADING,
                                    TP_DIM_OUT_LEADING,
                                    (TP_INPUT_WINDOW_VSIZE_A / TP_CASC_LEN),
                                    (TP_INPUT_WINDOW_VSIZE_B / TP_CASC_LEN),
                                    cascIn,
                                    cascOut>;
    // avoid redundant instances of unsued templates -- Fixes x86sim linker error without resorting to recursive
    // template metaprogramming
    using onlyMatMult = typename std::conditional<(TP_CASC_LEN == 1), matMultCasc<false, false>, no_kernel>::type;
    using firstMatMult = typename std::conditional<(TP_CASC_LEN > 1), matMultCasc<false, true>, onlyMatMult>::type;
    using lastMatMult = typename std::conditional<(TP_CASC_LEN > 1), matMultCasc<true, false>, firstMatMult>::type;
    using middleMatMult = typename std::conditional<(TP_CASC_LEN > 2), matMultCasc<true, true>, lastMatMult>::type;

    // Todo structured binding instead (C++17)
    // AIE_API tiling scheme in use - single configuration for each data type.
    // Tiling scheme doesn't change vs cascade or not.
    static constexpr typename middleMatMult::tilingStruct tilingScheme = middleMatMult::getTilingScheme();

    // Forward compatible for batch window processing.
    static constexpr unsigned int dimAPerKernel = (TP_INPUT_WINDOW_VSIZE_A / TP_DIM_AB);
    static constexpr unsigned int dimBPerKernel = (TP_INPUT_WINDOW_VSIZE_B / TP_DIM_AB);

    using TilerClassA = tilerKernelClass<tilingScheme.Atile,
                                         tilingScheme.ABtile,
                                         dimAPerKernel,
                                         (TP_DIM_AB / TP_CASC_LEN),
                                         TP_DIM_A_LEADING,
                                         TT_DATA_A>;
    using TilerClassB = tilerKernelClass<tilingScheme.ABtile,
                                         tilingScheme.Btile,
                                         (TP_DIM_AB / TP_CASC_LEN),
                                         dimBPerKernel,
                                         TP_DIM_B_LEADING,
                                         TT_DATA_B>;
    using DetilerClassOut = untilerKernelClass<tilingScheme.Atile,
                                               tilingScheme.Btile,
                                               dimAPerKernel,
                                               dimBPerKernel,
                                               TP_DIM_OUT_LEADING,
                                               outType_t<TT_DATA_A, TT_DATA_B> >;

    static constexpr bool isRedundantTilerA =
        (((TP_DIM_AB / TP_CASC_LEN) <= tilingScheme.ABtile) && (TP_DIM_A_LEADING == ROW_MAJOR));
    static constexpr bool isRedundantTilerB =
        ((dimBPerKernel <= tilingScheme.Btile) && (TP_DIM_B_LEADING == ROW_MAJOR));
    static constexpr bool isRedundantTilerOut =
        ((dimBPerKernel <= tilingScheme.Btile) && (TP_DIM_OUT_LEADING == ROW_MAJOR));

    /**
     * @brief This is the constructor function for the Matric Multiply graph.
     **/
    matrix_mult_graph() {
        if (isRedundantTilerA && TP_ADD_TILING_A == 1) {
            printf(
                "WARNING: TP_ADD_TILING_A is true, but P_DIM_AB is small enough that tiling is not nessecary for this "
                "configuration. TP_ADD_TILING_A will be ignored. \n");
        }
        if (isRedundantTilerB && TP_ADD_TILING_B == 1) {
            printf(
                "WARNING: TP_ADD_TILING_B is true, but P_DIM_B is small enough that tiling is not nessecary for this "
                "configuration. TP_ADD_TILING_B will be ignored. \n");
        }
        if (isRedundantTilerOut && TP_ADD_DETILING_OUT == 1) {
            printf(
                "WARNING: TP_ADD_DETILING_OUT is true, but P_DIM_B is small enough that detiling is not nessecary for "
                "this configuration. TP_ADD_DETILING_OUT will be ignored. \n");
        }
        printf("\n");

        // make input connections
        for (int i = 0; i < TP_CASC_LEN; i++) {
            if (i >= 1 && i < (TP_CASC_LEN - 1)) {
                // both casccade
                m_MatmultKernels[i] = kernel::create_object<middleMatMult>();

            } else if (i >= 1 && i >= (TP_CASC_LEN - 1)) {
                // last kernel
                m_MatmultKernels[i] = kernel::create_object<lastMatMult>();
            } else {
                // first kernel
                m_MatmultKernels[i] = kernel::create_object<firstMatMult>();
            }
            if (i >= 1) {
                connect<cascade>(m_MatmultKernels[i - 1].out[0], m_MatmultKernels[i].in[2]);
            }
            // TODO, different window sizes for end kernel if the window size doesn't evenly split by CASC_LEN.

            if
                constexpr(!isRedundantTilerA && TP_ADD_TILING_A) {
                    tilerA[i] = kernel::create_object<TilerClassA>();
                    connect<>(inA[i], tilerA[i].in[0]);
                    dimensions(tilerA[i].in[0]) = {TP_INPUT_WINDOW_VSIZE_A / TP_CASC_LEN};
                    connect<>(tilerA[i].out[0], m_MatmultKernels[i].in[0]);
                    dimensions(tilerA[i].out[0]) = {TP_INPUT_WINDOW_VSIZE_A / TP_CASC_LEN};
                    dimensions(m_MatmultKernels[i].in[0]) = {TP_INPUT_WINDOW_VSIZE_A / TP_CASC_LEN};
                }
            else {
                connect<>(inA[i], m_MatmultKernels[i].in[0]);
                dimensions(m_MatmultKernels[i].in[0]) = {TP_INPUT_WINDOW_VSIZE_A / TP_CASC_LEN};
            }

            if
                constexpr(!isRedundantTilerB && TP_ADD_TILING_B) {
                    tilerB[i] = kernel::create_object<TilerClassB>();
                    connect<>(inB[i], tilerB[i].in[0]);
                    dimensions(tilerB[i].in[0]) = {TP_INPUT_WINDOW_VSIZE_B / TP_CASC_LEN};
                    connect<>(tilerB[i].out[0], m_MatmultKernels[i].in[1]);
                    dimensions(tilerB[i].out[0]) = {TP_INPUT_WINDOW_VSIZE_B / TP_CASC_LEN};
                    dimensions(m_MatmultKernels[i].in[1]) = {TP_INPUT_WINDOW_VSIZE_B / TP_CASC_LEN};
                }
            else {
                connect<>(inB[i], m_MatmultKernels[i].in[1]);
                dimensions(m_MatmultKernels[i].in[1]) = {TP_INPUT_WINDOW_VSIZE_B / TP_CASC_LEN};
            }
            // Specify mapping constraints - Can be overriden in parent graph.
            runtime<ratio>(m_MatmultKernels[i]) = 0.8;
            runtime<ratio>(tilerA[i]) = 0.4;
            runtime<ratio>(tilerB[i]) = 0.4;
            // Source files
            source(m_MatmultKernels[i]) = "matrix_mult.cpp";
            source(tilerA[i]) = "matrix_mult_tiler.cpp";
            source(tilerB[i]) = "matrix_mult_tiler.cpp";
        }

        if
            constexpr(!isRedundantTilerOut && TP_ADD_DETILING_OUT) {
                untiler = kernel::create_object<DetilerClassOut>();
                connect<>(m_MatmultKernels[(TP_CASC_LEN - 1)].out[0], untiler.in[0]);
                dimensions(m_MatmultKernels[(TP_CASC_LEN - 1)].out[0]) = {dimAPerKernel * dimBPerKernel};
                dimensions(untiler.in[0]) = {dimAPerKernel * dimBPerKernel};
                connect<>(untiler.out[0], out);
                dimensions(untiler.out[0]) = {dimAPerKernel * dimBPerKernel};
            }
        else {
            connect<>(m_MatmultKernels[(TP_CASC_LEN - 1)].out[0], out);
            dimensions(m_MatmultKernels[(TP_CASC_LEN - 1)].out[0]) = {dimAPerKernel * dimBPerKernel};
        }
        runtime<ratio>(untiler) = 0.4;
        source(untiler) = "matrix_mult_untiler.cpp";
    }
};
}
}
}
}
}

#endif //_TEST_HARNESS_MATRIX_MULT_GRAPH_HPP_
