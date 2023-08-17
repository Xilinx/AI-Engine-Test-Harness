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

#ifndef _TEST_HARNESS_HPP_
#define _TEST_HARNESS_HPP_

#include <ap_int.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <hls_burst_maxi.h>
#ifndef __SYNTHESIS__
#include <iostream>
#endif

/**
 * @brief Testing mode, including functional testing mode, performance testing mode, and replay testing mode.
 *
 */
enum TestMode { FUNC_MODE = 0, PERF_MODE, REP_MODE };

/**
 * @brief buffer for test harness
 *
 * @tparam W Bit width of AXI-M, AXI-Stream and URAM buffer width. 128 default.
 * @tparam D Depth of URAM buffer for input/output.
 */
template <int W, int D>
class buff_channel {
   public:
#ifndef __SYNTHESIS__
    ap_uint<W>* buff;
#else
    ap_uint<W> buff[D];
#endif

    buff_channel() {
#ifndef __SYNTHESIS__
        buff = (ap_uint<W>*)malloc(sizeof(ap_uint<W>) * D);
#else
#pragma HLS inline
#pragma HLS bind_storage variable = buff type = RAM_1P impl = URAM
#endif
    }

    ~buff_channel() {
#ifndef __SYNTHESIS__
        free(buff);
#endif
    }

    void load_buff(ap_uint<W>* source, int size_in_frame) {
        if (size_in_frame > D) {
            for (int i = 0; i < D; i++) {
#pragma HLS pipeline II = 1
                buff[i] = source[i];
            }
        } else {
            for (int i = 0; i < size_in_frame; i++) {
#pragma HLS pipeline II = 1
                buff[i] = source[i];
            }
        }
    }

    void load_stream(ap_uint<64> mode,
                     hls::stream<ap_axiu<W, 0, 0, 0> >& to_strm,
                     ap_uint<64> delay,
                     int frames,
                     int rep,
                     ap_uint<64>& end) {
#pragma HLS dataflow
        hls::stream<bool> trigger_strm;
#pragma HLS stream variable = trigger_strm depth = 4
        feed(mode, to_strm, delay, frames, rep, trigger_strm);
        counter(trigger_strm, end);
    }

    void load_stream(
        hls::stream<ap_axiu<W, 0, 0, 0> >& to_strm, ap_uint<64> delay, int frames, int rep, ap_uint<64>& end) {
#pragma HLS dataflow
        hls::stream<bool> trigger_strm;
#pragma HLS stream variable = trigger_strm depth = 4
        feed(to_strm, delay, frames, rep, trigger_strm);
        counter(trigger_strm, end);
    }

    void store_buff(ap_uint<W>* dst, int size_in_frame) {
        for (int i = 0; i < size_in_frame; i++) {
#pragma HLS pipeline II = 1
            dst[i] = buff[i];
        }
    }

    void store_stream(ap_uint<64> mode,
                      hls::stream<ap_axiu<W, 0, 0, 0> >& from_strm,
                      ap_uint<64> delay,
                      int frames,
                      int rep,
                      ap_uint<64>& end) {
#pragma HLS dataflow
        hls::stream<bool> trigger_strm;
#pragma HLS stream variable = trigger_strm depth = 4
        get(mode, from_strm, delay, frames, rep, trigger_strm);
        counter(trigger_strm, end);
    }

    void store_stream(
        hls::stream<ap_axiu<W, 0, 0, 0> >& from_strm, ap_uint<64> delay, int frames, int rep, ap_uint<64>& end) {
#pragma HLS dataflow
        hls::stream<bool> trigger_strm;
#pragma HLS stream variable = trigger_strm depth = 4
        get(from_strm, delay, frames, rep, trigger_strm);
        counter(trigger_strm, end);
    }

   private:
    void counter(hls::stream<bool>& trigger_strm, ap_uint<64>& end) {
        bool succ_start = false;
        bool succ_end = false;
        ap_uint<64> counter = 0;
        while (!succ_start) {
#pragma HLS pipeline II = 1
            counter++;
            bool tmp;
            succ_start = trigger_strm.read_nb(tmp);
        }
        while (!succ_end) {
#pragma HLS pipeline II = 1
            counter++;
            bool tmp;
            succ_end = trigger_strm.read_nb(tmp);
            if (succ_end) {
                end = counter;
            }
        }
    }

    void feed(ap_uint<64> mode,
              hls::stream<ap_axiu<W, 0, 0, 0> >& to_strm,
              ap_uint<64> delay,
              int frames,
              int rep,
              hls::stream<bool>& trigger_strm) {
        // delay and send one trigger
        if (delay != 0) {
            while (delay != 0) {
#pragma HLS pipeline II = 1
                delay--;
                if (delay == 0) {
                    trigger_strm.write(true);
                }
            }
        } else {
            trigger_strm.write(true);
        }

        // push data to AXI stream
        ap_uint<W> tmp_val;
        ap_axiu<W, 0, 0, 0> tmp;
        tmp.keep = -1;
        tmp.last = 0;
        switch (mode) {
            case PERF_MODE:
                for (int i = 0; i < frames; i++) {
#pragma HLS pipeline II = 1
                    if (i < D) {
                        tmp.data = buff[i];
                    } else {
                        tmp_val = buff[i % frames];
                        tmp.data.range(15, 0) = (tmp_val.range(15, 0) << 1) % ap_uint<32>(0x0001);
                        tmp.data.range(31, 16) = (tmp_val.range(31, 16) << 2) % ap_uint<32>(0x0203);
                        tmp.data.range(47, 32) = (tmp_val.range(47, 32) << 3) % ap_uint<32>(0x0405);
                        tmp.data.range(63, 48) = (tmp_val.range(63, 48) << 4) % ap_uint<32>(0x0607);
                        tmp.data.range(79, 64) = (tmp_val.range(79, 64) << 5) % ap_uint<32>(0x0809);
                        tmp.data.range(95, 80) = (tmp_val.range(95, 80) << 6) % ap_uint<32>(0x0a0b);
                        tmp.data.range(111, 96) = (tmp_val.range(111, 96) << 7) % ap_uint<32>(0x0c0d);
                        tmp.data.range(127, 112) = (tmp_val.range(127, 112) << 8) % ap_uint<32>(0x0e0f);
                    }
                    to_strm.write(tmp);
                }
                break;
            case REP_MODE:
                for (int j = 0; j < rep; j++) {
                    for (int i = 0; i < frames; i++) {
#pragma HLS pipeline II = 1
                        ap_axiu<W, 0, 0, 0> tmp;
                        tmp.data = buff[i];
                        tmp.keep = -1;
                        tmp.last = 0;
                        to_strm.write(tmp);
                    }
                }
                break;
            default:
#ifndef __SYNTHESIS__
                std::cerr << "[ERROR]: Only PERF_MODE and REP_MODE use URAM\n";
                exit(1);
#endif
                break;
        }

        // send last trigger
        trigger_strm.write(true);
    }

