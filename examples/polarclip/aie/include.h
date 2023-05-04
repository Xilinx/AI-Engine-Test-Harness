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

#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#define INTERPOLATOR27_INPUT_SAMPLES 128
// NOTE: THIS AMOUNT MUST AGREE WITH THE INPUT_SAMPLES IN HOST.CPP

#define INTERPOLATOR27_INPUT_MARGIN (16 * 4)
#define INTERPOLATOR27_COEFFICIENTS 16
#define INTERPOLATOR27_OUTPUT_SAMPLES (INTERPOLATOR27_INPUT_SAMPLES * 2)
#define INTERPOLATOR27_INPUT_BLOCK_SIZE (INTERPOLATOR27_INPUT_SAMPLES * 4)

#define POLAR_CLIP_INPUT_SAMPLES (INTERPOLATOR27_OUTPUT_SAMPLES)
#define POLAR_CLIP_INPUT_BLOCK_SIZE (POLAR_CLIP_INPUT_SAMPLES * 4)
#define POLAR_CLIP_OUTPUT_SAMPLES (POLAR_CLIP_INPUT_SAMPLES)

#define CLASSIFIER_OUTPUT_SAMPLES (POLAR_CLIP_OUTPUT_SAMPLES)
#define CLASSIFIER_OUTPUT_BLOCK_SIZE (CLASSIFIER_OUTPUT_SAMPLES * 4)

#endif /**********__INCLUDE_H__**********/
