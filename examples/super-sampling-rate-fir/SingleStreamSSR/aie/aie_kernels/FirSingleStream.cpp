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

#include "FirSingleStream.h"

template <int NSamples, int ShiftAcc>
void SingleStream::FIR_MultiKernel_cout<NSamples, ShiftAcc>::filter(input_stream_cint16* sin,
                                                                    output_stream_cacc48* cout) {
    v8cint16 taps = *(v8cint16*)weights;
    v16cint16* ptr_delay_line = (v16cint16*)delay_line;
    v16cint16 data = *ptr_delay_line;

    v4cacc48 acc = undef_v4cacc48();

    // Computes 16 samples per iteration
    for (int i = 0; i < NSamples / 16; i++)
        chess_prepare_for_pipelining chess_loop_range(NSamples / 16, NSamples / 16) {
            acc = mul4(data, 1, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, 3, 0x3210, 1, taps, 2, 0x0000, 1);
            data = upd_v(data, 2, readincr_v4(sin));
            acc = mac4(acc, data, 5, 0x3210, 1, taps, 4, 0x0000, 1);
            acc = mac4(acc, data, 7, 0x3210, 1, taps, 6, 0x0000, 1);
            writeincr_v4(cout, acc);

            acc = mul4(data, 5, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, 7, 0x3210, 1, taps, 2, 0x0000, 1);
            data = upd_v(data, 3, readincr_v4(sin));
            acc = mac4(acc, data, 9, 0x3210, 1, taps, 4, 0x0000, 1);
            acc = mac4(acc, data, 11, 0x3210, 1, taps, 6, 0x0000, 1);
            writeincr_v4(cout, acc);

            acc = mul4(data, 9, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, 11, 0x3210, 1, taps, 2, 0x0000, 1);
            data = upd_v(data, 0, readincr_v4(sin));
            acc = mac4(acc, data, 13, 0x3210, 1, taps, 4, 0x0000, 1);
            acc = mac4(acc, data, 15, 0x3210, 1, taps, 6, 0x0000, 1);
            writeincr_v4(cout, acc);

            acc = mul4(data, 13, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, 15, 0x3210, 1, taps, 2, 0x0000, 1);
            data = upd_v(data, 1, readincr_v4(sin));
            acc = mac4(acc, data, 1, 0x3210, 1, taps, 4, 0x0000, 1);
            acc = mac4(acc, data, 3, 0x3210, 1, taps, 6, 0x0000, 1);
            writeincr_v4(cout, acc);
        }

    *ptr_delay_line = data;
}

template <int NSamples, int ShiftAcc>
void SingleStream::FIR_MultiKernel_cincout<NSamples, ShiftAcc>::filter(input_stream_cint16* sin,
                                                                       input_stream_cacc48* cin,
                                                                       output_stream_cacc48* cout) {
    v8cint16 taps = *(v8cint16*)weights;
    v16cint16* ptr_delay_line = (v16cint16*)delay_line;
    v16cint16 data = *ptr_delay_line;

    v4cacc48 acc = undef_v4cacc48();

    // Computes 16 samples per iteration
    for (int i = 0; i < NSamples / 16; i++)
        chess_prepare_for_pipelining chess_loop_range(NSamples / 16, NSamples / 16) {
            acc = readincr_v4(cin);
            acc = mac4(acc, data, 1, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, 3, 0x3210, 1, taps, 2, 0x0000, 1);
            data = upd_v(data, 2, readincr_v4(sin));
            acc = mac4(acc, data, 5, 0x3210, 1, taps, 4, 0x0000, 1);
            acc = mac4(acc, data, 7, 0x3210, 1, taps, 6, 0x0000, 1);
            writeincr_v4(cout, acc);

            acc = readincr_v4(cin);
            acc = mac4(acc, data, 5, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, 7, 0x3210, 1, taps, 2, 0x0000, 1);
            data = upd_v(data, 3, readincr_v4(sin));
            acc = mac4(acc, data, 9, 0x3210, 1, taps, 4, 0x0000, 1);
            acc = mac4(acc, data, 11, 0x3210, 1, taps, 6, 0x0000, 1);
            writeincr_v4(cout, acc);

            acc = readincr_v4(cin);
            acc = mac4(acc, data, 9, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, 11, 0x3210, 1, taps, 2, 0x0000, 1);
            data = upd_v(data, 0, readincr_v4(sin));
            acc = mac4(acc, data, 13, 0x3210, 1, taps, 4, 0x0000, 1);
            acc = mac4(acc, data, 15, 0x3210, 1, taps, 6, 0x0000, 1);
            writeincr_v4(cout, acc);

            acc = readincr_v4(cin);
            acc = mac4(acc, data, 13, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, 15, 0x3210, 1, taps, 2, 0x0000, 1);
            data = upd_v(data, 1, readincr_v4(sin));
            acc = mac4(acc, data, 1, 0x3210, 1, taps, 4, 0x0000, 1);
            acc = mac4(acc, data, 3, 0x3210, 1, taps, 6, 0x0000, 1);
            writeincr_v4(cout, acc);
        }

    *ptr_delay_line = data;
}

