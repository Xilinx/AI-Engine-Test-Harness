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
#include <aie_api/aie.hpp>

// ============================================================
// Input Tile
// ============================================================

// Performs two (1x2) x (2x4) OPS performed in as many cycles

template<class TT_DATA,class TT_COEFF,class TT_ACC,unsigned NSAMP>
class dft_1xN_input {
private:
  // Filter tap weights:
  alignas(16) TT_COEFF (&coeff0)[8];
  alignas(16) TT_COEFF (&coeff1)[8];

public:

  // Check parameters:
  static_assert( (NSAMP % 8) == 0, "NSAMP must be a multiple of 8");

  // Constructor:
  dft_1xN_input( TT_COEFF (&coeff0_i)[8], TT_COEFF (&coeff1_i)[8] );

  // Run:
  void run_input( input_stream<TT_DATA>* __restrict sig0_i,
                  input_stream<TT_DATA>* __restrict sig1_i,
                  output_cascade<TT_ACC>* __restrict acc_o );

  static void registerKernelClass( void )
  {
    REGISTER_FUNCTION( dft_1xN_input::run_input );
    REGISTER_PARAMETER( coeff0 );
    REGISTER_PARAMETER( coeff1 );
  }
};

// ============================================================
// Middle Tile
// ============================================================

// Performs two (1x2) x (2x4) OPS performed in as many cycles

template<class TT_DATA,class TT_COEFF,class TT_ACC,unsigned NSAMP>
class dft_1xN_middle {
private:
  // Filter tap weights:
  alignas(16) TT_COEFF (&coeff0)[8];
  alignas(16) TT_COEFF (&coeff1)[8];

public:
  // Constructor:
  dft_1xN_middle( TT_COEFF (&coeff0_i)[8], TT_COEFF (&coeff1_i)[8] );

  // Run:
  void run_middle( input_stream<TT_DATA>* __restrict sig0_i,
                   input_stream<TT_DATA>* __restrict sig1_i,
                   input_cascade<TT_ACC>*  __restrict acc_i,
                   output_cascade<TT_ACC>* __restrict acc_o );

  static void registerKernelClass( void )
  {
    REGISTER_FUNCTION( dft_1xN_middle::run_middle );
    REGISTER_PARAMETER( coeff0 );
    REGISTER_PARAMETER( coeff1 );
  }
};

// ============================================================
// Output Tile
// ============================================================

// Performs two (1x2) x (2x4) OPS performed in as many cycles

template<class TT_DATA,class TT_COEFF,class TT_ACC,unsigned NSAMP>
class dft_1xN_output {
private:
  static constexpr unsigned BIT_SHIFT = 15; // Assumes Q.15 x Q.15 (twiddles x data)
  // Filter tap weights:
  alignas(16) TT_COEFF (&coeff0)[8];
  alignas(16) TT_COEFF (&coeff1)[8];

public:
  // Constructor:
  dft_1xN_output( TT_COEFF (&coeff0_i)[8], TT_COEFF (&coeff1_i)[8] );

  // Run:
  void run_output( input_stream<TT_DATA>*  __restrict sig0_i,
                   input_stream<TT_DATA>*  __restrict sig1_i,
                   input_cascade<TT_ACC>*   __restrict acc_i,
                   output_stream<TT_DATA>* __restrict sig0_o,
                   output_stream<TT_DATA>* __restrict sig1_o );

  static void registerKernelClass( void )
  {
    REGISTER_FUNCTION( dft_1xN_output::run_output );
    REGISTER_PARAMETER( coeff0 );
    REGISTER_PARAMETER( coeff1 );
  }
};

