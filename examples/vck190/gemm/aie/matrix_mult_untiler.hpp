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

#ifndef _TEST_HARNESS_MATRIX_MULT_UNTILER_HPP_
#define _TEST_HARNESS_MATRIX_MULT_UNTILER_HPP_

#include <adf.h>

using namespace adf;

namespace xf {
namespace dsp {
namespace aie {
namespace blas {
namespace matrix_mult {
/*
* @brief Acts as a wrapper and the entry point from the graph.
*/
template <unsigned M, unsigned N, unsigned inRow, unsigned inCol, unsigned leadingDim, typename T_D>
class untilerKernelClass {
   public:
    void unTile(input_buffer<T_D>& __restrict inWindow, output_buffer<T_D>& __restrict outWindow);

    static void registerKernelClass() { REGISTER_FUNCTION(untilerKernelClass::unTile); }
};
}
}
}
}
}

#endif // _TEST_HARNESS_MATRIX_MULT_UNTILER_HPP_
