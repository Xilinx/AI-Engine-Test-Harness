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

#pragma once

#ifndef _TEST_HARNESS_FIR_PARAMS_DEFAULTS_HPP_
#define _TEST_HARNESS_FIR_PARAMS_DEFAULTS_HPP_

#include <stdio.h>
#include <adf.h>

namespace xf {
namespace dsp {
namespace aie {

struct fir_params_defaults {
   public:
    using BTT_DATA = cint16;
    using BTT_COEFF = int16;
    static constexpr unsigned int Bdim = 0;
    static constexpr unsigned int BTP_FIR_LEN = 16;
    static constexpr unsigned int BTP_FIR_RANGE_LEN = 4;
    static constexpr unsigned int BTP_SHIFT = 0;
    static constexpr unsigned int BTP_RND = 0;
    static constexpr unsigned int BTP_INTERPOLATE_FACTOR = 1;
    static constexpr unsigned int BTP_DECIMATE_FACTOR = 1;
    static constexpr unsigned int BTP_INPUT_WINDOW_VSIZE = 256;
    static constexpr unsigned int BTP_CASC_LEN = 1;
    static constexpr unsigned int BTP_USE_COEFF_RELOAD = 0;
    static constexpr unsigned int BTP_NUM_OUTPUTS = 1;
    static constexpr unsigned int BTP_DUAL_IP = 0;
    static constexpr unsigned int BTP_API = 0;
    static constexpr unsigned int BTP_SSR = 1;
    static constexpr unsigned int BTP_COEFF_PHASE = 0;
    static constexpr unsigned int BTP_COEFF_PHASE_OFFSET = 0;
    static constexpr unsigned int BTP_COEFF_PHASES = 1;
    static constexpr unsigned int BTP_COEFF_PHASES_LEN = BTP_FIR_RANGE_LEN * BTP_COEFF_PHASES;
    static constexpr unsigned int BTP_PARA_INTERP_POLY = 1;
    static constexpr unsigned int BTP_PARA_DECI_POLY = 1;
    static constexpr unsigned int BTP_PARA_INTERP_INDEX = 0;
    static constexpr unsigned int BTP_PARA_DECI_INDEX = 0;
    static constexpr bool BTP_CASC_IN = false;
    static constexpr bool BTP_CASC_OUT = false;
    static constexpr int BTP_MODIFY_MARGIN_OFFSET = 0;
    static constexpr unsigned int BTP_KERNEL_POSITION = 0;
};
template <typename fp = fir_params_defaults>
void printParams() {
    printf("FIR Params: \n");
    printf("Bdim                      = %d.\n", fp::Bdim);
    printf("BTP_FIR_LEN               = %d.\n", fp::BTP_FIR_LEN);
    printf("BTP_FIR_RANGE_LEN         = %d.\n", fp::BTP_FIR_RANGE_LEN);
    printf("BTP_SHIFT                 = %d.\n", fp::BTP_SHIFT);
    printf("BTP_RND                   = %d.\n", fp::BTP_RND);
    printf("BTP_INTERPOLATE_FACTOR    = %d.\n", fp::BTP_INTERPOLATE_FACTOR);
    printf("BTP_DECIMATE_FACTOR       = %d.\n", fp::BTP_DECIMATE_FACTOR);
    printf("BTP_INPUT_WINDOW_VSIZE    = %d.\n", fp::BTP_INPUT_WINDOW_VSIZE);
    printf("BTP_CASC_LEN              = %d.\n", fp::BTP_CASC_LEN);
    printf("BTP_USE_COEFF_RELOAD      = %d.\n", fp::BTP_USE_COEFF_RELOAD);
    printf("BTP_NUM_OUTPUTS           = %d.\n", fp::BTP_NUM_OUTPUTS);
    printf("BTP_DUAL_IP               = %d.\n", fp::BTP_DUAL_IP);
    printf("BTP_API                   = %d.\n", fp::BTP_API);
    printf("BTP_SSR                   = %d.\n", fp::BTP_SSR);
    printf("BTP_COEFF_PHASE           = %d.\n", fp::BTP_COEFF_PHASE);
    printf("BTP_COEFF_PHASE_OFFSET    = %d.\n", fp::BTP_COEFF_PHASE_OFFSET);
    printf("BTP_COEFF_PHASES          = %d.\n", fp::BTP_COEFF_PHASES);
    printf("BTP_COEFF_PHASES_LEN      = %d.\n", fp::BTP_COEFF_PHASES_LEN);
    printf("BTP_PARA_INTERP_INDEX     = %d.\n", fp::BTP_PARA_INTERP_INDEX);
    printf("BTP_PARA_DECI_INDEX       = %d.\n", fp::BTP_PARA_DECI_INDEX);
    printf("BTP_PARA_INTERP_POLY      = %d.\n", fp::BTP_PARA_INTERP_POLY);
    printf("BTP_PARA_DECI_POLY        = %d.\n", fp::BTP_PARA_DECI_POLY);
    printf("BTP_CASC_IN               = %d.\n", fp::BTP_CASC_IN);
    printf("BTP_CASC_OUT              = %d.\n", fp::BTP_CASC_OUT);
    printf("BTP_MODIFY_MARGIN_OFFSET  = %d.\n", fp::BTP_MODIFY_MARGIN_OFFSET);
    printf("BTP_KERNEL_POSITION       = %d.\n", fp::BTP_KERNEL_POSITION);
}
template <typename fp = fir_params_defaults>
class fir_type_default {
   public:
    static constexpr unsigned int getKernelFirRangeLen();

    static constexpr unsigned int getFirRangeLen();

    static constexpr unsigned int getTapLen();

    static constexpr unsigned int getDF();

    static constexpr unsigned int getSSRMargin();

    static constexpr unsigned int getFirType();
};
}
}
}
#endif // _TEST_HARNESS_FIR_PARAMS_DEFAULTS_HPP_
