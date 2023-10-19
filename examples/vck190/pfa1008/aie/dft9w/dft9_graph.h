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
#include "dft9_compute.h"
#include "dft9_combine.h"
#include "dft9_twiddle.h"

using namespace adf;

// ------------------------------------------------------------
// Top Level Graph
// ------------------------------------------------------------

class dft9_graph : public graph {
public:
  static constexpr unsigned NFFT = 7*16;
  typedef cint16 TT_DATA;
  typedef cint16 TT_TWIDDLE;
  typedef cacc48 TT_ACC;

  // Kernels:
  kernel k_tile0;
  kernel k_tile1;
  kernel k_tile2;
  kernel k_tile3;

  std::array<port< input>,2> sig_i;
  std::array<port<output>,2> sig_o;

  using TT_COMPUTE = dft9_compute<TT_DATA,TT_TWIDDLE,TT_ACC,NFFT>;
  using TT_COMBINE = dft9_combine<TT_DATA,NFFT>;

  dft9_graph( void )
  {
    k_tile0 = kernel::create_object<TT_COMPUTE>( std::vector<TT_TWIDDLE>{DFT9_TWID0} );
    k_tile1 = kernel::create_object<TT_COMPUTE>( std::vector<TT_TWIDDLE>{DFT9_TWID1} );
    k_tile2 = kernel::create_object<TT_COMPUTE>( std::vector<TT_TWIDDLE>{DFT9_TWID2} );
    k_tile3 = kernel::create_object<TT_COMBINE>();
    source(k_tile0) = "dft9_compute.cpp";
    source(k_tile1) = "dft9_compute.cpp";
    source(k_tile2) = "dft9_compute.cpp";
    source(k_tile3) = "dft9_combine.cpp";
    runtime<ratio>(k_tile0) = 0.9;
    runtime<ratio>(k_tile1) = 0.9;
    runtime<ratio>(k_tile2) = 0.9;
    runtime<ratio>(k_tile3) = 0.9;

    // Connect stream I/O:
    connect<>(sig_i[0],k_tile0.in[0]);
    connect<>(sig_i[1],k_tile0.in[1]);
    connect<>(sig_i[0],k_tile1.in[0]);
    connect<>(sig_i[1],k_tile1.in[1]);
    connect<>(sig_i[0],k_tile2.in[0]);
    connect<>(sig_i[1],k_tile2.in[1]);

    // Connect to combiner tile:
    connect<>(k_tile0.out[0],k_tile3.in[0]);
    connect<>(k_tile1.out[0],k_tile3.in[1]);
    connect<>(k_tile2.out[0],k_tile3.in[2]);
    connect<>(k_tile3.out[0],sig_o[0]);
    connect<>(k_tile3.out[1],sig_o[1]);
  }
};

