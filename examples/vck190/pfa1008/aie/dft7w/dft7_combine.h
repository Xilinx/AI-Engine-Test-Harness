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

template<class TT_DATA, unsigned NFFT>
class dft7_combine {
public:
  static constexpr unsigned NSAMP_I = 4*NFFT; // # of samples delivered to combiner by each tile
  // Constructor:
  dft7_combine( void );

  // Run:
  void run(  input_buffer<TT_DATA,extents<NSAMP_I> >& __restrict sig0_i,
             input_buffer<TT_DATA,extents<NSAMP_I> >& __restrict sig1_i,
            output_stream<TT_DATA>* __restrict sig0_o,
            output_stream<TT_DATA>* __restrict sig1_o );

  static void registerKernelClass( void )
  {
    REGISTER_FUNCTION( dft7_combine::run );
  }
};

