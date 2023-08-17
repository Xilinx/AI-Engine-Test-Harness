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

using namespace adf;

template<class TT_DATA, class TT_TWIDDLE, class TT_ACC, unsigned NFFT>
class dft7_compute {
public:
  static constexpr unsigned NSAMP_I = 7*NFFT/2; // # of samples on each input stream
  static constexpr unsigned NSAMP_O = 4*NFFT; // # of samples delivered to combiner from each tile
  static constexpr unsigned DNSHIFT = 15;
  static constexpr unsigned COEFF_DEPTH = 7*4;
private:
  // DFT coefficients:
  // --> Stores left half of twiddles 7 x 4, taken in row-major order
  alignas(16) TT_TWIDDLE (&coeff)[COEFF_DEPTH];
public:
  // Constructor:
  dft7_compute( TT_TWIDDLE (&coeff_i)[COEFF_DEPTH] );

  // Run:
  void run(  input_buffer<TT_DATA,extents<NSAMP_I> >& __restrict sig0_i,
             input_buffer<TT_DATA,extents<NSAMP_I> >& __restrict sig1_i,
             output_buffer<TT_DATA,extents<NSAMP_O> >& __restrict sig_o );

  static void registerKernelClass( void )
  {
    REGISTER_FUNCTION( dft7_compute::run );
    REGISTER_PARAMETER( coeff );
  }
};