template <int NSamples, int ShiftAcc>
void SingleStream::FIR_MultiKernel_cin<NSamples, ShiftAcc>::filter(input_stream_cint16* sin,
                                                                   input_stream_cacc48* cin,
                                                                   output_stream_cint16* sout) {
    v8cint16 taps = *(v8cint16*)weights;
    v16cint16* ptr_delay_line = (v16cint16*)delay_line;
    v16cint16 data = *ptr_delay_line;

    v4cacc48 acc = undef_v4cacc48();

    // Computes 16 samples per iteration
    for (int i = 0; i < NSamples / 16; i++)
        chess_prepare_for_pipelining chess_loop_range(NSamples / 16, NSamples / 16) {
            acc = readincr_v4(cin);
            acc = mac4(acc, data, 1, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, 3, 0x3210, 1, taps, 2, 0x0000, 1);
            data = upd_v(data, 2, readincr_v4(sin));
            acc = mac4(acc, data, 5, 0x3210, 1, taps, 4, 0x0000, 1);
            acc = mac4(acc, data, 7, 0x3210, 1, taps, 6, 0x0000, 1);
            writeincr_v4(sout, srs(acc, ShiftAcc));

            acc = readincr_v4(cin);
            acc = mac4(acc, data, 5, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, 7, 0x3210, 1, taps, 2, 0x0000, 1);
            data = upd_v(data, 3, readincr_v4(sin));
            acc = mac4(acc, data, 9, 0x3210, 1, taps, 4, 0x0000, 1);
            acc = mac4(acc, data, 11, 0x3210, 1, taps, 6, 0x0000, 1);
            writeincr_v4(sout, srs(acc, ShiftAcc));

            acc = readincr_v4(cin);
            acc = mac4(acc, data, 9, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, 11, 0x3210, 1, taps, 2, 0x0000, 1);
            data = upd_v(data, 0, readincr_v4(sin));
            acc = mac4(acc, data, 13, 0x3210, 1, taps, 4, 0x0000, 1);
            acc = mac4(acc, data, 15, 0x3210, 1, taps, 6, 0x0000, 1);
            writeincr_v4(sout, srs(acc, ShiftAcc));

            acc = readincr_v4(cin);
            acc = mac4(acc, data, 13, 0x3210, 1, taps, 0, 0x0000, 1);
            acc = mac4(acc, data, 15, 0x3210, 1, taps, 2, 0x0000, 1);
            data = upd_v(data, 1, readincr_v4(sin));
            acc = mac4(acc, data, 1, 0x3210, 1, taps, 4, 0x0000, 1);
            acc = mac4(acc, data, 3, 0x3210, 1, taps, 6, 0x0000, 1);
            writeincr_v4(sout, srs(acc, ShiftAcc));
        }

    *ptr_delay_line = data;
}

void SingleStream::FIRinit(const int Delay) {
    for (int i = 0; i < Delay; ++i) {
        get_ss(0);
    }
}
