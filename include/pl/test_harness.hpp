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

#ifndef _TEST_HARNESS_HPP_
#define _TEST_HARNESS_HPP_

#include <ap_int.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <hls_burst_maxi.h>
#include <hls_fence.h>
#include <sys/types.h>
#include <cstdint>
#include "ap_shift_reg.h"

#ifndef __SYNTHESIS__
#include <iostream>
#endif

/**
 * @brief Testing mode, including functional testing mode, performance testing mode, and replay testing mode.
 *
 */
enum TestMode { FUNC_MODE = 0, PERF_MODE };

/**
 * @brief buffer for test harness
 *
 * @tparam URAM_DEPTH Depth of URAM buffer for input/output.
 * @tparam STREAM_WIDTH Bit width of AXI-M, AXI-Stream and URAM buffer width. 128 default.
 * @tparam MEM_WIDTH Bit width of M_AXI buffer width. 512 default.
 */
template <int URAM_DEPTH, int STREAM_WIDTH = 128, int MEM_WIDTH = 512>
class buff_channel {
    static_assert(MEM_WIDTH % STREAM_WIDTH == 0, "MEM_WIDTH should be multiple of STREAM_WIDTH");
    static constexpr const uint32_t MEM_RATIO = MEM_WIDTH / STREAM_WIDTH;
    static_assert(URAM_DEPTH % MEM_RATIO == 0, "URAM_DEPTH * STREAM_WIDTH should be multiple of MEM_WIDTH");
    static constexpr const uint32_t MEM_DEPTH = URAM_DEPTH / MEM_RATIO;
    static_assert(MEM_RATIO <= 16, "MEM_WIDTH / STREAM_WIDTH should be less than or equal to 16");
   public:
    TestMode mode;

    uint32_t numFrames;
    uint32_t numRep;
    uint32_t offsets;

    uint64_t delays;
    uint64_t firstcc;
    uint64_t lastcc;
#ifndef __SYNTHESIS__
    std::vector<ap_uint<STREAM_WIDTH>> buff;
#else
    ap_uint<STREAM_WIDTH> buff[URAM_DEPTH];
#endif

    buff_channel() {
#ifndef __SYNTHESIS__
        buff.resize(URAM_DEPTH);
#else
#pragma HLS inline
#pragma HLS bind_storage variable = buff type = RAM_1P impl = URAM
#endif
        firstcc = 0;
        lastcc = 0;
    }

    ~buff_channel() = default;

    void load_buff(ap_uint<MEM_WIDTH>* source) {
        if (mode == PERF_MODE) {
            if (numFrames > URAM_DEPTH) {
                for (int i = 0; i < MEM_DEPTH; i++) {
                    ap_uint<MEM_WIDTH> data = source[i + offsets];
                    for(int j = 0; j < MEM_RATIO; j++) {
#pragma HLS pipeline II = 1
                        buff[i * MEM_RATIO + j] = data.range((j + 1) * STREAM_WIDTH - 1, j * STREAM_WIDTH);
                    }   
                }
            } else {
                const int numMemFrames = numFrames / MEM_RATIO;
                const int numMemRes = numFrames % MEM_RATIO;
                for (int i = 0; i < numMemFrames; i++) {
                    ap_uint<MEM_WIDTH> data = source[i + offsets];
                    for(int j = 0; j < MEM_RATIO; j++) {
#pragma HLS pipeline II = 1
                        buff[i * MEM_RATIO + j] = data.range((j + 1) * STREAM_WIDTH - 1, j * STREAM_WIDTH);
                    }
                }
                if(numMemRes != 0) {
                    ap_uint<MEM_WIDTH> data = source[numMemFrames + offsets];
                    for(ap_uint<5> j = 0; j < numMemRes; j++) {
#pragma HLS pipeline II = 1
                        buff[numMemFrames * MEM_RATIO + j] = data.range((j + 1) * STREAM_WIDTH - 1, j * STREAM_WIDTH);
                    }
                }
            }
        }
    }