    void feed(hls::stream<ap_axiu<W, 0, 0, 0> >& to_strm,
              ap_uint<64> delay,
              int frames,
              int rep,
              hls::stream<bool>& trigger_strm) {
        // delay and send one trigger
        if (delay != 0) {
            while (delay != 0) {
#pragma HLS pipeline II = 1
                delay--;
                if (delay == 0) {
                    trigger_strm.write(true);
                }
            }
        } else {
            trigger_strm.write(true);
        }

        // push data to AXI stream
        for (int j = 0; j < rep; j++) {
            for (int i = 0; i < frames; i++) {
#pragma HLS pipeline II = 1
                ap_axiu<W, 0, 0, 0> tmp;
                tmp.data = buff[i];
                tmp.keep = -1;
                tmp.last = 0;
                to_strm.write(tmp);
            }
        }

        // send last trigger
        trigger_strm.write(true);
    }

    void get(ap_uint<64> mode,
             hls::stream<ap_axiu<W, 0, 0, 0> >& from_strm,
             ap_uint<64> delay,
             int frames,
             int rep,
             hls::stream<bool>& trigger_strm) {
        // delay and send one trigger
        if (delay != 0) {
            while (delay != 0) {
#pragma HLS pipeline II = 1
                delay--;
                if (delay == 0) {
                    trigger_strm.write(true);
                }
            }
        } else {
            trigger_strm.write(true);
        }

        // pull data from AXI stream
        switch (mode) {
            case PERF_MODE:
                for (int i = 0; i < frames; i++) {
#pragma HLS pipeline II = 1
                    ap_axiu<W, 0, 0, 0> tmp = from_strm.read();
                    buff[i % D] = tmp.data;
                }
                break;
            case REP_MODE:
                for (int j = 0; j < rep; j++) {
                    for (int i = 0; i < frames; i++) {
#pragma HLS pipeline II = 1
                        ap_axiu<W, 0, 0, 0> tmp = from_strm.read();
                        buff[i] = tmp.data;
                    }
                }
                break;
            default:
#ifndef __SYNTHESIS__
                std::cerr << "[ERROR]: Only PERF_MODE and REP_MODE use URAM\n";
                exit(1);
#endif
                break;
        }

        // send last trigger
        trigger_strm.write(true);
    }

    void get(hls::stream<ap_axiu<W, 0, 0, 0> >& from_strm,
             ap_uint<64> delay,
             int frames,
             int rep,
             hls::stream<bool>& trigger_strm) {
        // delay and send one trigger
        if (delay != 0) {
            while (delay != 0) {
#pragma HLS pipeline II = 1
                delay--;
                if (delay == 0) {
                    trigger_strm.write(true);
                }
            }
        } else {
            trigger_strm.write(true);
        }

        // pull data from AXI stream
        for (int j = 0; j < rep; j++) {
            for (int i = 0; i < frames; i++) {
#pragma HLS pipeline II = 1
                ap_axiu<W, 0, 0, 0> tmp = from_strm.read();
                buff[i] = tmp.data;
            }
        }

        // send last trigger
        trigger_strm.write(true);
    }
};

/**
 * @brief test harness
 *
 * @tparam W Bit width of AXI-M, AXI-Stream and URAM buffer width. 128 default.
 * @tparam D Depth of URAM buffer for input/output.
 * @tparam N Number of channel to feed AIE and get from AIE. Should be 1, 2, 4, 8, 16
 */
template <int W, int D, int N>
class test_harness {
   public:
    buff_channel<W, D> to_aie_ch[N];
    buff_channel<W, D> from_aie_ch[N];
    ap_uint<64> to_aie_delay[N];
    ap_uint<64> to_aie_last[N];
    ap_uint<64> to_aie_frame[N];
    ap_uint<64> to_aie_rep[N];
    ap_uint<64> to_aie_offt[N];
    ap_uint<64> from_aie_delay[N];
    ap_uint<64> from_aie_last[N];
    ap_uint<64> from_aie_frame[N];
    ap_uint<64> from_aie_rep[N];
    ap_uint<64> from_aie_offt[N];
    ap_uint<64> test_mode[N];

    test_harness() {
#pragma HLS inline
#pragma HLS array_partition variable = to_aie_ch type = complete dim = 1
#pragma HLS array_partition variable = from_aie_ch type = complete dim = 1
#pragma HLS array_partition variable = to_aie_delay type = complete dim = 1
#pragma HLS array_partition variable = to_aie_last type = complete dim = 1
#pragma HLS array_partition variable = to_aie_frame type = complete dim = 1
#pragma HLS array_partition variable = to_aie_rep type = complete dim = 1
#pragma HLS array_partition variable = to_aie_offt type = complete dim = 1
#pragma HLS array_partition variable = from_aie_delay type = complete dim = 1
#pragma HLS array_partition variable = from_aie_last type = complete dim = 1
#pragma HLS array_partition variable = from_aie_frame type = complete dim = 1
#pragma HLS array_partition variable = from_aie_rep type = complete dim = 1
#pragma HLS array_partition variable = from_aie_offt type = complete dim = 1
#pragma HLS array_partition variable = test_mode type = complete dim = 1
    }

