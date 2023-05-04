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
#include "polyphase_fir.h"

using namespace adf;

// ------------------------------------------------------------
// Top Level Graph
// ------------------------------------------------------------

template<unsigned PATTERN,unsigned NSAMP>
class polyphase_fir_graph : public graph {
private:
  kernel k_fir;

public:
  std::array<port<input>,1>  sig_i;
  std::array<port<output>,1> sig_o;

  using TT_KERNEL = polyphase_fir<PATTERN,NSAMP>;

  // Constructor:
  polyphase_fir_graph( std::vector<typename TT_KERNEL::TT_COEFF> tapsA_i,
                       std::vector<typename TT_KERNEL::TT_COEFF> tapsB_i )
  {
    k_fir = kernel::create_object<TT_KERNEL>( tapsA_i, tapsB_i );

    source(k_fir) = "polyphase_fir.cpp";
    runtime<ratio>(k_fir) = 0.9;

    connect<stream,stream>( sig_i[0],     k_fir.in[0] );
    connect<stream,stream>( k_fir.out[0], sig_o[0]    );
  }
};

