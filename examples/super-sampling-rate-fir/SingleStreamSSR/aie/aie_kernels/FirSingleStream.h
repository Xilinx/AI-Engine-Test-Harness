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
 */
#pragma once

#include <adf.h>

namespace SingleStream {

void FIRinit(const int Delay);

template <int NSamples, int ShiftAcc>
class FIR_MultiKernel_cout {
   private:
    alignas(32) cint16 weights[8];
    alignas(32) cint16 delay_line[16];

   public:
    FIR_MultiKernel_cout(const cint16 (&taps)[8], const int Delay) {
        for (int i = 0; i < 8; i++) weights[i] = taps[i];
        for (int i = 0; i < 16; i++) delay_line[i] = (cint16){0, 0};

        FIRinit(Delay);
    };

    void filter(input_stream_cint16* sin, output_stream_cacc48* cout);

    static void registerKernelClass() { REGISTER_FUNCTION(FIR_MultiKernel_cout::filter); };
};

template <int NSamples, int ShiftAcc>
class FIR_MultiKernel_cincout {
   private:
    alignas(32) cint16 weights[8];
    alignas(32) cint16 delay_line[16];

   public:
    FIR_MultiKernel_cincout(const cint16 (&taps)[8], const int Delay) {
        for (int i = 0; i < 8; i++) weights[i] = taps[i];
        for (int i = 0; i < 16; i++) delay_line[i] = (cint16){0, 0};

        FIRinit(Delay);
    };

    void filter(input_stream_cint16* sin, input_stream_cacc48* cin, output_stream_cacc48* cout);

    static void registerKernelClass() { REGISTER_FUNCTION(FIR_MultiKernel_cincout::filter); };
};

template <int NSamples, int ShiftAcc>
class FIR_MultiKernel_cin {
   private:
    alignas(32) cint16 weights[8];
    alignas(32) cint16 delay_line[16];

   public:
    FIR_MultiKernel_cin(const cint16 (&taps)[8], const int Delay) {
        for (int i = 0; i < 8; i++) weights[i] = taps[i];
        for (int i = 0; i < 16; i++) delay_line[i] = (cint16){0, 0};

        FIRinit(Delay);
    };

    void filter(input_stream_cint16* sin, input_stream_cacc48* cin, output_stream_cint16* sout);

    static void registerKernelClass() { REGISTER_FUNCTION(FIR_MultiKernel_cin::filter); };
};
}