    void load_cfg(ap_uint<64>* cfg) {
        for (int i = 0; i < N; i++) {
            test_mode[i] = cfg[0];
        }

        for (int i = 1; i <= N; i++) {
            to_aie_delay[i - 1] = cfg[i];
        }

        for (int i = 1; i <= N; i++) {
            to_aie_frame[i - 1] = cfg[i + N];
        }

        for (int i = 1; i <= N; i++) {
            to_aie_rep[i - 1] = cfg[i + N * 2];
        }

        for (int i = 1; i <= N; i++) {
            to_aie_offt[i - 1] = cfg[i + N * 3];
        }

        for (int i = 1; i <= N; i++) {
            from_aie_delay[i - 1] = cfg[i + N * 4];
        }

        for (int i = 1; i <= N; i++) {
            from_aie_frame[i - 1] = cfg[i + N * 5];
        }

        for (int i = 1; i <= N; i++) {
            from_aie_rep[i - 1] = cfg[i + N * 6];
        }

        for (int i = 1; i <= N; i++) {
            from_aie_offt[i - 1] = cfg[i + N * 7];
        }
    }

    void store_perf(ap_uint<64>* perf) {
        for (int i = 0; i < N; i++) {
            perf[i] = to_aie_last[i];
        }

        for (int i = 0; i < N; i++) {
            perf[i + N] = from_aie_last[i];
        }
    }

    void load_buff(ap_uint<W>* data) {
        for (int i = 0; i < N; i++) {
            to_aie_ch[i].load_buff(data + i * D, to_aie_frame[i]);
        }
    }

    void store_buff(ap_uint<W>* data) {
        for (int i = 0; i < N; i++) {
            from_aie_ch[i].store_buff(data + i * D, from_aie_frame[i]);
        }
    }

