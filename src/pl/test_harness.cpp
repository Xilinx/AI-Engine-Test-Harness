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

#include <ap_int.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <hls_burst_maxi.h>
#include "test_harness.hpp"

extern "C" void vck190_test_harness_func(ap_uint<64>* cfg,
                                         ap_uint<64>* perf,
                                         ap_uint<128>* to_aie_data0,
                                         ap_uint<128>* to_aie_data1,
                                         ap_uint<128>* to_aie_data2,
                                         ap_uint<128>* to_aie_data3,
                                         ap_uint<128>* to_aie_data4,
                                         ap_uint<128>* to_aie_data5,
                                         ap_uint<128>* from_aie_data0,
                                         ap_uint<128>* from_aie_data1,
                                         ap_uint<128>* from_aie_data2,
                                         ap_uint<128>* from_aie_data3,
                                         ap_uint<128>* from_aie_data4,
                                         ap_uint<128>* from_aie_data5,
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
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm16,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm17,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm18,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm19,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm20,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm21,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm22,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm23,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm24,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm25,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm26,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm27,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm28,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm29,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm30,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm31,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm32,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm33,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm34,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm35,
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
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm15,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm16,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm17,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm18,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm19,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm20,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm21,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm22,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm23,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm24,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm25,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm26,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm27,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm28,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm29,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm30,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm31,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm32,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm33,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm34,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm35) {
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0 port = cfg depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0 port = perf depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0 port = to_aie_data0 depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem1 port = to_aie_data1 depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem2 port = to_aie_data2 depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem3 port = to_aie_data3 depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem4 port = to_aie_data4 depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem5 port = to_aie_data5 depth = 4096

#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0 port = from_aie_data0 depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem1 port = from_aie_data1 depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem2 port = from_aie_data2 depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem3 port = from_aie_data3 depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem4 port = from_aie_data4 depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem5 port = from_aie_data5 depth = 4096

#pragma HLS INTERFACE s_axilite port = cfg bundle = control
#pragma HLS INTERFACE s_axilite port = perf bundle = control
#pragma HLS INTERFACE s_axilite port = to_aie_data0 bundle = control
#pragma HLS INTERFACE s_axilite port = to_aie_data1 bundle = control
#pragma HLS INTERFACE s_axilite port = to_aie_data2 bundle = control
#pragma HLS INTERFACE s_axilite port = to_aie_data3 bundle = control
#pragma HLS INTERFACE s_axilite port = to_aie_data4 bundle = control
#pragma HLS INTERFACE s_axilite port = to_aie_data5 bundle = control
#pragma HLS INTERFACE s_axilite port = from_aie_data0 bundle = control
#pragma HLS INTERFACE s_axilite port = from_aie_data1 bundle = control
#pragma HLS INTERFACE s_axilite port = from_aie_data2 bundle = control
#pragma HLS INTERFACE s_axilite port = from_aie_data3 bundle = control
#pragma HLS INTERFACE s_axilite port = from_aie_data4 bundle = control
#pragma HLS INTERFACE s_axilite port = from_aie_data5 bundle = control
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

    test_harness<128, 4096, 36> inst;
    inst.load_cfg(cfg);
    inst.load_store_balance(
        to_aie_data0, to_aie_data1, to_aie_data2, to_aie_data3, to_aie_data4, to_aie_data5, //
        to_aie_strm0, to_aie_strm1, to_aie_strm2, to_aie_strm3, to_aie_strm4, to_aie_strm5, to_aie_strm6, to_aie_strm7,
        to_aie_strm8, to_aie_strm9, to_aie_strm10, to_aie_strm11, to_aie_strm12, to_aie_strm13, to_aie_strm14,
        to_aie_strm15, to_aie_strm16, to_aie_strm17, to_aie_strm18, to_aie_strm19, to_aie_strm20, to_aie_strm21,
        to_aie_strm22, to_aie_strm23, to_aie_strm24, to_aie_strm25, to_aie_strm26, to_aie_strm27, to_aie_strm28,
        to_aie_strm29, to_aie_strm30, to_aie_strm31, to_aie_strm32, to_aie_strm33, to_aie_strm34,
        to_aie_strm35, //
        from_aie_data0, from_aie_data1, from_aie_data2, from_aie_data3, from_aie_data4, from_aie_data5, //
        from_aie_strm0, from_aie_strm1, from_aie_strm2, from_aie_strm3, from_aie_strm4, from_aie_strm5, from_aie_strm6,
        from_aie_strm7, from_aie_strm8, from_aie_strm9, from_aie_strm10, from_aie_strm11, from_aie_strm12,
        from_aie_strm13, from_aie_strm14, from_aie_strm15, from_aie_strm16, from_aie_strm17, from_aie_strm18,
        from_aie_strm19, from_aie_strm20, from_aie_strm21, from_aie_strm22, from_aie_strm23, from_aie_strm24,
        from_aie_strm25, from_aie_strm26, from_aie_strm27, from_aie_strm28, from_aie_strm29, from_aie_strm30,
        from_aie_strm31, from_aie_strm32, from_aie_strm33, from_aie_strm34, from_aie_strm35);
}

