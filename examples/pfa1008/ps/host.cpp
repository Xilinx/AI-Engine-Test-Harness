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


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#include "vck190_test_harness_mgr.hpp"

#include "utils.hpp" // Utilities (read_data_from_file, check_size...)

typedef std::complex<int16_t> cint16_t;

using namespace vck190_test_harness;

using namespace std;

int main(int argc, char** argv) {
    // Prepare data
    int in_sz = 1008 * 16;
    int out_sz = 1008 * 16;

    std::vector<cint16_t> in_dft7_0;
    std::vector<cint16_t> in_dft7_1;
    std::vector<cint16_t> in_dft9_0;
    std::vector<cint16_t> in_dft9_1;
    std::vector<cint16_t> in_dft16_0;
    std::vector<cint16_t> in_dft16_1;
    std::vector<cint16_t> out_dft7_0;
    std::vector<cint16_t> out_dft7_1;
    std::vector<cint16_t> out_dft9_0;
    std::vector<cint16_t> out_dft9_1;
    std::vector<cint16_t> out_dft16_0;
    std::vector<cint16_t> out_dft16_1;
    std::vector<cint16_t> golden_dft7_0;
    std::vector<cint16_t> golden_dft7_1;
    std::vector<cint16_t> golden_dft9_0;
    std::vector<cint16_t> golden_dft9_1;
    std::vector<cint16_t> golden_dft16_0;
    std::vector<cint16_t> golden_dft16_1;

    std::string root_path = argv[2];
    std::string in_dft7_0_path = root_path + "/dft7_i_0.txt";
    std::string in_dft7_1_path = root_path + "/dft7_i_1.txt";
    std::string in_dft9_0_path = root_path + "/dft9_i_0.txt";
    std::string in_dft9_1_path = root_path + "/dft9_i_1.txt";
    std::string in_dft16_0_path = root_path + "/dft16_i_0.txt";
    std::string in_dft16_1_path = root_path + "/dft16_i_1.txt";
    std::string out_dft7_0_path = root_path + "/dft7_o_0.txt";
    std::string out_dft7_1_path = root_path + "/dft7_o_1.txt";
    std::string out_dft9_0_path = root_path + "/dft9_o_0.txt";
    std::string out_dft9_1_path = root_path + "/dft9_o_1.txt";
    std::string out_dft16_0_path = root_path + "/dft16_o_0.txt";
    std::string out_dft16_1_path = root_path + "/dft16_o_1.txt";

    read_data_from_file(in_dft7_0_path, in_dft7_0);
    read_data_from_file(in_dft7_1_path, in_dft7_1);
    read_data_from_file(in_dft9_0_path, in_dft9_0);
    read_data_from_file(in_dft9_1_path, in_dft9_1);
    read_data_from_file(in_dft16_0_path, in_dft16_0);
    read_data_from_file(in_dft16_1_path, in_dft16_1);
    read_data_from_file(out_dft7_0_path, golden_dft7_0);
    read_data_from_file(out_dft7_1_path, golden_dft7_1);
    read_data_from_file(out_dft9_0_path, golden_dft9_0);
    read_data_from_file(out_dft9_1_path, golden_dft9_1);
    read_data_from_file(out_dft16_0_path, golden_dft16_0);
    read_data_from_file(out_dft16_1_path, golden_dft16_1);

    out_dft7_0.resize(golden_dft7_0.size());
    out_dft7_1.resize(golden_dft7_1.size());
    out_dft9_0.resize(golden_dft9_0.size());
    out_dft9_1.resize(golden_dft9_1.size());
    out_dft16_0.resize(golden_dft16_0.size());
    out_dft16_1.resize(golden_dft16_1.size());

    // run test with test harness
    test_harness_mgr mgr(0, argv[1], {"aie_dut"});
    std::vector<test_harness_args> args;
    args.push_back({channel_index(PLIO_01_TO_AIE), in_sz, 1, 0, (char*)in_dft7_0.data()});
    args.push_back({channel_index(PLIO_03_TO_AIE), in_sz, 1, 0, (char*)in_dft9_0.data()});
    args.push_back({channel_index(PLIO_05_TO_AIE), in_sz, 1, 0, (char*)in_dft16_0.data()});
    args.push_back({channel_index(PLIO_07_TO_AIE), in_sz, 1, 0, (char*)in_dft7_1.data()});
    args.push_back({channel_index(PLIO_09_TO_AIE), in_sz, 1, 0, (char*)in_dft9_1.data()});
    args.push_back({channel_index(PLIO_11_TO_AIE), in_sz, 1, 0, (char*)in_dft16_1.data()});

    args.push_back({channel_index(PLIO_02_FROM_AIE), out_sz, 1, 0, (char*)out_dft7_0.data()});
    args.push_back({channel_index(PLIO_04_FROM_AIE), out_sz, 1, 0, (char*)out_dft9_0.data()});
    args.push_back({channel_index(PLIO_06_FROM_AIE), out_sz, 1, 0, (char*)out_dft16_0.data()});
    args.push_back({channel_index(PLIO_08_FROM_AIE), out_sz, 1, 0, (char*)out_dft7_1.data()});
    args.push_back({channel_index(PLIO_10_FROM_AIE), out_sz, 1, 0, (char*)out_dft9_1.data()});
    args.push_back({channel_index(PLIO_12_FROM_AIE), out_sz, 1, 0, (char*)out_dft16_1.data()});

    mgr.runAIEGraph(0, 8);
    mgr.runTestHarness(args);
    mgr.waitForRes(10000);

    int NUM_SAMPLES_O = out_sz/4; // Each sample is 4 bytes (32-bits or cint16)
	int errorCount = 0;
    {
        for (int i = 0; i < NUM_SAMPLES_O; i++) {
			int16_t err_re_dft7_0 = abs(real(golden_dft7_0[i]) - real(out_dft7_0[i]));
			int16_t err_im_dft7_0 = abs(imag(golden_dft7_0[i]) - imag(out_dft7_0[i]));
			int16_t err_re_dft7_1 = abs(real(golden_dft7_1[i]) - real(out_dft7_1[i]));
			int16_t err_im_dft7_1 = abs(imag(golden_dft7_1[i]) - imag(out_dft7_1[i]));
			int16_t err_re_dft9_0 = abs(real(golden_dft9_0[i]) - real(out_dft9_0[i]));
			int16_t err_im_dft9_0 = abs(imag(golden_dft9_0[i]) - imag(out_dft9_0[i]));
			int16_t err_re_dft9_1 = abs(real(golden_dft9_1[i]) - real(out_dft9_1[i]));
			int16_t err_im_dft9_1 = abs(imag(golden_dft9_1[i]) - imag(out_dft9_1[i]));
			int16_t err_re_dft16_0 = abs(real(golden_dft16_0[i]) - real(out_dft16_0[i]));
			int16_t err_im_dft16_0 = abs(imag(golden_dft16_0[i]) - imag(out_dft16_0[i]));
			int16_t err_re_dft16_1 = abs(real(golden_dft16_1[i]) - real(out_dft16_1[i]));
			int16_t err_im_dft16_1 = abs(imag(golden_dft16_1[i]) - imag(out_dft16_1[i]));
            if ( ( err_re_dft7_0 > 5 ) || ( err_im_dft7_0 > 5 ) || ( err_re_dft7_1 > 5 ) || ( err_im_dft7_1 > 5 ) || 
			     ( err_re_dft9_0 > 5 ) || ( err_im_dft9_0 > 5 ) || ( err_re_dft9_1 > 5 ) || ( err_im_dft9_1 > 5 ) || 
				 ( err_re_dft16_0 > 5 ) || ( err_im_dft16_0 > 5 ) || ( err_re_dft16_1 > 5 ) || ( err_im_dft16_1 > 5 ) )  {  // Matlab is not bit accurate
                printf("Error found @ %d\n", i);
                printf("dft7_0 (%d,%d) != (%d,%d)\n", real(out_dft7_0[i]), imag(out_dft7_0[i]), real(golden_dft7_0[i]), imag(golden_dft7_0[i]));
                printf("dft7_1 (%d,%d) != (%d,%d)\n", real(out_dft7_1[i]), imag(out_dft7_1[i]), real(golden_dft7_1[i]), imag(golden_dft7_1[i]));
                printf("dft9_0 (%d,%d) != (%d,%d)\n", real(out_dft9_0[i]), imag(out_dft9_0[i]), real(golden_dft9_0[i]), imag(golden_dft9_0[i]));
                printf("dft9_1 (%d,%d) != (%d,%d)\n", real(out_dft9_1[i]), imag(out_dft9_1[i]), real(golden_dft9_1[i]), imag(golden_dft9_1[i]));
                printf("dft16_0 (%d,%d) != (%d,%d)\n", real(out_dft16_0[i]), imag(out_dft16_0[i]), real(golden_dft16_0[i]), imag(golden_dft16_0[i]));
                printf("dft16_1 (%d,%d) != (%d,%d)\n", real(out_dft16_1[i]), imag(out_dft16_1[i]), real(golden_dft16_1[i]), imag(golden_dft16_1[i]));
                errorCount++;
            }
        }
        if (errorCount)
            printf("Test failed with %d errors\n", errorCount);
        else
            printf("TEST PASSED\n");
    }
    return errorCount;
}