    void load_store_stream(hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm0,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm1,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm2,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm3,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm4,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm5,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm6,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm7,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm8,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm9,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm10,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm11,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm12,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm13,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm14,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm15,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm16,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm17,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm18,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm19,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm20,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm21,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm22,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm23,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm24,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm25,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm26,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm27,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm28,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm29,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm30,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm31,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm32,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm33,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm34,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm35,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm0,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm1,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm2,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm3,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm4,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm5,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm6,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm7,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm8,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm9,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm10,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm11,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm12,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm13,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm14,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm15,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm16,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm17,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm18,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm19,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm20,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm21,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm22,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm23,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm24,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm25,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm26,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm27,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm28,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm29,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm30,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm31,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm32,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm33,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm34,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm35) {
#pragma HLS dataflow

        to_aie_ch[0].load_stream(test_mode[0], to_aie_strm0, to_aie_delay[0], to_aie_frame[0], to_aie_rep[0],
                                 to_aie_last[0]);
        to_aie_ch[1].load_stream(test_mode[1], to_aie_strm1, to_aie_delay[1], to_aie_frame[1], to_aie_rep[1],
                                 to_aie_last[1]);
        to_aie_ch[2].load_stream(test_mode[2], to_aie_strm2, to_aie_delay[2], to_aie_frame[2], to_aie_rep[2],
                                 to_aie_last[2]);
        to_aie_ch[3].load_stream(test_mode[3], to_aie_strm3, to_aie_delay[3], to_aie_frame[3], to_aie_rep[3],
                                 to_aie_last[3]);
        to_aie_ch[4].load_stream(test_mode[4], to_aie_strm4, to_aie_delay[4], to_aie_frame[4], to_aie_rep[4],
                                 to_aie_last[4]);
        to_aie_ch[5].load_stream(test_mode[5], to_aie_strm5, to_aie_delay[5], to_aie_frame[5], to_aie_rep[5],
                                 to_aie_last[5]);
        to_aie_ch[6].load_stream(test_mode[6], to_aie_strm6, to_aie_delay[6], to_aie_frame[6], to_aie_rep[6],
                                 to_aie_last[6]);
        to_aie_ch[7].load_stream(test_mode[7], to_aie_strm7, to_aie_delay[7], to_aie_frame[7], to_aie_rep[7],
                                 to_aie_last[7]);
        to_aie_ch[8].load_stream(test_mode[8], to_aie_strm8, to_aie_delay[8], to_aie_frame[8], to_aie_rep[8],
                                 to_aie_last[8]);
        to_aie_ch[9].load_stream(test_mode[9], to_aie_strm9, to_aie_delay[9], to_aie_frame[9], to_aie_rep[9],
                                 to_aie_last[9]);
        to_aie_ch[10].load_stream(test_mode[10], to_aie_strm10, to_aie_delay[10], to_aie_frame[10], to_aie_rep[10],
                                  to_aie_last[10]);
        to_aie_ch[11].load_stream(test_mode[11], to_aie_strm11, to_aie_delay[11], to_aie_frame[11], to_aie_rep[11],
                                  to_aie_last[11]);
        to_aie_ch[12].load_stream(test_mode[12], to_aie_strm12, to_aie_delay[12], to_aie_frame[12], to_aie_rep[12],
                                  to_aie_last[12]);
        to_aie_ch[13].load_stream(test_mode[13], to_aie_strm13, to_aie_delay[13], to_aie_frame[13], to_aie_rep[13],
                                  to_aie_last[13]);
        to_aie_ch[14].load_stream(test_mode[14], to_aie_strm14, to_aie_delay[14], to_aie_frame[14], to_aie_rep[14],
                                  to_aie_last[14]);
        to_aie_ch[15].load_stream(test_mode[15], to_aie_strm15, to_aie_delay[15], to_aie_frame[15], to_aie_rep[15],
                                  to_aie_last[15]);
        to_aie_ch[16].load_stream(test_mode[16], to_aie_strm16, to_aie_delay[16], to_aie_frame[16], to_aie_rep[16],
                                  to_aie_last[16]);
        to_aie_ch[17].load_stream(test_mode[17], to_aie_strm17, to_aie_delay[17], to_aie_frame[17], to_aie_rep[17],
                                  to_aie_last[17]);
        to_aie_ch[18].load_stream(test_mode[18], to_aie_strm18, to_aie_delay[18], to_aie_frame[18], to_aie_rep[18],
                                  to_aie_last[18]);
        to_aie_ch[19].load_stream(test_mode[19], to_aie_strm19, to_aie_delay[19], to_aie_frame[19], to_aie_rep[19],
                                  to_aie_last[19]);
        to_aie_ch[20].load_stream(test_mode[20], to_aie_strm20, to_aie_delay[20], to_aie_frame[20], to_aie_rep[20],
                                  to_aie_last[20]);
        to_aie_ch[21].load_stream(test_mode[21], to_aie_strm21, to_aie_delay[21], to_aie_frame[21], to_aie_rep[21],
                                  to_aie_last[21]);
        to_aie_ch[22].load_stream(test_mode[22], to_aie_strm22, to_aie_delay[22], to_aie_frame[22], to_aie_rep[22],
                                  to_aie_last[22]);
        to_aie_ch[23].load_stream(test_mode[23], to_aie_strm23, to_aie_delay[23], to_aie_frame[23], to_aie_rep[23],
                                  to_aie_last[23]);
        to_aie_ch[24].load_stream(test_mode[24], to_aie_strm24, to_aie_delay[24], to_aie_frame[24], to_aie_rep[24],
                                  to_aie_last[24]);
        to_aie_ch[25].load_stream(test_mode[25], to_aie_strm25, to_aie_delay[25], to_aie_frame[25], to_aie_rep[25],
                                  to_aie_last[25]);
        to_aie_ch[26].load_stream(test_mode[26], to_aie_strm26, to_aie_delay[26], to_aie_frame[26], to_aie_rep[26],
                                  to_aie_last[26]);
        to_aie_ch[27].load_stream(test_mode[27], to_aie_strm27, to_aie_delay[27], to_aie_frame[27], to_aie_rep[27],
                                  to_aie_last[27]);
        to_aie_ch[28].load_stream(test_mode[28], to_aie_strm28, to_aie_delay[28], to_aie_frame[28], to_aie_rep[28],
                                  to_aie_last[28]);
        to_aie_ch[29].load_stream(test_mode[29], to_aie_strm29, to_aie_delay[29], to_aie_frame[29], to_aie_rep[29],
                                  to_aie_last[29]);
        to_aie_ch[30].load_stream(test_mode[30], to_aie_strm30, to_aie_delay[30], to_aie_frame[30], to_aie_rep[30],
                                  to_aie_last[30]);
        to_aie_ch[31].load_stream(test_mode[31], to_aie_strm31, to_aie_delay[31], to_aie_frame[31], to_aie_rep[31],
                                  to_aie_last[31]);
        to_aie_ch[32].load_stream(test_mode[32], to_aie_strm32, to_aie_delay[32], to_aie_frame[32], to_aie_rep[32],
                                  to_aie_last[32]);
        to_aie_ch[33].load_stream(test_mode[33], to_aie_strm33, to_aie_delay[33], to_aie_frame[33], to_aie_rep[33],
                                  to_aie_last[33]);
        to_aie_ch[34].load_stream(test_mode[34], to_aie_strm34, to_aie_delay[34], to_aie_frame[34], to_aie_rep[34],
                                  to_aie_last[34]);
        to_aie_ch[35].load_stream(test_mode[35], to_aie_strm35, to_aie_delay[35], to_aie_frame[35], to_aie_rep[35],
                                  to_aie_last[35]);

        from_aie_ch[0].store_stream(test_mode[0], from_aie_strm0, from_aie_delay[0], from_aie_frame[0], from_aie_rep[0],
                                    from_aie_last[0]);
        from_aie_ch[1].store_stream(test_mode[1], from_aie_strm1, from_aie_delay[1], from_aie_frame[1], from_aie_rep[1],
                                    from_aie_last[1]);
        from_aie_ch[2].store_stream(test_mode[2], from_aie_strm2, from_aie_delay[2], from_aie_frame[2], from_aie_rep[2],
                                    from_aie_last[2]);
        from_aie_ch[3].store_stream(test_mode[3], from_aie_strm3, from_aie_delay[3], from_aie_frame[3], from_aie_rep[3],
                                    from_aie_last[3]);
        from_aie_ch[4].store_stream(test_mode[4], from_aie_strm4, from_aie_delay[4], from_aie_frame[4], from_aie_rep[4],
                                    from_aie_last[4]);
        from_aie_ch[5].store_stream(test_mode[5], from_aie_strm5, from_aie_delay[5], from_aie_frame[5], from_aie_rep[5],
                                    from_aie_last[5]);
        from_aie_ch[6].store_stream(test_mode[6], from_aie_strm6, from_aie_delay[6], from_aie_frame[6], from_aie_rep[6],
                                    from_aie_last[6]);
        from_aie_ch[7].store_stream(test_mode[7], from_aie_strm7, from_aie_delay[7], from_aie_frame[7], from_aie_rep[7],
                                    from_aie_last[7]);
        from_aie_ch[8].store_stream(test_mode[8], from_aie_strm8, from_aie_delay[8], from_aie_frame[8], from_aie_rep[8],
                                    from_aie_last[8]);
        from_aie_ch[9].store_stream(test_mode[9], from_aie_strm9, from_aie_delay[9], from_aie_frame[9], from_aie_rep[9],
                                    from_aie_last[9]);
        from_aie_ch[10].store_stream(test_mode[10], from_aie_strm10, from_aie_delay[10], from_aie_frame[10],
                                     from_aie_rep[10], from_aie_last[10]);
        from_aie_ch[11].store_stream(test_mode[11], from_aie_strm11, from_aie_delay[11], from_aie_frame[11],
                                     from_aie_rep[11], from_aie_last[11]);
        from_aie_ch[12].store_stream(test_mode[12], from_aie_strm12, from_aie_delay[12], from_aie_frame[12],
                                     from_aie_rep[12], from_aie_last[12]);
        from_aie_ch[13].store_stream(test_mode[13], from_aie_strm13, from_aie_delay[13], from_aie_frame[13],
                                     from_aie_rep[13], from_aie_last[13]);
        from_aie_ch[14].store_stream(test_mode[14], from_aie_strm14, from_aie_delay[14], from_aie_frame[14],
                                     from_aie_rep[14], from_aie_last[14]);
        from_aie_ch[15].store_stream(test_mode[15], from_aie_strm15, from_aie_delay[15], from_aie_frame[15],
                                     from_aie_rep[15], from_aie_last[15]);
        from_aie_ch[16].store_stream(test_mode[16], from_aie_strm16, from_aie_delay[16], from_aie_frame[16],
                                     from_aie_rep[16], from_aie_last[16]);
        from_aie_ch[17].store_stream(test_mode[17], from_aie_strm17, from_aie_delay[17], from_aie_frame[17],
                                     from_aie_rep[17], from_aie_last[17]);
        from_aie_ch[18].store_stream(test_mode[18], from_aie_strm18, from_aie_delay[18], from_aie_frame[18],
                                     from_aie_rep[18], from_aie_last[18]);
        from_aie_ch[19].store_stream(test_mode[19], from_aie_strm19, from_aie_delay[19], from_aie_frame[19],
                                     from_aie_rep[19], from_aie_last[19]);
        from_aie_ch[20].store_stream(test_mode[20], from_aie_strm20, from_aie_delay[20], from_aie_frame[20],
                                     from_aie_rep[20], from_aie_last[20]);
        from_aie_ch[21].store_stream(test_mode[21], from_aie_strm21, from_aie_delay[21], from_aie_frame[21],
                                     from_aie_rep[21], from_aie_last[21]);
        from_aie_ch[22].store_stream(test_mode[22], from_aie_strm22, from_aie_delay[22], from_aie_frame[22],
                                     from_aie_rep[22], from_aie_last[22]);
        from_aie_ch[23].store_stream(test_mode[23], from_aie_strm23, from_aie_delay[23], from_aie_frame[23],
                                     from_aie_rep[23], from_aie_last[23]);
        from_aie_ch[24].store_stream(test_mode[24], from_aie_strm24, from_aie_delay[24], from_aie_frame[24],
                                     from_aie_rep[24], from_aie_last[24]);
        from_aie_ch[25].store_stream(test_mode[25], from_aie_strm25, from_aie_delay[25], from_aie_frame[25],
                                     from_aie_rep[25], from_aie_last[25]);
        from_aie_ch[26].store_stream(test_mode[26], from_aie_strm26, from_aie_delay[26], from_aie_frame[26],
                                     from_aie_rep[26], from_aie_last[26]);
        from_aie_ch[27].store_stream(test_mode[27], from_aie_strm27, from_aie_delay[27], from_aie_frame[27],
                                     from_aie_rep[27], from_aie_last[27]);
        from_aie_ch[28].store_stream(test_mode[28], from_aie_strm28, from_aie_delay[28], from_aie_frame[28],
                                     from_aie_rep[28], from_aie_last[28]);
        from_aie_ch[29].store_stream(test_mode[29], from_aie_strm29, from_aie_delay[29], from_aie_frame[29],
                                     from_aie_rep[29], from_aie_last[29]);
        from_aie_ch[30].store_stream(test_mode[30], from_aie_strm30, from_aie_delay[30], from_aie_frame[30],
                                     from_aie_rep[30], from_aie_last[30]);
        from_aie_ch[31].store_stream(test_mode[31], from_aie_strm31, from_aie_delay[31], from_aie_frame[31],
                                     from_aie_rep[31], from_aie_last[31]);
        from_aie_ch[32].store_stream(test_mode[32], from_aie_strm32, from_aie_delay[32], from_aie_frame[32],
                                     from_aie_rep[32], from_aie_last[32]);
        from_aie_ch[33].store_stream(test_mode[33], from_aie_strm33, from_aie_delay[33], from_aie_frame[33],
                                     from_aie_rep[33], from_aie_last[33]);
        from_aie_ch[34].store_stream(test_mode[34], from_aie_strm34, from_aie_delay[34], from_aie_frame[34],
                                     from_aie_rep[34], from_aie_last[34]);
        from_aie_ch[35].store_stream(test_mode[35], from_aie_strm35, from_aie_delay[35], from_aie_frame[35],
                                     from_aie_rep[35], from_aie_last[35]);
    }

