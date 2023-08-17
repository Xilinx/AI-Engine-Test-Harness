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

//------------------------------------------------------------------------------
// UUT DEFAULT CONFIGURATION

#ifndef DATA_TYPE
#define DATA_TYPE cint16
#endif

#ifndef MIXER_MODE
#define MIXER_MODE 0
#endif

#ifndef INPUT_WINDOW_VSIZE
#define INPUT_WINDOW_VSIZE 256
#endif

#ifndef P_API
#define P_API 0
#endif

#ifndef UUT_SSR
#define UUT_SSR 1
#endif

#ifndef NITER
#define NITER 16
#endif

#ifndef INITIAL_DDS_OFFSET
#define INITIAL_DDS_OFFSET 0
#endif

#ifndef DDS_PHASE_INC
#define DDS_PHASE_INC 1797958314
#endif

#ifndef DATA_SEED
#define DATA_SEED 1
#endif

#ifndef DATA_STIM_TYPE
#define DATA_STIM_TYPE 0
#endif

#ifndef SFDR
#define SFDR 90
#endif

#ifndef AIE_VARIANT
#define AIE_VARIANT 1
#endif

// END OF UUT CONFIGURATION
//------------------------------------------------------------------------------
