/*
 * Copyright 2021 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ap_int.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <hls_burst_maxi.h>
#include "vck190_test_harness.hpp"
//#include "pragma_macro.hpp"

const int W = 128;
const int D = 8192;
const int CN = 16;
const int DL = 25;
const int TA = 128;
const int FA = 192;
const int TR = 2;
const int FR = 3;

extern "C" void vck190_test_harness(ap_uint<64>* cfg,
                                    ap_uint<64>* perf,
                                    ap_uint<128>* to_aie_data,
                                    ap_uint<128>* from_aie_data,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm0,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm1,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm2,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm3,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm4,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm5,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm6,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm7,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm8,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm9,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm10,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm11,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm12,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm13,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm14,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm15,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm0,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm1,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm2,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm3,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm4,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm5,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm6,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm7,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm8,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm9,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm10,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm11,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm12,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm13,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm14,
                                    hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm15) {
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0 port = cfg depth = 8192
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0 port = perf depth = 8192
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0 port = to_aie_data depth = 8192
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0 port = from_aie_data depth = 8192
#pragma HLS INTERFACE s_axilite port = cfg bundle = control
#pragma HLS INTERFACE s_axilite port = perf bundle = control
#pragma HLS INTERFACE s_axilite port = to_aie_data bundle = control
#pragma HLS INTERFACE s_axilite port = from_aie_data bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control
#pragma HLS INTERFACE axis port = to_aie_strm0
#pragma HLS INTERFACE axis port = to_aie_strm1
#pragma HLS INTERFACE axis port = to_aie_strm2
#pragma HLS INTERFACE axis port = to_aie_strm3
#pragma HLS INTERFACE axis port = to_aie_strm4
#pragma HLS INTERFACE axis port = to_aie_strm5
#pragma HLS INTERFACE axis port = to_aie_strm6
#pragma HLS INTERFACE axis port = to_aie_strm7
#pragma HLS INTERFACE axis port = to_aie_strm8
#pragma HLS INTERFACE axis port = to_aie_strm9
#pragma HLS INTERFACE axis port = to_aie_strm10
#pragma HLS INTERFACE axis port = to_aie_strm11
#pragma HLS INTERFACE axis port = to_aie_strm12
#pragma HLS INTERFACE axis port = to_aie_strm13
#pragma HLS INTERFACE axis port = to_aie_strm14
#pragma HLS INTERFACE axis port = to_aie_strm15
#pragma HLS INTERFACE axis port = from_aie_strm0
#pragma HLS INTERFACE axis port = from_aie_strm1
#pragma HLS INTERFACE axis port = from_aie_strm2
#pragma HLS INTERFACE axis port = from_aie_strm3
#pragma HLS INTERFACE axis port = from_aie_strm4
#pragma HLS INTERFACE axis port = from_aie_strm5
#pragma HLS INTERFACE axis port = from_aie_strm6
#pragma HLS INTERFACE axis port = from_aie_strm7
#pragma HLS INTERFACE axis port = from_aie_strm8
#pragma HLS INTERFACE axis port = from_aie_strm9
#pragma HLS INTERFACE axis port = from_aie_strm10
#pragma HLS INTERFACE axis port = from_aie_strm11
#pragma HLS INTERFACE axis port = from_aie_strm12
#pragma HLS INTERFACE axis port = from_aie_strm13
#pragma HLS INTERFACE axis port = from_aie_strm14
#pragma HLS INTERFACE axis port = from_aie_strm15

    test_harness<W, D, CN> inst;
    inst.load_cfg(cfg);
    inst.load_buff(to_aie_data);
    inst.load_store_stream(to_aie_strm0, to_aie_strm1, to_aie_strm2, to_aie_strm3, to_aie_strm4, to_aie_strm5,
                           to_aie_strm6, to_aie_strm7, to_aie_strm8, to_aie_strm9, to_aie_strm10, to_aie_strm11,
                           to_aie_strm12, to_aie_strm13, to_aie_strm14, to_aie_strm15, from_aie_strm0, from_aie_strm1,
                           from_aie_strm2, from_aie_strm3, from_aie_strm4, from_aie_strm5, from_aie_strm6,
                           from_aie_strm7, from_aie_strm8, from_aie_strm9, from_aie_strm10, from_aie_strm11,
                           from_aie_strm12, from_aie_strm13, from_aie_strm14, from_aie_strm15);
    // inst.load_store_stream(to_aie_strm0, from_aie_strm0);

    inst.store_buff(from_aie_data);
    inst.store_perf(perf);
}