    template <int grp_idx, int grp_num>
    void smart_load(ap_uint<W>* data,
                    hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm0,
                    hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm1,
                    hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm2,
                    hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm3,
                    hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm4,
                    hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm5) {
#ifndef __SYNTHESIS__
        std::cout << "to_aie_frame[" << grp_num * grp_idx + 0 << "] = " << to_aie_frame[grp_num * grp_idx + 0] << std::endl;
        std::cout << "to_aie_frame[" << grp_num * grp_idx + 1 << "] = " << to_aie_frame[grp_num * grp_idx + 1] << std::endl;
        std::cout << "to_aie_frame[" << grp_num * grp_idx + 2 << "] = " << to_aie_frame[grp_num * grp_idx + 2] << std::endl;
        std::cout << "to_aie_frame[" << grp_num * grp_idx + 3 << "] = " << to_aie_frame[grp_num * grp_idx + 3] << std::endl;
        std::cout << "to_aie_frame[" << grp_num * grp_idx + 4 << "] = " << to_aie_frame[grp_num * grp_idx + 4] << std::endl;
        std::cout << "to_aie_frame[" << grp_num * grp_idx + 5 << "] = " << to_aie_frame[grp_num * grp_idx + 5] << std::endl;
        std::cout << "to_aie_offt[" << grp_num * grp_idx + 0 << "] = " << to_aie_offt[grp_num * grp_idx + 0] << std::endl;
        std::cout << "to_aie_offt[" << grp_num * grp_idx + 1 << "] = " << to_aie_offt[grp_num * grp_idx + 1] << std::endl;
        std::cout << "to_aie_offt[" << grp_num * grp_idx + 2 << "] = " << to_aie_offt[grp_num * grp_idx + 2] << std::endl;
        std::cout << "to_aie_offt[" << grp_num * grp_idx + 3 << "] = " << to_aie_offt[grp_num * grp_idx + 3] << std::endl;
        std::cout << "to_aie_offt[" << grp_num * grp_idx + 4 << "] = " << to_aie_offt[grp_num * grp_idx + 4] << std::endl;
        std::cout << "to_aie_offt[" << grp_num * grp_idx + 5 << "] = " << to_aie_offt[grp_num * grp_idx + 5] << std::endl;
#endif
        // final end status
        ap_uint<grp_num> last;
        for (int i = 0; i < grp_num; i++) {
#pragma HLS unroll
            last[i] = 1;
        }
        // end label of each channel
        ap_uint<grp_num> end = 0;
        // counter of each channel
        ap_uint<64> cnt[grp_num];
#pragma HLS array_partition variable = cnt type = complete dim = 1
        // channel index
        ap_uint<8> idx = 0;

        // initialize counter for each channel
        // clear latency counter as functional testing mode do not support performance profiling
        for (int ch = 0; ch < grp_num; ch++) {
#pragma HLS unroll
            cnt[ch] = 0;
            to_aie_last[grp_num * grp_idx + ch] = 0;
        }

        while (last != end) {
#pragma HLS pipeline II = 1
            ap_uint<64> addr = to_aie_offt[grp_num * grp_idx + idx] + cnt[idx];
            ap_axiu<W, 0, 0, 0> tmp;
            tmp.data = data[addr];
            tmp.keep = -1;
            tmp.last = 0;
            // label the channel with end once required number of data pushed
            if (cnt[idx] == to_aie_frame[grp_num * grp_idx + idx]) {
                end[idx] = 1;
            }
            // not ending
            if (!end[idx]) {
                // try non-blocking write to this channel
                switch (idx) {
                    case 0:
                        if (!to_aie_strm0.full()) {
                            to_aie_strm0.write(tmp);
                            cnt[0]++;
                        }
                        break;
                    case 1:
                        if (!to_aie_strm1.full()) {
                            to_aie_strm1.write(tmp);
                            cnt[1]++;
                        }
                        break;
                    case 2:
                        if (!to_aie_strm2.full()) {
                            to_aie_strm2.write(tmp);
                            cnt[2]++;
                        }
                        break;
                    case 3:
                        if (!to_aie_strm3.full()) {
                            to_aie_strm3.write(tmp);
                            cnt[3]++;
                        }
                        break;
                    case 4:
                        if (!to_aie_strm4.full()) {
                            to_aie_strm4.write(tmp);
                            cnt[4]++;
                        }
                        break;
                    case 5:
                        if (!to_aie_strm5.full()) {
                            to_aie_strm5.write(tmp);
                            cnt[5]++;
                        }
                        break;
                    default:
                        break;
                }
            }
            // switch to the next channel in a round-robin manner
            if (idx == grp_num - 1) {
                idx = 0;
            } else {
                idx++;
            }
        }
    }

