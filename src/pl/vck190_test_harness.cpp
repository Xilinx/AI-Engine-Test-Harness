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

// for vck190 performance mode
template <int NUM_CHANNELS, int URAM_DEPTH, int STREAM_WIDTH, int MEM_WIDTH>
void vck190_load_store_stream(test_harness<NUM_CHANNELS, URAM_DEPTH, STREAM_WIDTH, MEM_WIDTH>& test_harness,
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
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm16,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm17,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm18,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm19,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm20,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm21,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm22,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm23,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm24,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm25,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm26,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm27,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm28,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm29,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm30,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm31,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm32,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm33,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm34,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm35,
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
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm15,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm16,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm17,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm18,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm19,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm20,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm21,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm22,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm23,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm24,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm25,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm26,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm27,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm28,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm29,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm30,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm31,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm32,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm33,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm34,
                              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm35) {
#pragma HLS dataflow

    hls::stream<ap_uint<MEM_WIDTH> > mem_to_aie_streams[NUM_CHANNELS], mem_from_aie_streams[NUM_CHANNELS];

    test_harness.template load_memories<BURST_LENGTH_EACH_CHANNEL>(to_aie_mem, mem_to_aie_streams);
    test_harness.to_aie_ch[0].load_stream(mem_to_aie_streams[0], to_aie_strm0);
    test_harness.to_aie_ch[1].load_stream(mem_to_aie_streams[1], to_aie_strm1);
    test_harness.to_aie_ch[2].load_stream(mem_to_aie_streams[2], to_aie_strm2);
    test_harness.to_aie_ch[3].load_stream(mem_to_aie_streams[3], to_aie_strm3);
    test_harness.to_aie_ch[4].load_stream(mem_to_aie_streams[4], to_aie_strm4);
    test_harness.to_aie_ch[5].load_stream(mem_to_aie_streams[5], to_aie_strm5);
    test_harness.to_aie_ch[6].load_stream(mem_to_aie_streams[6], to_aie_strm6);
    test_harness.to_aie_ch[7].load_stream(mem_to_aie_streams[7], to_aie_strm7);
    test_harness.to_aie_ch[8].load_stream(mem_to_aie_streams[8], to_aie_strm8);
    test_harness.to_aie_ch[9].load_stream(mem_to_aie_streams[9], to_aie_strm9);
    test_harness.to_aie_ch[10].load_stream(mem_to_aie_streams[10], to_aie_strm10);
    test_harness.to_aie_ch[11].load_stream(mem_to_aie_streams[11], to_aie_strm11);
    test_harness.to_aie_ch[12].load_stream(mem_to_aie_streams[12], to_aie_strm12);
    test_harness.to_aie_ch[13].load_stream(mem_to_aie_streams[13], to_aie_strm13);
    test_harness.to_aie_ch[14].load_stream(mem_to_aie_streams[14], to_aie_strm14);
    test_harness.to_aie_ch[15].load_stream(mem_to_aie_streams[15], to_aie_strm15);
    test_harness.to_aie_ch[16].load_stream(mem_to_aie_streams[16], to_aie_strm16);
    test_harness.to_aie_ch[17].load_stream(mem_to_aie_streams[17], to_aie_strm17);
    test_harness.to_aie_ch[18].load_stream(mem_to_aie_streams[18], to_aie_strm18);
    test_harness.to_aie_ch[19].load_stream(mem_to_aie_streams[19], to_aie_strm19);
    test_harness.to_aie_ch[20].load_stream(mem_to_aie_streams[20], to_aie_strm20);
    test_harness.to_aie_ch[21].load_stream(mem_to_aie_streams[21], to_aie_strm21);
    test_harness.to_aie_ch[22].load_stream(mem_to_aie_streams[22], to_aie_strm22);
    test_harness.to_aie_ch[23].load_stream(mem_to_aie_streams[23], to_aie_strm23);
    test_harness.to_aie_ch[24].load_stream(mem_to_aie_streams[24], to_aie_strm24);
    test_harness.to_aie_ch[25].load_stream(mem_to_aie_streams[25], to_aie_strm25);
    test_harness.to_aie_ch[26].load_stream(mem_to_aie_streams[26], to_aie_strm26);
    test_harness.to_aie_ch[27].load_stream(mem_to_aie_streams[27], to_aie_strm27);
    test_harness.to_aie_ch[28].load_stream(mem_to_aie_streams[28], to_aie_strm28);
    test_harness.to_aie_ch[29].load_stream(mem_to_aie_streams[29], to_aie_strm29);
    test_harness.to_aie_ch[30].load_stream(mem_to_aie_streams[30], to_aie_strm30);
    test_harness.to_aie_ch[31].load_stream(mem_to_aie_streams[31], to_aie_strm31);
    test_harness.to_aie_ch[32].load_stream(mem_to_aie_streams[32], to_aie_strm32);
    test_harness.to_aie_ch[33].load_stream(mem_to_aie_streams[33], to_aie_strm33);
    test_harness.to_aie_ch[34].load_stream(mem_to_aie_streams[34], to_aie_strm34);
    test_harness.to_aie_ch[35].load_stream(mem_to_aie_streams[35], to_aie_strm35);

    test_harness.from_aie_ch[0].store_stream(mem_from_aie_streams[0], from_aie_strm0);
    test_harness.from_aie_ch[1].store_stream(mem_from_aie_streams[1], from_aie_strm1);
    test_harness.from_aie_ch[2].store_stream(mem_from_aie_streams[2], from_aie_strm2);
    test_harness.from_aie_ch[3].store_stream(mem_from_aie_streams[3], from_aie_strm3);
    test_harness.from_aie_ch[4].store_stream(mem_from_aie_streams[4], from_aie_strm4);
    test_harness.from_aie_ch[5].store_stream(mem_from_aie_streams[5], from_aie_strm5);
    test_harness.from_aie_ch[6].store_stream(mem_from_aie_streams[6], from_aie_strm6);
    test_harness.from_aie_ch[7].store_stream(mem_from_aie_streams[7], from_aie_strm7);
    test_harness.from_aie_ch[8].store_stream(mem_from_aie_streams[8], from_aie_strm8);
    test_harness.from_aie_ch[9].store_stream(mem_from_aie_streams[9], from_aie_strm9);
    test_harness.from_aie_ch[10].store_stream(mem_from_aie_streams[10], from_aie_strm10);
    test_harness.from_aie_ch[11].store_stream(mem_from_aie_streams[11], from_aie_strm11);
    test_harness.from_aie_ch[12].store_stream(mem_from_aie_streams[12], from_aie_strm12);
    test_harness.from_aie_ch[13].store_stream(mem_from_aie_streams[13], from_aie_strm13);
    test_harness.from_aie_ch[14].store_stream(mem_from_aie_streams[14], from_aie_strm14);
    test_harness.from_aie_ch[15].store_stream(mem_from_aie_streams[15], from_aie_strm15);
    test_harness.from_aie_ch[16].store_stream(mem_from_aie_streams[16], from_aie_strm16);
    test_harness.from_aie_ch[17].store_stream(mem_from_aie_streams[17], from_aie_strm17);
    test_harness.from_aie_ch[18].store_stream(mem_from_aie_streams[18], from_aie_strm18);
    test_harness.from_aie_ch[19].store_stream(mem_from_aie_streams[19], from_aie_strm19);
    test_harness.from_aie_ch[20].store_stream(mem_from_aie_streams[20], from_aie_strm20);
    test_harness.from_aie_ch[21].store_stream(mem_from_aie_streams[21], from_aie_strm21);
    test_harness.from_aie_ch[22].store_stream(mem_from_aie_streams[22], from_aie_strm22);
    test_harness.from_aie_ch[23].store_stream(mem_from_aie_streams[23], from_aie_strm23);
    test_harness.from_aie_ch[24].store_stream(mem_from_aie_streams[24], from_aie_strm24);
    test_harness.from_aie_ch[25].store_stream(mem_from_aie_streams[25], from_aie_strm25);
    test_harness.from_aie_ch[26].store_stream(mem_from_aie_streams[26], from_aie_strm26);
    test_harness.from_aie_ch[27].store_stream(mem_from_aie_streams[27], from_aie_strm27);
    test_harness.from_aie_ch[28].store_stream(mem_from_aie_streams[28], from_aie_strm28);
    test_harness.from_aie_ch[29].store_stream(mem_from_aie_streams[29], from_aie_strm29);
    test_harness.from_aie_ch[30].store_stream(mem_from_aie_streams[30], from_aie_strm30);
    test_harness.from_aie_ch[31].store_stream(mem_from_aie_streams[31], from_aie_strm31);
    test_harness.from_aie_ch[32].store_stream(mem_from_aie_streams[32], from_aie_strm32);
    test_harness.from_aie_ch[33].store_stream(mem_from_aie_streams[33], from_aie_strm33);
    test_harness.from_aie_ch[34].store_stream(mem_from_aie_streams[34], from_aie_strm34);
    test_harness.from_aie_ch[35].store_stream(mem_from_aie_streams[35], from_aie_strm35);
    test_harness.template store_memories<BURST_LENGTH_EACH_CHANNEL>(from_aie_mem, mem_from_aie_streams);
}

