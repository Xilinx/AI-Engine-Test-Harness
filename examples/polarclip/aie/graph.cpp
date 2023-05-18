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

#include "graph.h"

using namespace adf;

clipped clipgraph; // A graph object 'clipgraph' is declared

// REQUIRED: Instantiate a graph to occupy the PLIOs unused by the user graph.
static std::vector<std::string> usedInputs = {"Column_12_TO_AIE"};
static std::vector<std::string> usedOutputs = {"Column_28_FROM_AIE"};
vck190_test_harness::occupyUnusedPLIO<1, 1> unusedPLIOs(usedInputs, usedOutputs);

// This main() function runs only for AIESIM and X86Sim targets.
// Emulation uses a different host code
#if defined(__AIESIM__) || defined(__X86SIM__)
int main(int argc, char** argv) {
    clipgraph.init(); // Loads the graph to the AI Engine Array
    clipgraph.run(4); // Starts the graph execution by enabling the processors.
    clipgraph.end();  // Wait for 4 iterations to finish
    return 0;
}
#endif