    template <int grp_idx, int grp_num>
    void smart_store(ap_uint<W>* data,
                     hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm0,
                     hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm1,
                     hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm2,
                     hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm3,
                     hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm4,
                     hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm5) {
#ifndef __SYNTHESIS__
        std::cout << "from_aie_frame[" << grp_num * grp_idx + 0 << "] = " << from_aie_frame[grp_num * grp_idx + 0] << std::endl;
        std::cout << "from_aie_frame[" << grp_num * grp_idx + 1 << "] = " << from_aie_frame[grp_num * grp_idx + 1] << std::endl;
        std::cout << "from_aie_frame[" << grp_num * grp_idx + 2 << "] = " << from_aie_frame[grp_num * grp_idx + 2] << std::endl;
        std::cout << "from_aie_frame[" << grp_num * grp_idx + 3 << "] = " << from_aie_frame[grp_num * grp_idx + 3] << std::endl;
        std::cout << "from_aie_frame[" << grp_num * grp_idx + 4 << "] = " << from_aie_frame[grp_num * grp_idx + 4] << std::endl;
        std::cout << "from_aie_frame[" << grp_num * grp_idx + 5 << "] = " << from_aie_frame[grp_num * grp_idx + 5] << std::endl;
        std::cout << "from_aie_offt[" << grp_num * grp_idx + 0 << "] = " << from_aie_offt[grp_num * grp_idx + 0] << std::endl;
        std::cout << "from_aie_offt[" << grp_num * grp_idx + 1 << "] = " << from_aie_offt[grp_num * grp_idx + 1] << std::endl;
        std::cout << "from_aie_offt[" << grp_num * grp_idx + 2 << "] = " << from_aie_offt[grp_num * grp_idx + 2] << std::endl;
        std::cout << "from_aie_offt[" << grp_num * grp_idx + 3 << "] = " << from_aie_offt[grp_num * grp_idx + 3] << std::endl;
        std::cout << "from_aie_offt[" << grp_num * grp_idx + 4 << "] = " << from_aie_offt[grp_num * grp_idx + 4] << std::endl;
        std::cout << "from_aie_offt[" << grp_num * grp_idx + 5 << "] = " << from_aie_offt[grp_num * grp_idx + 5] << std::endl;
#endif
        // final end status
        ap_uint<grp_num> last;
        for (int i = 0; i < grp_num; i++) {
#pragma HLS unroll
            last[i] = 1;
        }
        // end label of each channel
        ap_uint<grp_num> end = 0;
        // counter of each channel
        ap_uint<64> cnt[grp_num];
#pragma HLS array_partition variable = cnt type = complete dim = 1
        // channel index
        ap_uint<8> idx = 0;

        // initialize counter for each channel
        // clear latency counter as functional testing mode do not support performance profiling
        for (int ch = 0; ch < grp_num; ch++) {
#pragma HLS unroll
            cnt[ch] = 0;
            from_aie_last[grp_num * grp_idx + ch] = 0;
        }

        while (last != end) {
#pragma HLS pipeline II = 1
            ap_uint<64> addr = from_aie_offt[grp_num * grp_idx + idx] + cnt[idx];
            // label the channel with end once required number of data pushed
            if (cnt[idx] == from_aie_frame[grp_num * grp_idx + idx]) {
                end[idx] = 1;
            }
            // not ending
            if (!end[idx]) {
                // try non-blocking write to this channel
                ap_axiu<W, 0, 0, 0> tmp;
                switch (idx) {
                    case 0:
                        if (!from_aie_strm0.empty()) {
                            tmp = from_aie_strm0.read();
                            data[addr] = tmp.data;
                            cnt[0]++;
                        }
                        break;
                    case 1:
                        if (!from_aie_strm1.empty()) {
                            tmp = from_aie_strm1.read();
                            data[addr] = tmp.data;
                            cnt[1]++;
                        }
                        break;
                    case 2:
                        if (!from_aie_strm2.empty()) {
                            tmp = from_aie_strm2.read();
                            data[addr] = tmp.data;
                            cnt[2]++;
                        }
                        break;
                    case 3:
                        if (!from_aie_strm3.empty()) {
                            tmp = from_aie_strm3.read();
                            data[addr] = tmp.data;
                            cnt[3]++;
                        }
                        break;
                    case 4:
                        if (!from_aie_strm4.empty()) {
                            tmp = from_aie_strm4.read();
                            data[addr] = tmp.data;
                            cnt[4]++;
                        }
                        break;
                    case 5:
                        if (!from_aie_strm5.empty()) {
                            tmp = from_aie_strm5.read();
                            data[addr] = tmp.data;
                            cnt[5]++;
                        }
                        break;
                    default:
                        break;
                }
            }
            // switch to the next channel in a round-robin manner
            if (idx == grp_num - 1) {
                idx = 0;
            } else {
                idx++;
            }
        }
    }