    void store_buff(ap_uint<MEM_WIDTH>* dst) {
        if (mode == PERF_MODE) {
            if (numFrames > URAM_DEPTH) {
                for (int i = 0; i < MEM_DEPTH; i++) {
                    ap_uint<MEM_WIDTH> data = 0;
                    for(int j = 0; j < MEM_RATIO; j++) {
#pragma HLS pipeline II = 1
                        data.range((j + 1) * STREAM_WIDTH - 1, j * STREAM_WIDTH) = buff[i * MEM_RATIO + j];
                    }
                    dst[i + offsets] = data;
                }
            } else {
                const int numMemFrames = numFrames / MEM_RATIO;
                const int numMemRes = numFrames % MEM_RATIO;
                for (int i = 0; i < numMemFrames; i++) {
                    ap_uint<MEM_WIDTH> data = 0;
                    for(int j = 0; j < MEM_RATIO; j++) {
#pragma HLS pipeline II = 1
                        data.range((j + 1) * STREAM_WIDTH - 1, j * STREAM_WIDTH) = buff[i * MEM_RATIO + j];
                    }
                    dst[i + offsets] = data;
                }
                if(numMemRes != 0) {
                    ap_uint<MEM_WIDTH> data = 0;
                    for(ap_uint<5> j = 0; j < numMemRes; j++) {
#pragma HLS pipeline II = 1
                        data.range((j + 1) * STREAM_WIDTH - 1, j * STREAM_WIDTH) = buff[numMemFrames * MEM_RATIO + j];
                    }
                    dst[numMemFrames + offsets] = data;
                }
            }
        }
    }

    void load_stream(hls::stream<ap_uint<MEM_WIDTH> >& memStr, hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_strm) {
#pragma HLS dataflow
        hls::stream<bool> trigger_strm;
#pragma HLS stream variable = trigger_strm depth = 4
        feed(memStr, to_strm, trigger_strm);
        counter(trigger_strm);
    }

    void store_stream(hls::stream<ap_uint<MEM_WIDTH> >& memStr, hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_strm) {
#pragma HLS dataflow
        hls::stream<bool> trigger_strm;
#pragma HLS stream variable = trigger_strm depth = 4
        get(memStr, from_strm, trigger_strm);
        counter(trigger_strm);
    }

   private:
    void counter(hls::stream<bool>& trigger_strm) {
        enum State { START = 0, FIRST = 1, LAST = 3};
        State st = START;
        uint64_t counter = 0;
        while (true) {
#pragma HLS pipeline II = 1
            bool isEmpty = trigger_strm.empty();
            switch (st) {
                case START:
                    if (!isEmpty) {
                        trigger_strm.read();
                        st = FIRST;
                        firstcc = counter;
                    } else {
                        st = START;
                    }
                    break;
                case FIRST:
                    if (!isEmpty) {
                        trigger_strm.read();
                        lastcc = counter;
                        st = LAST;
                    } else {
                        st = FIRST;
                    }
                    break;
                case LAST:
                    return;
                    break;
                default:
                    st = START;
                    break;
            }
            counter++;
        }
    }

    void feed(hls::stream<ap_uint<MEM_WIDTH> >& memStr,
              hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& to_strm,
              hls::stream<bool>& trigger_strm) {
        // delay and send one trigger
        uint64_t count = delays;       
        while (count != 0) {
#pragma HLS pipeline II = 1
            count--;
        }
        
        trigger_strm.write(true);
        // push data to AXI stream
        ap_axiu<STREAM_WIDTH, 0, 0, 0> tmp;
        tmp.keep = -1;
        tmp.last = 0;
        switch (mode) {
            case FUNC_MODE: 
                {
                    uint32_t numStreamReads = numFrames / MEM_RATIO;
                    uint32_t numResiduals = numFrames % MEM_RATIO;
                    for (int i = 0; i < numStreamReads; i++) {
                        ap_uint<MEM_WIDTH> data = memStr.read();
                        for(int j = 0; j < MEM_RATIO; j++) {
#pragma HLS pipeline II = 1
                            tmp.data = data.range((j + 1) * STREAM_WIDTH - 1, j * STREAM_WIDTH);
                            to_strm.write(tmp);
                        }
                    }
                    if (numResiduals != 0) {
                        auto data = memStr.read();
                        for(ap_uint<5> j = 0; j < numResiduals; j++) {
#pragma HLS pipeline II = 1
                            tmp.data = data.range((j + 1) * STREAM_WIDTH - 1, j * STREAM_WIDTH);
                            to_strm.write(tmp);
                        }
                    }
                }
                break;
            case PERF_MODE:
                for (uint32_t j = numRep; j != 0; j--)
                    for (uint32_t i = numFrames; i != 0; i--) {
#pragma HLS pipeline II = 1
                        uint32_t idx = numFrames - i;
                        auto tmp_val = buff[idx % URAM_DEPTH];
                        constexpr int GRAN_WIDTH = 16;  // currently the granularity width is set as 16
                        ap_uint<GRAN_WIDTH> div = idx / URAM_DEPTH;
                        ap_uint<STREAM_WIDTH> val;
                        for(int k = 0; k < STREAM_WIDTH / GRAN_WIDTH; k ++) {
                            val.range((k + 1) * GRAN_WIDTH - 1, k * GRAN_WIDTH) = div << k;
                        }
                        tmp.data = tmp_val ^ val;
                        to_strm.write(tmp);
                    }
                break;
            default:
#ifndef __SYNTHESIS__
                std::cerr << "[ERROR]: Unsupported mode!!! \n";
                exit(1);
#endif
                break;
        }

        // send last trigger
        trigger_strm.write(true);
    }

