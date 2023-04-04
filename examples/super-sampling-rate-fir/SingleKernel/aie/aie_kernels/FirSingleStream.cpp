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
//#include "../system_settings.h"

#define MULMAC(N)                                                \
    taps = *coeff++;                                             \
    acc = mul4(data, N, 0x3210, 1, taps, 0, 0x0000, 1);          \
    acc = mac4(acc, data, N + 2, 0x3210, 1, taps, 2, 0x0000, 1); \
    acc = mac4(acc, data, N + 4, 0x3210, 1, taps, 4, 0x0000, 1); \
    acc = mac4(acc, data, N + 6, 0x3210, 1, taps, 6, 0x0000, 1)

#define MACMAC(N)                                                \
    taps = *coeff++;                                             \
    acc = mac4(acc, data, N, 0x3210, 1, taps, 0, 0x0000, 1);     \
    acc = mac4(acc, data, N + 2, 0x3210, 1, taps, 2, 0x0000, 1); \
    acc = mac4(acc, data, N + 4, 0x3210, 1, taps, 4, 0x0000, 1); \
    acc = mac4(acc, data, N + 6, 0x3210, 1, taps, 6, 0x0000, 1)

template <int NSamples, int ShiftAcc>
void SingleStream::FIR_SingleStream<NSamples, ShiftAcc>::filter(input_stream_cint16* sin, output_stream_cint16* sout) {
    v8cint16* coeff = (v8cint16*)weights;
    v8cint16 taps = undef_v8cint16();
    v32cint16* ptr_delay_line = (v32cint16*)delay_line;
    v32cint16 data = *ptr_delay_line;

    v4cacc48 acc = undef_v4cacc48();

    // Computes 32 samples per iteration
    for (int i = 0; i < NSamples / 32; i++)
        chess_prepare_for_pipelining chess_loop_range(NSamples / 32, NSamples / 32) chess_pipeline_adjust_preamble(10) {
            MULMAC(1);
            MACMAC(9);
            MACMAC(17);
            data = upd_v(data, 0, readincr_v4(sin));
            MACMAC(25);
            writeincr_v4(sout, srs(acc, ShiftAcc));
            coeff -= 4;

            MULMAC(5);
            MACMAC(13);
            MACMAC(21);
            data = upd_v(data, 1, readincr_v4(sin));
            MACMAC(29);
            writeincr_v4(sout, srs(acc, ShiftAcc));
            coeff -= 4;

            MULMAC(9);
            MACMAC(17);
            MACMAC(25);
            data = upd_v(data, 2, readincr_v4(sin));
            MACMAC(1);
            writeincr_v4(sout, srs(acc, ShiftAcc));
            coeff -= 4;

            MULMAC(13);
            MACMAC(21);
            MACMAC(29);
            data = upd_v(data, 3, readincr_v4(sin));
            MACMAC(5);
            writeincr_v4(sout, srs(acc, ShiftAcc));
            coeff -= 4;

            MULMAC(17);
            MACMAC(25);
            MACMAC(1);
            data = upd_v(data, 4, readincr_v4(sin));
            MACMAC(9);
            writeincr_v4(sout, srs(acc, ShiftAcc));
            coeff -= 4;

            MULMAC(21);
            MACMAC(29);
            MACMAC(5);
            data = upd_v(data, 5, readincr_v4(sin));
            MACMAC(13);
            writeincr_v4(sout, srs(acc, ShiftAcc));
            coeff -= 4;

            MULMAC(25);
            MACMAC(1);
            MACMAC(9);
            data = upd_v(data, 6, readincr_v4(sin));
            MACMAC(17);
            writeincr_v4(sout, srs(acc, ShiftAcc));
            coeff -= 4;

            MULMAC(29);
            MACMAC(5);
            MACMAC(13);
            data = upd_v(data, 7, readincr_v4(sin));
            MACMAC(21);
            writeincr_v4(sout, srs(acc, ShiftAcc));
            coeff -= 4;
        }

    *ptr_delay_line = data;
}