    void load_store_balance(ap_uint<W>* to_aie_data0,
                            ap_uint<W>* to_aie_data1,
                            ap_uint<W>* to_aie_data2,
                            ap_uint<W>* to_aie_data3,
                            ap_uint<W>* to_aie_data4,
                            ap_uint<W>* to_aie_data5,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm0,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm1,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm2,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm3,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm4,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm5,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm6,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm7,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm8,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm9,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm10,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm11,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm12,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm13,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm14,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm15,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm16,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm17,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm18,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm19,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm20,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm21,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm22,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm23,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm24,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm25,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm26,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm27,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm28,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm29,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm30,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm31,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm32,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm33,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm34,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm35,
                            ap_uint<W>* from_aie_data0,
                            ap_uint<W>* from_aie_data1,
                            ap_uint<W>* from_aie_data2,
                            ap_uint<W>* from_aie_data3,
                            ap_uint<W>* from_aie_data4,
                            ap_uint<W>* from_aie_data5,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm0,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm1,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm2,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm3,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm4,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm5,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm6,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm7,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm8,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm9,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm10,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm11,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm12,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm13,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm14,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm15,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm16,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm17,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm18,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm19,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm20,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm21,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm22,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm23,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm24,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm25,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm26,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm27,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm28,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm29,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm30,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm31,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm32,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm33,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm34,
                            hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm35) {
#pragma HLS dataflow
        smart_load<0, 6>(to_aie_data0, //
                         to_aie_strm0, to_aie_strm1, to_aie_strm2, to_aie_strm3, to_aie_strm4, to_aie_strm5);
        smart_load<1, 6>(to_aie_data1, //
                         to_aie_strm6, to_aie_strm7, to_aie_strm8, to_aie_strm9, to_aie_strm10, to_aie_strm11);
        smart_load<2, 6>(to_aie_data2, //
                         to_aie_strm12, to_aie_strm13, to_aie_strm14, to_aie_strm15, to_aie_strm16, to_aie_strm17);
        smart_load<3, 6>(to_aie_data3, //
                         to_aie_strm18, to_aie_strm19, to_aie_strm20, to_aie_strm21, to_aie_strm22, to_aie_strm23);
        smart_load<4, 6>(to_aie_data4, //
                         to_aie_strm24, to_aie_strm25, to_aie_strm26, to_aie_strm27, to_aie_strm28, to_aie_strm29);
        smart_load<5, 6>(to_aie_data5, //
                         to_aie_strm30, to_aie_strm31, to_aie_strm32, to_aie_strm33, to_aie_strm34, to_aie_strm35);

        smart_store<0, 6>(from_aie_data0, //
                          from_aie_strm0, from_aie_strm1, from_aie_strm2, from_aie_strm3, from_aie_strm4,
                          from_aie_strm5);
        smart_store<1, 6>(from_aie_data1, //
                          from_aie_strm6, from_aie_strm7, from_aie_strm8, from_aie_strm9, from_aie_strm10,
                          from_aie_strm11);
        smart_store<2, 6>(from_aie_data2, //
                          from_aie_strm12, from_aie_strm13, from_aie_strm14, from_aie_strm15, from_aie_strm16,
                          from_aie_strm17);
        smart_store<3, 6>(from_aie_data3, //
                          from_aie_strm18, from_aie_strm19, from_aie_strm20, from_aie_strm21, from_aie_strm22,
                          from_aie_strm23);
        smart_store<4, 6>(from_aie_data4, //
                          from_aie_strm24, from_aie_strm25, from_aie_strm26, from_aie_strm27, from_aie_strm28,
                          from_aie_strm29);
        smart_store<5, 6>(from_aie_data5, //
                          from_aie_strm30, from_aie_strm31, from_aie_strm32, from_aie_strm33, from_aie_strm34,
                          from_aie_strm35);
    }