    void get(hls::stream<ap_uint<MEM_WIDTH> >& memStr,
             hls::stream<ap_axiu<STREAM_WIDTH, 0, 0, 0> >& from_strm,
             hls::stream<bool>& trigger_strm) {
        // delay and send one trigger
        uint64_t count = delays;
       
        while (count != 0) {
#pragma HLS pipeline II = 1
            count--;
        }

        bool firstData = true;

        // pull data from AXI stream
        switch (mode) {
            case FUNC_MODE:
                {
                    uint32_t numStreamReads = numFrames / MEM_RATIO;
                    uint32_t numResiduals = numFrames % MEM_RATIO;
                    for (int i = 0; i < numStreamReads; i++) {
                        ap_uint<MEM_WIDTH> data = 0;
                        for(int j = 0; j < MEM_RATIO; j++) {
#pragma HLS pipeline II = 1
                            ap_axiu<STREAM_WIDTH, 0, 0, 0> tmp = from_strm.read();
                            data.range((j + 1) * STREAM_WIDTH - 1, j * STREAM_WIDTH) = tmp.data;
                            hls::fence({from_strm}, {trigger_strm});
                            if(firstData) {
                                trigger_strm.write(true);
                                firstData = false;
                            }
                        }
                        memStr.write(data);
                    }
                    if (numResiduals != 0) {
                        ap_uint<MEM_WIDTH> data = 0;
                        for(ap_uint<5> j = 0; j < numResiduals; j++) {
#pragma HLS pipeline II = 1
                            ap_axiu<STREAM_WIDTH, 0, 0, 0> tmp = from_strm.read();
                            data.range((j + 1) * STREAM_WIDTH - 1, j * STREAM_WIDTH) = tmp.data;
                            hls::fence({from_strm}, {trigger_strm});
                            if(firstData) {
                                trigger_strm.write(true);
                                firstData = false;
                            }
                        }
                        memStr.write(data);
                    }
                }
                break;
            case PERF_MODE:
                for (uint32_t j = numRep; j != 0; j--)
                    for (uint32_t i = numFrames; i != 0; i--) {
#pragma HLS pipeline II = 1
                        uint32_t idx = numFrames - i;
                        ap_axiu<STREAM_WIDTH, 0, 0, 0> tmp = from_strm.read();
                        hls::fence({from_strm}, {trigger_strm});
                        if(firstData) {
                            trigger_strm.write(true);
                            firstData = false;
                        }
                        buff[idx % URAM_DEPTH] = tmp.data;
                    }
                break;
            default:
#ifndef __SYNTHESIS__
                std::cerr << "[ERROR]: Unsupported mode!!! \n";
                exit(1);
#endif
                break;
        }

        if(firstData) {
            trigger_strm.write(true);
        }
        // send last trigger
        trigger_strm.write(true);
    }
};

/**
 * @brief test harness
 *
 * @tparam NUM_CHANNELS Number of channel to feed AIE and get from AIE. Should be 1, 2, 4, 8, 16
 * @tparam URAM_DEPTH Depth of URAM buffer for input/output.
 * @tparam STREAM_WIDTH Bit width of AXI-Stream and URAM buffer width. 128 default.
 * @tparam MEM_WIDTH Bit width of M_AXI buffer width. 512 default.
 */
