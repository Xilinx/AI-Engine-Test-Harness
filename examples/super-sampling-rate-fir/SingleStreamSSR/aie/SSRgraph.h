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

#include <adf.h>
#include "system_settings.h"
#include "aie_kernels.h"
#include "vck190_test_harness_graph.hpp"

std::vector<cint16> taps = std::vector<cint16>(
    {{-82, -253}, {0, -204},    {11, -35},    {-198, 273},  {-642, 467},  {-1026, 333},  {-927, 0},     {-226, -73},
     {643, 467},  {984, 1355},  {550, 1691},  {0, 647},     {538, -1656}, {2860, -3936}, {6313, -4587}, {9113, -2961},
     {9582, 0},   {7421, 2411}, {3936, 2860}, {1023, 1409}, {-200, -615}, {0, -1778},    {517, -1592},  {467, -643},
     {-192, 140}, {-882, 287},  {-1079, 0},   {-755, -245}, {-273, -198}, {22, 30},      {63, 194},     {0, 266}});

std::vector<cint16> taps_aie(taps.rbegin(), taps.rend());

#define GetPhase(Start, Step)                                                                            \
    {                                                                                                    \
        taps_aie[Start], taps_aie[Start + Step], taps_aie[Start + 2 * Step], taps_aie[Start + 3 * Step], \
            taps_aie[Start + 4 * Step], taps_aie[Start + 5 * Step], taps_aie[Start + 6 * Step],          \
            taps_aie[Start + 7 * Step]                                                                   \
    }

std::vector<cint16> taps4_p0 = std::vector<cint16>(GetPhase(0, 4));

std::vector<cint16> taps4_p1 = std::vector<cint16>(GetPhase(1, 4));

std::vector<cint16> taps4_p2 = std::vector<cint16>(GetPhase(2, 4));

std::vector<cint16> taps4_p3 = std::vector<cint16>(GetPhase(3, 4));

const int SHIFT = 0; // to analyze the output generated by impulses at the input
                     // const int SHIFT = 15; // for realistic input samples

using namespace adf;

#define FirstCol 25
#define LastCol (FirstCol + 3)

class FIRGraph_SSR4 : public adf::graph {
   private:
    kernel k[4][4];

   public:
    input_port in[4];
    output_port out[4];

