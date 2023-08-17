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

#include <adf.h>
#include <aie_api/aie.hpp>

#include "dft16_combine.h"


// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------

template<class TT_DATA, unsigned NFFT>
dft16_combine<TT_DATA,NFFT>::dft16_combine( void )
{
  aie::set_rounding(aie::rounding_mode::symmetric_inf);
  aie::set_saturation(aie::saturation_mode::saturate);
}

// ------------------------------------------------------------
// Run
// ------------------------------------------------------------

template<class TT_DATA, unsigned NFFT>
void dft16_combine<TT_DATA,NFFT>
::run( input_buffer<TT_DATA,extents<NSAMP_I> >& restrict sig0_i,
       input_buffer<TT_DATA,extents<NSAMP_I> >& restrict sig1_i,
       input_buffer<TT_DATA,extents<NSAMP_I> >& restrict sig2_i,
       input_buffer<TT_DATA,extents<NSAMP_I> >& restrict sig3_i,
       output_stream<TT_DATA>* restrict sig0_o,
       output_stream<TT_DATA>* restrict sig1_o )
{
  auto it0 = aie::begin_vector<4,aie_dm_resource::a>( sig0_i );
  auto it1 = aie::begin_vector<4,aie_dm_resource::b>( sig1_i );
  auto it2 = aie::begin_vector<4,aie_dm_resource::b>( sig2_i );
  auto it3 = aie::begin_vector<4,aie_dm_resource::b>( sig3_i );

  // Loop over 16 samples
  // --> We will run 7*9 = 63 transforms in total for a complete PFA
  for (unsigned rr=0; rr < NFFT; rr++)
    chess_loop_range(63,)
    chess_prepare_for_pipelining
  {
    writeincr<aie_stream_resource_out::a>(sig0_o,*it0++);
    writeincr<aie_stream_resource_out::b>(sig1_o,*it1++);
    writeincr<aie_stream_resource_out::a>(sig0_o,*it2++);
    writeincr<aie_stream_resource_out::b>(sig1_o,*it3++);
  } // rr
}