template <int NUM_CHANNELS, int URAM_DEPTH, int STREAM_WIDTH = 128, int MEM_WIDTH = 512>
class test_harness {
    static_assert(MEM_WIDTH % STREAM_WIDTH == 0, "MEM_WIDTH should be multiple of STREAM_WIDTH");
    static constexpr const uint32_t MEM_RATIO = MEM_WIDTH / STREAM_WIDTH;
    static_assert(URAM_DEPTH % MEM_RATIO == 0, "URAM_DEPTH * STREAM_WIDTH should be multiple of MEM_WIDTH");
    static constexpr const uint32_t MEM_DEPTH = URAM_DEPTH / MEM_RATIO;

   public:
    buff_channel<URAM_DEPTH, STREAM_WIDTH, MEM_WIDTH> to_aie_ch[NUM_CHANNELS];
    buff_channel<URAM_DEPTH, STREAM_WIDTH, MEM_WIDTH> from_aie_ch[NUM_CHANNELS];

    test_harness() {
#pragma HLS inline
#pragma HLS array_partition variable = to_aie_ch type = complete dim = 1
#pragma HLS array_partition variable = from_aie_ch type = complete dim = 1
    }

    void load_cfg(uint64_t* cfg) {
#pragma HLS INLINE off
        for (int i = 0; i < NUM_CHANNELS; i++) {
            to_aie_ch[i].mode = cfg[0] == 0 ? FUNC_MODE : PERF_MODE;
            from_aie_ch[i].mode = cfg[0] == 0 ? FUNC_MODE : PERF_MODE;
        }

        for (int i = 1; i <= NUM_CHANNELS; i++) {
            to_aie_ch[i - 1].delays = cfg[i];
        }

        for (int i = 1; i <= NUM_CHANNELS; i++) {
            to_aie_ch[i - 1].numFrames = cfg[i + NUM_CHANNELS];
        }

        for (int i = 1; i <= NUM_CHANNELS; i++) {
            to_aie_ch[i - 1].numRep = cfg[i + NUM_CHANNELS * 2];
        }

        for (int i = 1; i <= NUM_CHANNELS; i++) {
            to_aie_ch[i - 1].offsets = cfg[i + NUM_CHANNELS * 3];
        }

        for (int i = 1; i <= NUM_CHANNELS; i++) {
            from_aie_ch[i - 1].delays = cfg[i + NUM_CHANNELS * 4];
        }

        for (int i = 1; i <= NUM_CHANNELS; i++) {
            from_aie_ch[i - 1].numFrames = cfg[i + NUM_CHANNELS * 5];
        }

        for (int i = 1; i <= NUM_CHANNELS; i++) {
            from_aie_ch[i - 1].numRep = cfg[i + NUM_CHANNELS * 6];
        }

        for (int i = 1; i <= NUM_CHANNELS; i++) {
            from_aie_ch[i - 1].offsets = cfg[i + NUM_CHANNELS * 7];
        }
    }

    void store_perf(uint64_t* perf) {
#pragma HLS INLINE off
        for (int i = 0; i < NUM_CHANNELS; i++) {
            perf[i] = to_aie_ch[i].firstcc;
        }
        for (int i = 0; i < NUM_CHANNELS; i++) {
            perf[i + NUM_CHANNELS] = to_aie_ch[i].lastcc;
        }
        for (int i = 0; i < NUM_CHANNELS; i++) {
            perf[i + NUM_CHANNELS * 2] = from_aie_ch[i].firstcc;
        }
        for (int i = 0; i < NUM_CHANNELS; i++) {
            perf[i + NUM_CHANNELS * 3] = from_aie_ch[i].lastcc;
        }
    }

    void load_buffs(ap_uint<MEM_WIDTH>* mem) {
#pragma HLS INLINE off
        for (int i = 0; i < NUM_CHANNELS; i++) {
            to_aie_ch[i].load_buff(mem);
        }
    }

    void store_buffs(ap_uint<MEM_WIDTH>* mem) {
#pragma HLS INLINE off
        for (int i = 0; i < NUM_CHANNELS; i++) {
            from_aie_ch[i].store_buff(mem);
        }
    }