#define MEM_WIDTH (PARAM_MEM_WIDTH * 8)
#define STREAM_WIDTH (PARAM_WIDTH * 8)

extern "C" void vck190_test_harness(uint64_t* cfg,
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
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm16,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm17,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm18,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm19,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm20,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm21,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm22,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm23,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm24,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm25,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm26,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm27,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm28,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm29,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm30,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm31,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm32,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm33,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm34,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_aie_strm35,
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
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm15,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm16,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm17,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm18,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm19,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm20,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm21,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm22,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm23,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm24,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm25,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm26,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm27,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm28,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm29,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm30,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm31,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm32,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm33,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm34,
                                    hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_aie_strm35) {
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
#pragma HLS INTERFACE axis port = to_aie_strm16
#pragma HLS INTERFACE axis port = to_aie_strm17
#pragma HLS INTERFACE axis port = to_aie_strm18
#pragma HLS INTERFACE axis port = to_aie_strm19
#pragma HLS INTERFACE axis port = to_aie_strm20
#pragma HLS INTERFACE axis port = to_aie_strm21
#pragma HLS INTERFACE axis port = to_aie_strm22
#pragma HLS INTERFACE axis port = to_aie_strm23
#pragma HLS INTERFACE axis port = to_aie_strm24
#pragma HLS INTERFACE axis port = to_aie_strm25
#pragma HLS INTERFACE axis port = to_aie_strm26
#pragma HLS INTERFACE axis port = to_aie_strm27
#pragma HLS INTERFACE axis port = to_aie_strm28
#pragma HLS INTERFACE axis port = to_aie_strm29
#pragma HLS INTERFACE axis port = to_aie_strm30
#pragma HLS INTERFACE axis port = to_aie_strm31
#pragma HLS INTERFACE axis port = to_aie_strm32
#pragma HLS INTERFACE axis port = to_aie_strm33
#pragma HLS INTERFACE axis port = to_aie_strm34
#pragma HLS INTERFACE axis port = to_aie_strm35
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
#pragma HLS INTERFACE axis port = from_aie_strm16
#pragma HLS INTERFACE axis port = from_aie_strm17
#pragma HLS INTERFACE axis port = from_aie_strm18
#pragma HLS INTERFACE axis port = from_aie_strm19
#pragma HLS INTERFACE axis port = from_aie_strm20
#pragma HLS INTERFACE axis port = from_aie_strm21
#pragma HLS INTERFACE axis port = from_aie_strm22
#pragma HLS INTERFACE axis port = from_aie_strm23
#pragma HLS INTERFACE axis port = from_aie_strm24
#pragma HLS INTERFACE axis port = from_aie_strm25
#pragma HLS INTERFACE axis port = from_aie_strm26
#pragma HLS INTERFACE axis port = from_aie_strm27
#pragma HLS INTERFACE axis port = from_aie_strm28
#pragma HLS INTERFACE axis port = from_aie_strm29
#pragma HLS INTERFACE axis port = from_aie_strm30
#pragma HLS INTERFACE axis port = from_aie_strm31
#pragma HLS INTERFACE axis port = from_aie_strm32
#pragma HLS INTERFACE axis port = from_aie_strm33
#pragma HLS INTERFACE axis port = from_aie_strm34
#pragma HLS INTERFACE axis port = from_aie_strm35

    test_harness<PARAM_CHANNELS, PARAM_DEPTH, STREAM_WIDTH, MEM_WIDTH> inst;
    inst.load_cfg(cfg);
    inst.load_buffs(to_aie_mem);
    vck190_load_store_stream(
        inst, to_aie_mem, from_aie_mem, //
        to_aie_strm0, to_aie_strm1, to_aie_strm2, to_aie_strm3, to_aie_strm4, to_aie_strm5, to_aie_strm6, to_aie_strm7,
        to_aie_strm8, to_aie_strm9, to_aie_strm10, to_aie_strm11, to_aie_strm12, to_aie_strm13, to_aie_strm14,
        to_aie_strm15, to_aie_strm16, to_aie_strm17, to_aie_strm18, to_aie_strm19, to_aie_strm20, to_aie_strm21,
        to_aie_strm22, to_aie_strm23, to_aie_strm24, to_aie_strm25, to_aie_strm26, to_aie_strm27, to_aie_strm28,
        to_aie_strm29, to_aie_strm30, to_aie_strm31, to_aie_strm32, to_aie_strm33, to_aie_strm34, to_aie_strm35, //
        from_aie_strm0, from_aie_strm1, from_aie_strm2, from_aie_strm3, from_aie_strm4, from_aie_strm5, from_aie_strm6,
        from_aie_strm7, from_aie_strm8, from_aie_strm9, from_aie_strm10, from_aie_strm11, from_aie_strm12,
        from_aie_strm13, from_aie_strm14, from_aie_strm15, from_aie_strm16, from_aie_strm17, from_aie_strm18,
        from_aie_strm19, from_aie_strm20, from_aie_strm21, from_aie_strm22, from_aie_strm23, from_aie_strm24,
        from_aie_strm25, from_aie_strm26, from_aie_strm27, from_aie_strm28, from_aie_strm29, from_aie_strm30,
        from_aie_strm31, from_aie_strm32, from_aie_strm33, from_aie_strm34, from_aie_strm35);
    inst.store_buffs(from_aie_mem);
    inst.store_perf(perf);
}
