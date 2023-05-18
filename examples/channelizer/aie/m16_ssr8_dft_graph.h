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
#include "twiddle_m16_ssr8_dft.h"
#include "dft_1xN_widget_graph.h"

using namespace adf;

// ------------------------------------------------------------
// Top Level Graph
// ------------------------------------------------------------

class m16_ssr8_dft_graph : public graph {
  static constexpr unsigned NSAMP = 8192;
public:
  std::array<port<input>,8>  sig_i;
  std::array<port<output>,8> sig_o;

  using GRAPH = dft_1xN_widget_graph<NSAMP>;

  // DUT graph:
  GRAPH dut_graphA;
  GRAPH dut_graphB;
  GRAPH dut_graphC;
  GRAPH dut_graphD;

  // Constructor:
  m16_ssr8_dft_graph( void ) : dut_graphA(std::vector<std::vector<cint16>>{A_TWID0},std::vector<std::vector<cint16>>{A_TWID1}),
                               dut_graphB(std::vector<std::vector<cint16>>{B_TWID0},std::vector<std::vector<cint16>>{B_TWID1}),
                               dut_graphC(std::vector<std::vector<cint16>>{C_TWID0},std::vector<std::vector<cint16>>{C_TWID1}),
                               dut_graphD(std::vector<std::vector<cint16>>{D_TWID0},std::vector<std::vector<cint16>>{D_TWID1})
  {

    for (int ii=0; ii < 8; ii++) {
      connect<stream>( sig_i[ii],    dut_graphA.sig_i[ii] );
      connect<stream>( sig_i[ii],    dut_graphB.sig_i[ii] );
      connect<stream>( sig_i[ii],    dut_graphC.sig_i[ii] );
      connect<stream>( sig_i[ii],    dut_graphD.sig_i[ii] );
      if ( ii < 2 ) {
        connect<stream>( dut_graphA.sig_o[ii  ], sig_o[ii] );
      }
      else if ( ii < 4 ) {
        connect<stream>( dut_graphB.sig_o[ii-2], sig_o[ii] );
      }
      else if ( ii < 6 ) {
        connect<stream>( dut_graphC.sig_o[ii-4], sig_o[ii] );
      }
      else {
        connect<stream>( dut_graphD.sig_o[ii-6], sig_o[ii] );
      }
    } // ii
  }
};