    template <int BURST_LEN>
    void load_memories(ap_uint<MEM_WIDTH>* mem, hls::stream<ap_uint<MEM_WIDTH> > to_aie_strm[NUM_CHANNELS]) {
        static_assert(BURST_LEN <= 128, "BURST_LEN should be less than or equal to 128");
#pragma HLS stream variable = to_aie_strm depth = BURST_LEN * 2
#pragma HLS array_partition variable = to_aie_strm complete
        static_assert(BURST_LEN % MEM_RATIO == 0, "BURST_LEN should be multiple of MEM_RATIO");
        uint32_t num_valid_channels = NUM_CHANNELS;
        uint32_t frame_idx[NUM_CHANNELS];
        ap_shift_reg<uint32_t, NUM_CHANNELS> channel_regs;
        for(int i=0;i<NUM_CHANNELS;i++) {
            channel_regs.shift(i);
            frame_idx[i] = 0;
        }
        
        if (to_aie_ch[0].mode == FUNC_MODE) {
            while (num_valid_channels != 0) {
                uint32_t ch_id = channel_regs.read(num_valid_channels - 1);
                const uint32_t numData = (to_aie_ch[ch_id].numFrames + MEM_RATIO - 1 ) / MEM_RATIO - frame_idx[ch_id];
                if (numData == 0) {
                    num_valid_channels--;
                    continue;
                }
                channel_regs.shift(ch_id);

                if (numData >= BURST_LEN && to_aie_strm[ch_id].size() + BURST_LEN <= to_aie_strm[ch_id].capacity()) {
                    for (int j = 0; j < BURST_LEN; j++) {
#pragma HLS pipeline II = 1
                        ap_int<MEM_WIDTH> tmp = mem[to_aie_ch[ch_id].offsets + frame_idx[ch_id] + j];
                        to_aie_strm[ch_id].write(tmp);
                    }
                    frame_idx[ch_id] += BURST_LEN;
                } else if (to_aie_strm[ch_id].size() + numData <= to_aie_strm[ch_id].capacity()) {
                    for (ap_uint<8> j = 0; j < numData; j++) {
#pragma HLS pipeline II = 1
                        ap_int<MEM_WIDTH> tmp = mem[to_aie_ch[ch_id].offsets + frame_idx[ch_id] + j];
                        to_aie_strm[ch_id].write(tmp);
                    }
                    frame_idx[ch_id] += numData;
                }
            }
        }
    }

    template <int BURST_LEN>
    void store_memories(ap_uint<MEM_WIDTH>* mem, hls::stream<ap_uint<MEM_WIDTH> > from_aie_strm[NUM_CHANNELS]) {
        static_assert(BURST_LEN <= 128, "BURST_LEN should be less than or equal to 128");
#pragma HLS stream variable = from_aie_strm depth = BURST_LEN * 2
#pragma HLS array_partition variable = from_aie_strm complete
        static_assert(BURST_LEN % MEM_RATIO == 0, "BURST_LEN should be multiple of MEM_RATIO");
        uint32_t num_valid_channels = NUM_CHANNELS;
        uint32_t frame_idx[NUM_CHANNELS];
        ap_shift_reg<uint32_t, NUM_CHANNELS> channel_regs;
        for(int i=0;i<NUM_CHANNELS;i++) {
            channel_regs.shift(i);
            frame_idx[i] = 0;
        }

        if (from_aie_ch[0].mode == FUNC_MODE) {
            while (num_valid_channels != 0) {
                uint32_t ch_id = channel_regs.read(num_valid_channels - 1);
                const uint32_t numData = (from_aie_ch[ch_id].numFrames + MEM_RATIO - 1) / MEM_RATIO - frame_idx[ch_id];
                if (numData == 0) {
                    num_valid_channels--;
                    continue;
                }
                channel_regs.shift(ch_id);

                if (numData >= BURST_LEN && from_aie_strm[ch_id].size() >= BURST_LEN) {
                    for (int j = 0; j < BURST_LEN; j++) {
#pragma HLS pipeline II = 1
                        mem[from_aie_ch[ch_id].offsets + frame_idx[ch_id] + j] = from_aie_strm[ch_id].read();;
                    }
                    frame_idx[ch_id] += BURST_LEN;
                } else if (from_aie_strm[ch_id].size() == numData) {
                    for (ap_uint<8> j = 0; j < numData; j++) {
#pragma HLS pipeline II = 1
                        mem[from_aie_ch[ch_id].offsets + frame_idx[ch_id] + j] = from_aie_strm[ch_id].read();;
                    }
                    frame_idx[ch_id] += numData;
                }
            }
        }
    }
};

#endif
