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

#include "dft16_compute.h"


// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------

template<class TT_DATA, class TT_TWIDDLE, class TT_ACC, unsigned NFFT>
dft16_compute<TT_DATA,TT_TWIDDLE,TT_ACC,NFFT>::dft16_compute( TT_TWIDDLE (&coeff0_i)[COEFF_DEPTH/2],
                                                              TT_TWIDDLE (&coeff1_i)[COEFF_DEPTH/2] )
  : coeff0(coeff0_i), coeff1(coeff1_i)
{
  aie::set_rounding(aie::rounding_mode::symmetric_inf);
  aie::set_saturation(aie::saturation_mode::saturate);
}

// ------------------------------------------------------------
// Run
// ------------------------------------------------------------

template<class TT_DATA, class TT_TWIDDLE, class TT_ACC, unsigned NFFT>
void dft16_compute<TT_DATA,TT_TWIDDLE,TT_ACC,NFFT>
::run( input_stream<TT_DATA>* restrict sig0_i,
       input_stream<TT_DATA>* restrict sig1_i,
       output_buffer<TT_DATA,extents<NSAMP_O> >& restrict sig_o )
{
  // Iterator to write outputs:
  auto itw = aie::begin_vector<4>(sig_o);

  // Iterator to access DFT coefficients:
  auto it0 = aie::begin_vector_circular<8,COEFF_DEPTH/2,aie_dm_resource::a>( coeff0 );
  auto it1 = aie::begin_vector_circular<8,COEFF_DEPTH/2,aie_dm_resource::b>( coeff1 );

  // Declare buffer for input signals [1x2]:
  aie::vector<TT_DATA,8> buff_sigA = aie::zeros<TT_DATA,8>();
  aie::vector<TT_DATA,8> buff_sigB = aie::zeros<TT_DATA,8>();

  // Declare buffer for coefficients [2x4];
  aie::vector<TT_TWIDDLE,16> buff_coeff = aie::zeros<TT_TWIDDLE,16>();

  // Declare accumulators:
  aie::accum<TT_ACC,4> accA0;

  // Loop over 16 samples (one transform):
  // --> We will run 7*9 = 63 transforms in total for a complete PFA
  for (unsigned rr=0; rr < NFFT; rr++)
    chess_loop_range(63,)
    chess_prepare_for_pipelining
  {
    // | ------- ss0 ----- | ------ ss1 ------ |
    // | d0 | d1 | d2 | d3 | d4 | d5 | d6 | d7 |
    buff_sigA.insert(0,aie::concat(readincr_v<4,aie_stream_resource_in::a>(sig0_i),
                                   readincr_v<4,aie_stream_resource_in::b>(sig1_i)));

    // Compute [1x2] x [2x4]:
    // buff_sig: d0 d1 d2 d3 d4 d5 d6 d7
    buff_coeff.insert(0,*it0++);
    buff_coeff.insert(1,*it1++);
    accA0 = mul4(      as_v16cint16(buff_coeff),0,0x3210,4,as_v8cint16(buff_sigA),0,0x0000,1);
    accA0 = mac4(accA0,as_v16cint16(buff_coeff),8,0x3210,4,as_v8cint16(buff_sigA),2,0x0000,1);
    buff_coeff.insert(0,*it0++);
    buff_coeff.insert(1,*it1++);
    accA0 = mac4(accA0,as_v16cint16(buff_coeff),0,0x3210,4,as_v8cint16(buff_sigA),4,0x0000,1);
    accA0 = mac4(accA0,as_v16cint16(buff_coeff),8,0x3210,4,as_v8cint16(buff_sigA),6,0x0000,1);

    // | ------- ss0 ----- | ------ ss1 ------ |
    // | d8 | d9 | d10| d11| d12| d13| d14| d15|
    buff_sigB.insert(0,aie::concat(readincr_v<4,aie_stream_resource_in::a>(sig0_i),
                                   readincr_v<4,aie_stream_resource_in::b>(sig1_i)));

    // Compute [1x2] x [2x4]:
    // buff_sig: d8 d9 d10 d11 d12 d13 d14 d15
    buff_coeff.insert(0,*it0++);
    buff_coeff.insert(1,*it1++);
    accA0 = mac4(accA0,as_v16cint16(buff_coeff),0,0x3210,4,as_v8cint16(buff_sigB),0,0x0000,1);
    accA0 = mac4(accA0,as_v16cint16(buff_coeff),8,0x3210,4,as_v8cint16(buff_sigB),2,0x0000,1);
    buff_coeff.insert(0,*it0++);
    buff_coeff.insert(1,*it1++);
    accA0 = mac4(accA0,as_v16cint16(buff_coeff),0,0x3210,4,as_v8cint16(buff_sigB),4,0x0000,1);
    accA0 = mac4(accA0,as_v16cint16(buff_coeff),8,0x3210,4,as_v8cint16(buff_sigB),6,0x0000,1);

    *itw++ = accA0.template to_vector<TT_DATA>( DNSHIFT );
  }
}


