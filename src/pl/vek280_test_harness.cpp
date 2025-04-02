/*
 * MIT License
 *
 * Copyright (C) 2023-2025 Advanced Micro Devices, Inc.
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

#include <ap_int.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <hls_burst_maxi.h>
#include "test_harness.hpp"

#define BURST_LENGTH_EACH_CHANNEL 16

// For vek280 with 16 streams
template <int NUM_CHANNELS, int STREAM_WIDTH, int URAM_DEPTH, int MEM_WIDTH>
void vek280_load_store_stream(test_harness<NUM_CHANNELS, URAM_DEPTH, STREAM_WIDTH, MEM_WIDTH>& test_harness,
                              ap_uint<MEM_WIDTH>* to_aie_mem,
                              ap_uint<MEM_WIDTH>* from_aie_mem,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm0,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm1,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm2,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm3,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm4,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm5,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm6,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm7,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm8,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm9,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm10,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm11,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm12,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm13,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm14,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm15,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm0,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm1,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm2,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm3,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm4,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm5,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm6,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm7,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm8,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm9,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm10,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm11,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm12,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm13,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm14,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm15) {
#pragma HLS dataflow

    hls::stream<ap_uint<MEM_WIDTH> > to_streams[NUM_CHANNELS], from_streams[NUM_CHANNELS];

    test_harness.template load_memories<BURST_LENGTH_EACH_CHANNEL>(to_aie_mem, to_streams);
    test_harness.to_aie_ch[0].load_stream(to_streams[0], to_aie_strm0);
    test_harness.to_aie_ch[1].load_stream(to_streams[1], to_aie_strm1);
    test_harness.to_aie_ch[2].load_stream(to_streams[2], to_aie_strm2);
    test_harness.to_aie_ch[3].load_stream(to_streams[3], to_aie_strm3);
    test_harness.to_aie_ch[4].load_stream(to_streams[4], to_aie_strm4);
    test_harness.to_aie_ch[5].load_stream(to_streams[5], to_aie_strm5);
    test_harness.to_aie_ch[6].load_stream(to_streams[6], to_aie_strm6);
    test_harness.to_aie_ch[7].load_stream(to_streams[7], to_aie_strm7);
    test_harness.to_aie_ch[8].load_stream(to_streams[8], to_aie_strm8);
    test_harness.to_aie_ch[9].load_stream(to_streams[9], to_aie_strm9);
    test_harness.to_aie_ch[10].load_stream(to_streams[10], to_aie_strm10);
    test_harness.to_aie_ch[11].load_stream(to_streams[11], to_aie_strm11);
    test_harness.to_aie_ch[12].load_stream(to_streams[12], to_aie_strm12);
    test_harness.to_aie_ch[13].load_stream(to_streams[13], to_aie_strm13);
    test_harness.to_aie_ch[14].load_stream(to_streams[14], to_aie_strm14);
    test_harness.to_aie_ch[15].load_stream(to_streams[15], to_aie_strm15);
    test_harness.from_aie_ch[0].store_stream(from_streams[0], from_aie_strm0);
    test_harness.from_aie_ch[1].store_stream(from_streams[1], from_aie_strm1);
    test_harness.from_aie_ch[2].store_stream(from_streams[2], from_aie_strm2);
    test_harness.from_aie_ch[3].store_stream(from_streams[3], from_aie_strm3);
    test_harness.from_aie_ch[4].store_stream(from_streams[4], from_aie_strm4);
    test_harness.from_aie_ch[5].store_stream(from_streams[5], from_aie_strm5);
    test_harness.from_aie_ch[6].store_stream(from_streams[6], from_aie_strm6);
    test_harness.from_aie_ch[7].store_stream(from_streams[7], from_aie_strm7);
    test_harness.from_aie_ch[8].store_stream(from_streams[8], from_aie_strm8);
    test_harness.from_aie_ch[9].store_stream(from_streams[9], from_aie_strm9);
    test_harness.from_aie_ch[10].store_stream(from_streams[10], from_aie_strm10);
    test_harness.from_aie_ch[11].store_stream(from_streams[11], from_aie_strm11);
    test_harness.from_aie_ch[12].store_stream(from_streams[12], from_aie_strm12);
    test_harness.from_aie_ch[13].store_stream(from_streams[13], from_aie_strm13);
    test_harness.from_aie_ch[14].store_stream(from_streams[14], from_aie_strm14);
    test_harness.from_aie_ch[15].store_stream(from_streams[15], from_aie_strm15);
    test_harness.template store_memories<BURST_LENGTH_EACH_CHANNEL>(from_aie_mem, from_streams);
}

#define MEM_WIDTH (PARAM_MEM_WIDTH * 8)
#define STREAM_WIDTH (PARAM_WIDTH * 8)

extern "C" void vek280_test_harness(uint64_t* cfg,
                                    uint64_t* perf,
                                    ap_uint<MEM_WIDTH>* to_aie_mem,
                                    ap_uint<MEM_WIDTH>* from_aie_mem,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm0,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm1,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm2,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm3,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm4,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm5,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm6,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm7,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm8,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm9,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm10,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm11,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm12,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm13,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm14,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm15,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm0,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm1,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm2,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm3,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm4,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm5,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm6,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm7,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm8,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm9,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm10,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm11,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm12,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm13,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm14,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm15) {
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem port = cfg depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem port = perf depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem port = to_aie_mem depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem port = from_aie_mem depth = 4096

#pragma HLS INTERFACE s_axilite port = cfg bundle = control
#pragma HLS INTERFACE s_axilite port = perf bundle = control
#pragma HLS INTERFACE s_axilite port = to_aie_mem bundle = control
#pragma HLS INTERFACE s_axilite port = from_aie_mem bundle = control
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
#pragma HLS INTERFACE axis port = from_aie_strm13
#pragma HLS INTERFACE axis port = from_aie_strm13
#pragma HLS INTERFACE axis port = from_aie_strm14
#pragma HLS INTERFACE axis port = from_aie_strm15

    test_harness<PARAM_CHANNELS, PARAM_DEPTH, STREAM_WIDTH, MEM_WIDTH> inst;

    inst.load_cfg(cfg);
    inst.load_buffs(to_aie_mem);
    vek280_load_store_stream(inst, to_aie_mem, from_aie_mem, //
                             to_aie_strm0, to_aie_strm1, to_aie_strm2, to_aie_strm3, to_aie_strm4, to_aie_strm5,
                             to_aie_strm6, to_aie_strm7, to_aie_strm8, to_aie_strm9, to_aie_strm10, to_aie_strm11,
                             to_aie_strm12, to_aie_strm13, to_aie_strm14, to_aie_strm15, //
                             from_aie_strm0, from_aie_strm1,
                             from_aie_strm2, from_aie_strm3, from_aie_strm4, from_aie_strm5, from_aie_strm6,
                             from_aie_strm7, from_aie_strm8, from_aie_strm9, from_aie_strm10, from_aie_strm11,
                             from_aie_strm12, from_aie_strm13, from_aie_strm14, from_aie_strm15);
    inst.store_buffs(from_aie_mem);
    inst.store_perf(perf);
}