    void load_store_stream(hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm0,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm1,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm2,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm3,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm4,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm5,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm6,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm7,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm8,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm9,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm10,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm11,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm12,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm13,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm14,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& to_aie_strm15,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm0,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm1,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm2,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm3,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm4,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm5,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm6,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm7,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm8,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm9,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm10,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm11,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm12,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm13,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm14,
                           hls::stream<ap_axiu<W, 0, 0, 0> >& from_aie_strm15) {
#pragma HLS dataflow
        to_aie_ch[0].load_stream(to_aie_strm0, to_aie_delay[0], to_aie_frame[0], to_aie_rep[0], to_aie_last[0]);
        to_aie_ch[1].load_stream(to_aie_strm1, to_aie_delay[1], to_aie_frame[1], to_aie_rep[1], to_aie_last[1]);
        to_aie_ch[2].load_stream(to_aie_strm2, to_aie_delay[2], to_aie_frame[2], to_aie_rep[2], to_aie_last[2]);
        to_aie_ch[3].load_stream(to_aie_strm3, to_aie_delay[3], to_aie_frame[3], to_aie_rep[3], to_aie_last[3]);
        to_aie_ch[4].load_stream(to_aie_strm4, to_aie_delay[4], to_aie_frame[4], to_aie_rep[4], to_aie_last[4]);
        to_aie_ch[5].load_stream(to_aie_strm5, to_aie_delay[5], to_aie_frame[5], to_aie_rep[5], to_aie_last[5]);
        to_aie_ch[6].load_stream(to_aie_strm6, to_aie_delay[6], to_aie_frame[6], to_aie_rep[6], to_aie_last[6]);
        to_aie_ch[7].load_stream(to_aie_strm7, to_aie_delay[7], to_aie_frame[7], to_aie_rep[7], to_aie_last[7]);
        to_aie_ch[8].load_stream(to_aie_strm8, to_aie_delay[8], to_aie_frame[8], to_aie_rep[8], to_aie_last[8]);
        to_aie_ch[9].load_stream(to_aie_strm9, to_aie_delay[9], to_aie_frame[9], to_aie_rep[9], to_aie_last[9]);
        to_aie_ch[10].load_stream(to_aie_strm10, to_aie_delay[10], to_aie_frame[10], to_aie_rep[10], to_aie_last[10]);
        to_aie_ch[11].load_stream(to_aie_strm11, to_aie_delay[11], to_aie_frame[11], to_aie_rep[11], to_aie_last[11]);
        to_aie_ch[12].load_stream(to_aie_strm12, to_aie_delay[12], to_aie_frame[12], to_aie_rep[12], to_aie_last[12]);
        to_aie_ch[13].load_stream(to_aie_strm13, to_aie_delay[13], to_aie_frame[13], to_aie_rep[13], to_aie_last[13]);
        to_aie_ch[14].load_stream(to_aie_strm14, to_aie_delay[14], to_aie_frame[14], to_aie_rep[14], to_aie_last[14]);
        to_aie_ch[15].load_stream(to_aie_strm15, to_aie_delay[15], to_aie_frame[15], to_aie_rep[15], to_aie_last[15]);

        from_aie_ch[0].store_stream(from_aie_strm0, from_aie_delay[0], from_aie_frame[0], from_aie_rep[0],
                                    from_aie_last[0]);
        from_aie_ch[1].store_stream(from_aie_strm1, from_aie_delay[1], from_aie_frame[1], from_aie_rep[1],
                                    from_aie_last[1]);
        from_aie_ch[2].store_stream(from_aie_strm2, from_aie_delay[2], from_aie_frame[2], from_aie_rep[2],
                                    from_aie_last[2]);
        from_aie_ch[3].store_stream(from_aie_strm3, from_aie_delay[3], from_aie_frame[3], from_aie_rep[3],
                                    from_aie_last[3]);
        from_aie_ch[4].store_stream(from_aie_strm4, from_aie_delay[4], from_aie_frame[4], from_aie_rep[4],
                                    from_aie_last[4]);
        from_aie_ch[5].store_stream(from_aie_strm5, from_aie_delay[5], from_aie_frame[5], from_aie_rep[5],
                                    from_aie_last[5]);
        from_aie_ch[6].store_stream(from_aie_strm6, from_aie_delay[6], from_aie_frame[6], from_aie_rep[6],
                                    from_aie_last[6]);
        from_aie_ch[7].store_stream(from_aie_strm7, from_aie_delay[7], from_aie_frame[7], from_aie_rep[7],
                                    from_aie_last[7]);
        from_aie_ch[8].store_stream(from_aie_strm8, from_aie_delay[8], from_aie_frame[8], from_aie_rep[8],
                                    from_aie_last[8]);
        from_aie_ch[9].store_stream(from_aie_strm9, from_aie_delay[9], from_aie_frame[9], from_aie_rep[9],
                                    from_aie_last[9]);
        from_aie_ch[10].store_stream(from_aie_strm10, from_aie_delay[10], from_aie_frame[10], from_aie_rep[10],
                                     from_aie_last[10]);
        from_aie_ch[11].store_stream(from_aie_strm11, from_aie_delay[11], from_aie_frame[11], from_aie_rep[11],
                                     from_aie_last[11]);
        from_aie_ch[12].store_stream(from_aie_strm12, from_aie_delay[12], from_aie_frame[12], from_aie_rep[12],
                                     from_aie_last[12]);
        from_aie_ch[13].store_stream(from_aie_strm13, from_aie_delay[13], from_aie_frame[13], from_aie_rep[13],
                                     from_aie_last[13]);
        from_aie_ch[14].store_stream(from_aie_strm14, from_aie_delay[14], from_aie_frame[14], from_aie_rep[14],
                                     from_aie_last[14]);
        from_aie_ch[15].store_stream(from_aie_strm15, from_aie_delay[15], from_aie_frame[15], from_aie_rep[15],
                                     from_aie_last[15]);
    }
};

#endif