extern "C" void vck190_test_harness_perf(ap_uint<64>* cfg,
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
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm16,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm17,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm18,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm19,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm20,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm21,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm22,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm23,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm24,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm25,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm26,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm27,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm28,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm29,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm30,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm31,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm32,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm33,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm34,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& to_aie_strm35,
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
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm15,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm16,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm17,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm18,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm19,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm20,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm21,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm22,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm23,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm24,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm25,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm26,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm27,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm28,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm29,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm30,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm31,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm32,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm33,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm34,
                                         hls::stream<ap_axiu<128, 0, 0, 0> >& from_aie_strm35) {
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0 port = cfg depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0 port = perf depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem0 port = to_aie_data depth = 4096
#pragma HLS INTERFACE m_axi offset = slave latency = 64 num_write_outstanding = 16 num_read_outstanding = \
    16 max_write_burst_length = 64 max_read_burst_length = 64 bundle = gmem1 port = from_aie_data depth = 4096

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

    test_harness<128, 4096, 36> inst;
    inst.load_cfg(cfg);
    inst.load_buff(to_aie_data);
    inst.load_store_stream(
        to_aie_strm0, to_aie_strm1, to_aie_strm2, to_aie_strm3, to_aie_strm4, to_aie_strm5, to_aie_strm6, to_aie_strm7,
        to_aie_strm8, to_aie_strm9, to_aie_strm10, to_aie_strm11, to_aie_strm12, to_aie_strm13, to_aie_strm14,
        to_aie_strm15, to_aie_strm16, to_aie_strm17, to_aie_strm18, to_aie_strm19, to_aie_strm20, to_aie_strm21,
        to_aie_strm22, to_aie_strm23, to_aie_strm24, to_aie_strm25, to_aie_strm26, to_aie_strm27, to_aie_strm28,
        to_aie_strm29, to_aie_strm30, to_aie_strm31, to_aie_strm32, to_aie_strm33, to_aie_strm34,
        to_aie_strm35, //
        from_aie_strm0, from_aie_strm1, from_aie_strm2, from_aie_strm3, from_aie_strm4, from_aie_strm5, from_aie_strm6,
        from_aie_strm7, from_aie_strm8, from_aie_strm9, from_aie_strm10, from_aie_strm11, from_aie_strm12,
        from_aie_strm13, from_aie_strm14, from_aie_strm15, from_aie_strm16, from_aie_strm17, from_aie_strm18,
        from_aie_strm19, from_aie_strm20, from_aie_strm21, from_aie_strm22, from_aie_strm23, from_aie_strm24,
        from_aie_strm25, from_aie_strm26, from_aie_strm27, from_aie_strm28, from_aie_strm29, from_aie_strm30,
        from_aie_strm31, from_aie_strm32, from_aie_strm33, from_aie_strm34, from_aie_strm35);
    inst.store_buff(from_aie_data);
    inst.store_perf(perf);
}

extern "C" void vek280_test_harness(ap_uint<64>* cfg,
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

    test_harness<128, 8192, 16> inst;
    inst.load_cfg(cfg);
    inst.load_buff(to_aie_data);
    inst.load_store_stream(to_aie_strm0, to_aie_strm1, to_aie_strm2, to_aie_strm3, to_aie_strm4, to_aie_strm5,
                           to_aie_strm6, to_aie_strm7, to_aie_strm8, to_aie_strm9, to_aie_strm10, to_aie_strm11,
                           to_aie_strm12, to_aie_strm13, to_aie_strm14, to_aie_strm15, from_aie_strm0, from_aie_strm1,
                           from_aie_strm2, from_aie_strm3, from_aie_strm4, from_aie_strm5, from_aie_strm6,
                           from_aie_strm7, from_aie_strm8, from_aie_strm9, from_aie_strm10, from_aie_strm11,
                           from_aie_strm12, from_aie_strm13, from_aie_strm14, from_aie_strm15);
    inst.store_buff(from_aie_data);
    inst.store_perf(perf);
}