    FIRGraph_SSR4() {
        // k[N][0] is always the first in the cascade stream
        // Topology of the TopGraph
        //
        //      3,3   3,2   3,1   3,0 <--
        //  --> 2,0   2,1   2,2   2,3
        //      1,3   1,2   1,1   1,0 <--
        //  --> 0,0   0,1   0,2   0,3
        // The Upper-Left triangle must discard a sample (diagonal not included)
        //      (1)   (1)   (1)   (0) <--
        //  --> (1)   (1)   (0)   (0)
        //      (1)   (0)   (0)   (0) <--
        //  --> (0)   (0)   (0)   (0)

        k[0][0] = kernel::create_object<SingleStream::FIR_MultiKernel_cout<NUM_SAMPLES, SHIFT> >(taps4_p0, 0);
        k[0][1] = kernel::create_object<SingleStream::FIR_MultiKernel_cincout<NUM_SAMPLES, SHIFT> >(taps4_p1, 0);
        k[0][2] = kernel::create_object<SingleStream::FIR_MultiKernel_cincout<NUM_SAMPLES, SHIFT> >(taps4_p2, 0);
        k[0][3] = kernel::create_object<SingleStream::FIR_MultiKernel_cin<NUM_SAMPLES, SHIFT> >(taps4_p3, 0);

        k[1][0] = kernel::create_object<SingleStream::FIR_MultiKernel_cout<NUM_SAMPLES, SHIFT> >(taps4_p2, 0);
        k[1][1] = kernel::create_object<SingleStream::FIR_MultiKernel_cincout<NUM_SAMPLES, SHIFT> >(taps4_p1, 0);
        k[1][2] = kernel::create_object<SingleStream::FIR_MultiKernel_cincout<NUM_SAMPLES, SHIFT> >(taps4_p0, 0);
        k[1][3] = kernel::create_object<SingleStream::FIR_MultiKernel_cin<NUM_SAMPLES, SHIFT> >(taps4_p3, 0);

        k[2][0] = kernel::create_object<SingleStream::FIR_MultiKernel_cout<NUM_SAMPLES, SHIFT> >(taps4_p2, 0);
        k[2][1] = kernel::create_object<SingleStream::FIR_MultiKernel_cincout<NUM_SAMPLES, SHIFT> >(taps4_p3, 0);
        k[2][2] = kernel::create_object<SingleStream::FIR_MultiKernel_cincout<NUM_SAMPLES, SHIFT> >(taps4_p0, 0);
        k[2][3] = kernel::create_object<SingleStream::FIR_MultiKernel_cin<NUM_SAMPLES, SHIFT> >(taps4_p1, 0);

        k[3][0] = kernel::create_object<SingleStream::FIR_MultiKernel_cout<NUM_SAMPLES, SHIFT> >(taps4_p0, 0);
        k[3][1] = kernel::create_object<SingleStream::FIR_MultiKernel_cincout<NUM_SAMPLES, SHIFT> >(taps4_p3, 0);
        k[3][2] = kernel::create_object<SingleStream::FIR_MultiKernel_cincout<NUM_SAMPLES, SHIFT> >(taps4_p2, 0);
        k[3][3] = kernel::create_object<SingleStream::FIR_MultiKernel_cin<NUM_SAMPLES, SHIFT> >(taps4_p1, 0);

        const int NPhases = 4;

        for (int i = 0; i < NPhases; i++)
            for (int j = 0; j < NPhases; j++) {
                runtime<ratio>(k[i][j]) = 0.9;
                source(k[i][j]) = "aie_kernels/FirSingleStream.cpp";
                headers(k[i][j]) = {"aie_kernels/FirSingleStream.h"};
            }

        // Constraints: location of the first kernel in the cascade
        for (int i = 0; i < NPhases; i++) {
            int j = (i % 2 ? LastCol : FirstCol); // 25 on even rows and 28 on odd rows
            location<kernel>(k[i][0]) = tile(j, i);
        }

        // Cascade Connections
        for (int row = 0; row < NPhases; row++) {
            for (int i = 0; i < NPhases - 1; i++) connect<cascade>(k[row][i].out[0], k[row][i + 1].in[1]);
            connect<stream>(k[row][NPhases - 1].out[0], out[row]);
        }

        // Input Streams connections
        for (int row = 0; row < NPhases; row++)
            for (int col = 0; col < NPhases; col++) {
                int col1 = (row % 2 ? NPhases - col - 1 : col); // kernel col is inverted on odd rows
                int fiforow = row;                              // Each Kernel is served by an independent FIFO

                connect<stream> n0(in[col], k[row][col1].in[0]);
                fifo_depth(n0) = 512;
                location<fifo>(n0) = dma_fifo(aie_tile, FirstCol + col, fiforow, 0x0000, 512);
            }
    };
};

class TopGraph : public adf::graph {
   public:
    FIRGraph_SSR4 G1;

    input_plio in[4];
    output_plio out[4];

    TopGraph() {
        in[0] = input_plio::create("Column_12_TO_AIE", plio_128_bits, "data/PhaseIn_0.txt", 500);
        in[1] = input_plio::create("Column_13_TO_AIE", plio_128_bits, "data/PhaseIn_1.txt", 500);
        in[2] = input_plio::create("Column_14_TO_AIE", plio_128_bits, "data/PhaseIn_2.txt", 500);
        in[3] = input_plio::create("Column_15_TO_AIE", plio_128_bits, "data/PhaseIn_3.txt", 500);
        out[0] = output_plio::create("Column_28_FROM_AIE", plio_128_bits, "data/PhaseOut_0.txt", 500);
        out[1] = output_plio::create("Column_29_FROM_AIE", plio_128_bits, "data/PhaseOut_1.txt", 500);
        out[2] = output_plio::create("Column_30_FROM_AIE", plio_128_bits, "data/PhaseOut_2.txt", 500);
        out[3] = output_plio::create("Column_31_FROM_AIE", plio_128_bits, "data/PhaseOut_3.txt", 500);
        for (int i = 0; i < 4; i++) {
            connect<>(in[i].out[0], G1.in[i]);
            connect<>(G1.out[i], out[i].in[0]);
        }
    }
};
