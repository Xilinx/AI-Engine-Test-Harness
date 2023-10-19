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

#ifndef _TEST_HARNESS_GRAPH_UTILS_HPP_
#define _TEST_HARNESS_GRAPH_UTILS_HPP_
/*
The file captures the definition of the graph utilities commonly used across various library elements
*/

#include <adf.h>
#include <vector>

namespace xf {
namespace dsp {
namespace aie {
using namespace adf;

/**
 * @ingroup graph_utils
 *
 * @brief empty class is a helper utility to conditionally remove instances of other classes.
 */
class empty {};

/**
 * @ingroup graph_utils
 *
 * @brief no_port class is a helper utility to conditionally remove instances of other classes.
 */
class no_port {};

//--------------------------------------------------------------------------------------------------
// port_conditional
//--------------------------------------------------------------------------------------------------
/**
 * @ingroup graph_utils
 *
 * @brief port_conditional is an helper alias to conditionally instance port.
 * @tparam DIRECTION describes port direction: input, output or inout.
 * @tparam Condition when met port of a DIRECTION is created, otherwise a no_port struct is instanced.
 */
template <typename DIRECTION, bool Condition>
using port_conditional = typename std::conditional<Condition, port<DIRECTION>, no_port>::type;

//--------------------------------------------------------------------------------------------------
// port_array
//--------------------------------------------------------------------------------------------------
/**
 * @ingroup graph_utils
 *
 * @brief port_array is an helper alias to instance port array
 * Uses: ``std::array`` to instance an array of ``port<DIRECTION>`` classes.
 * @tparam DIRECTION describes port direction: input, output or inout.
 *
 * @tparam SIZE array size.
 */
template <typename DIRECTION, unsigned int SIZE>
using port_array = typename std::array<port<DIRECTION>, SIZE>;

//--------------------------------------------------------------------------------------------------
// port_array
//--------------------------------------------------------------------------------------------------
/**
 * @ingroup graph_utils
 *
 * @brief port_conditional_array is an helper alias to conditionally instance port array
 * Uses: ``std::conditional`` to instance a ``port_array<DIRECTION, SIZE>`` class or an empty ``no_port`` struct array.
 * @tparam DIRECTION describes port direction: input, output or inout.
 * @tparam Condition when met port of a DIRECTION is created, otherwise a no_port struct is instanced.
 * @tparam SIZE array size.
 */
template <typename DIRECTION, bool Condition, unsigned int SIZE>
using port_conditional_array =
    typename std::conditional<Condition, port_array<DIRECTION, SIZE>, std::array<no_port, SIZE> >::type;
}
}
} // namespace braces
#endif //_TEST_HARNESS_GRAPH_UTILS_HPP_
