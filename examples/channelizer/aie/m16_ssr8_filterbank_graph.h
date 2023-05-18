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
#include "polyphase_fir_graph.h"
#include "m16_ssr8_filterbank_taps_init.h"

using namespace adf;

// ------------------------------------------------------------
// Top Level Graph
// ------------------------------------------------------------

class m16_ssr8_filterbank_graph : public graph {
public:
  static constexpr unsigned NSAMP = 8192;
// PLIO:
std::array<port<input>,8>  sig_i;
std::array<port<output>,8> sig_o;

  // Declare sub-graphs:
  polyphase_fir_graph<0,NSAMP> dut_graph0;
  polyphase_fir_graph<1,NSAMP> dut_graph1;
  polyphase_fir_graph<2,NSAMP> dut_graph2;
  polyphase_fir_graph<3,NSAMP> dut_graph3;
  polyphase_fir_graph<4,NSAMP> dut_graph4;
  polyphase_fir_graph<5,NSAMP> dut_graph5;
  polyphase_fir_graph<6,NSAMP> dut_graph6;
  polyphase_fir_graph<7,NSAMP> dut_graph7;

  // Constructor:
  m16_ssr8_filterbank_graph( void )
    : dut_graph0( tile0_tapsA, tile0_tapsB ),
      dut_graph1( tile1_tapsA, tile1_tapsB ),
      dut_graph2( tile2_tapsA, tile2_tapsB ),
      dut_graph3( tile3_tapsA, tile3_tapsB ),
      dut_graph4( tile4_tapsA, tile4_tapsB ),
      dut_graph5( tile5_tapsA, tile5_tapsB ),
      dut_graph6( tile6_tapsA, tile6_tapsB ),
      dut_graph7( tile7_tapsA, tile7_tapsB )
  {
    // Connections:
    connect<stream>( sig_i[0], dut_graph0.sig_i[0] );
    connect<stream>( sig_i[1], dut_graph1.sig_i[0] );
    connect<stream>( sig_i[2], dut_graph2.sig_i[0] );
    connect<stream>( sig_i[3], dut_graph3.sig_i[0] );
    connect<stream>( sig_i[4], dut_graph4.sig_i[0] );
    connect<stream>( sig_i[5], dut_graph5.sig_i[0] );
    connect<stream>( sig_i[6], dut_graph6.sig_i[0] );
    connect<stream>( sig_i[7], dut_graph7.sig_i[0] );

    connect<stream>( dut_graph0.sig_o[0], sig_o[0] );
    connect<stream>( dut_graph1.sig_o[0], sig_o[1] );
    connect<stream>( dut_graph2.sig_o[0], sig_o[2] );
    connect<stream>( dut_graph3.sig_o[0], sig_o[3] );
    connect<stream>( dut_graph4.sig_o[0], sig_o[4] );
    connect<stream>( dut_graph5.sig_o[0], sig_o[5] );
    connect<stream>( dut_graph6.sig_o[0], sig_o[6] );
    connect<stream>( dut_graph7.sig_o[0], sig_o[7] );
  }
};




