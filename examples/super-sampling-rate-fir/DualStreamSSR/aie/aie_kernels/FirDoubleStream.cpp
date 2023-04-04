/*
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
 */
#include <adf.h>

#include "FirDoubleStream.h"

template <int NSamples, int ShiftAcc, bool DiscardSample, bool SwapRead>
void DoubleStream::FIR_MultiKernel_cout<NSamples, ShiftAcc, DiscardSample, SwapRead>::filter(
    input_stream_cint16* restrict sin1, input_stream_cint16* restrict sin2, output_stream_cacc48* cout) {
    v8cint16 taps = *(v8cint16*)weights;
    v16cint16* ptr_delay_line = (v16cint16*)delay_line;
    v16cint16 data = *ptr_delay_line;

    v4cacc48 acc = undef_v4cacc48();

    static const int Start_1 = (DiscardSample ? 2 : 1);
    static const int Start_3 = (DiscardSample ? 4 : 3);
    static const int Start_5 = (DiscardSample ? 6 : 5);
    static const int Start_7 = (DiscardSample ? 8 : 7);
    static const int Start_9 = (DiscardSample ? 10 : 9);
    static const int Start_11 = (DiscardSample ? 12 : 11);
    static const int Start_13 = (DiscardSample ? 14 : 13);
    static const int Start_15 = (DiscardSample ? 0 : 15);

    // Computes 16 samples per iteration
    for (int i = 0; i < NSamples / 16; i++)
        chess_prepare_for_pipelining chess_loop_range(NSamples / 16, NSamples / 16) {
            if (SwapRead)
                data = upd_w(data, 1, concat(getc_wss(1), getc_wss(0)));
            else
                data = upd_w(data, 1, concat(getc_wss(0), getc_wss(1)));

            acc = mul4(data, Start_1, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, Start_3, 0x3210, 1, taps, 2, 0x0000, 1);
            writeincr_v4(cout, acc);

            acc = mul4(data, Start_5, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, Start_7, 0x3210, 1, taps, 2, 0x0000, 1);
            writeincr_v4(cout, acc);

            if (SwapRead)
                data = upd_w(data, 0, concat(getc_wss(1), getc_wss(0)));
            else
                data = upd_w(data, 0, concat(getc_wss(0), getc_wss(1)));

            acc = mul4(data, Start_9, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, Start_11, 0x3210, 1, taps, 2, 0x0000, 1);
            writeincr_v4(cout, acc);

            acc = mul4(data, Start_13, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, Start_15, 0x3210, 1, taps, 2, 0x0000, 1);
            writeincr_v4(cout, acc);
        }

    *ptr_delay_line = data;
}

template <int NSamples, int ShiftAcc, bool DiscardSample, bool SwapRead>
void DoubleStream::FIR_MultiKernel_cincout<NSamples, ShiftAcc, DiscardSample, SwapRead>::filter(
    input_stream_cint16* restrict sin1,
    input_stream_cint16* restrict sin2,
    input_stream_cacc48* cin,
    output_stream_cacc48* cout) {
    v8cint16 taps = *(v8cint16*)weights;
    v16cint16* ptr_delay_line = (v16cint16*)delay_line;
    v16cint16 data = *ptr_delay_line;

    v4cacc48 acc = undef_v4cacc48();

    static const int Start_1 = (DiscardSample ? 2 : 1);
    static const int Start_3 = (DiscardSample ? 4 : 3);
    static const int Start_5 = (DiscardSample ? 6 : 5);
    static const int Start_7 = (DiscardSample ? 8 : 7);
    static const int Start_9 = (DiscardSample ? 10 : 9);
    static const int Start_11 = (DiscardSample ? 12 : 11);
    static const int Start_13 = (DiscardSample ? 14 : 13);
    static const int Start_15 = (DiscardSample ? 0 : 15);

    // Computes 16 samples per iteration
    for (int i = 0; i < NSamples / 16; i++)
        chess_prepare_for_pipelining chess_loop_range(NSamples / 16, NSamples / 16) {
            acc = readincr_v4(cin);
            if (SwapRead)
                data = upd_w(data, 1, concat(getc_wss(1), getc_wss(0)));
            else
                data = upd_w(data, 1, concat(getc_wss(0), getc_wss(1)));

            acc = mac4(acc, data, Start_1, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, Start_3, 0x3210, 1, taps, 2, 0x0000, 1);
            writeincr_v4(cout, acc);

            acc = readincr_v4(cin);
            acc = mac4(acc, data, Start_5, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, Start_7, 0x3210, 1, taps, 2, 0x0000, 1);
            writeincr_v4(cout, acc);

            acc = readincr_v4(cin);
            if (SwapRead)
                data = upd_w(data, 0, concat(getc_wss(1), getc_wss(0)));
            else
                data = upd_w(data, 0, concat(getc_wss(0), getc_wss(1)));

            acc = mac4(acc, data, Start_9, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, Start_11, 0x3210, 1, taps, 2, 0x0000, 1);
            writeincr_v4(cout, acc);

            acc = readincr_v4(cin);
            acc = mac4(acc, data, Start_13, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, Start_15, 0x3210, 1, taps, 2, 0x0000, 1);
            writeincr_v4(cout, acc);
        }

    *ptr_delay_line = data;
}

template <int NSamples, int ShiftAcc, bool DiscardSample, bool SwapRead>
void DoubleStream::FIR_MultiKernel_cin<NSamples, ShiftAcc, DiscardSample, SwapRead>::filter(
    input_stream_cint16* restrict sin1,
    input_stream_cint16* restrict sin2,
    input_stream_cacc48* cin,
    output_stream_cint16* restrict sout1,
    output_stream_cint16* restrict sout2) {
    v8cint16 taps = *(v8cint16*)weights;
    v16cint16* ptr_delay_line = (v16cint16*)delay_line;
    v16cint16 data = *ptr_delay_line;

    v4cacc48 acc = undef_v4cacc48();

    static const int Start_1 = (DiscardSample ? 2 : 1);
    static const int Start_3 = (DiscardSample ? 4 : 3);
    static const int Start_5 = (DiscardSample ? 6 : 5);
    static const int Start_7 = (DiscardSample ? 8 : 7);
    static const int Start_9 = (DiscardSample ? 10 : 9);
    static const int Start_11 = (DiscardSample ? 12 : 11);
    static const int Start_13 = (DiscardSample ? 14 : 13);
    static const int Start_15 = (DiscardSample ? 0 : 15);

    // Computes 16 samples per iteration
    for (int i = 0; i < NSamples / 16; i++)
        chess_prepare_for_pipelining chess_loop_range(NSamples / 16, NSamples / 16) {
            acc = readincr_v4(cin);
            if (SwapRead)
                data = upd_w(data, 1, concat(getc_wss(1), getc_wss(0)));
            else
                data = upd_w(data, 1, concat(getc_wss(0), getc_wss(1)));

            acc = mac4(acc, data, Start_1, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, Start_3, 0x3210, 1, taps, 2, 0x0000, 1);
            put_wms(0, srs(acc, ShiftAcc));

            acc = readincr_v4(cin);
            acc = mac4(acc, data, Start_5, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, Start_7, 0x3210, 1, taps, 2, 0x0000, 1);
            put_wms(1, srs(acc, ShiftAcc));

            acc = readincr_v4(cin);
            if (SwapRead)
                data = upd_w(data, 0, concat(getc_wss(1), getc_wss(0)));
            else
                data = upd_w(data, 0, concat(getc_wss(0), getc_wss(1)));

            acc = mac4(acc, data, Start_9, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, Start_11, 0x3210, 1, taps, 2, 0x0000, 1);
            put_wms(0, srs(acc, ShiftAcc));

            acc = readincr_v4(cin);
            acc = mac4(acc, data, Start_13, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, Start_15, 0x3210, 1, taps, 2, 0x0000, 1);
            put_wms(1, srs(acc, ShiftAcc));
        }

    *ptr_delay_line = data;
}

template <int Delay>
void DoubleStream::FIRinit() {
    /*
        int D = Delay - (Delay%2); // remove the 1 element if it exists
        int Block = (D+4)/8;
        int NTot = D/4;
        int n=0;
        for (int b = 0; b < Block; ++b,n++)
        {
                for(int i=0;i<4;i++)	get_ss(0);
                if(++n<NTot)
                        for(int i=0;i<4;i++)	get_ss(1);
        }
    */
}
