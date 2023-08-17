/*
 * MIT License
 *
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated
 * documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Advanced Micro Devices, Inc.
 * shall not be used in advertising or
 * otherwise to promote the sale, use or other dealings in this Software without
 * prior written authorization from
 * Advanced Micro Devices, Inc.
 */

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "test_harness_mgr.hpp"

#include "utils.hpp" // Utilities (read_data_from_file, check_size...)

#define CHANNELIZER_TOLERANCE 5

typedef std::complex<int16_t> cint16_t;

using namespace test_harness;
using namespace std;

int main(int argc, char** argv) {
    // Prepare data
    const int in_ch = 16;
    const int out_ch = 16;
    int in_sz = 4096 * 16;
    int out_sz = 4096 * 16;

    std::vector<cint16_t> in_dft_0;
    std::vector<cint16_t> in_dft_1;
    std::vector<cint16_t> in_dft_2;
    std::vector<cint16_t> in_dft_3;
    std::vector<cint16_t> in_dft_4;
    std::vector<cint16_t> in_dft_5;
    std::vector<cint16_t> in_dft_6;
    std::vector<cint16_t> in_dft_7;
    std::vector<cint16_t> out_dft_0;
    std::vector<cint16_t> out_dft_1;
    std::vector<cint16_t> out_dft_2;
    std::vector<cint16_t> out_dft_3;
    std::vector<cint16_t> out_dft_4;
    std::vector<cint16_t> out_dft_5;
    std::vector<cint16_t> out_dft_6;
    std::vector<cint16_t> out_dft_7;
    std::vector<cint16_t> golden_dft_0;
    std::vector<cint16_t> golden_dft_1;
    std::vector<cint16_t> golden_dft_2;
    std::vector<cint16_t> golden_dft_3;
    std::vector<cint16_t> golden_dft_4;
    std::vector<cint16_t> golden_dft_5;
    std::vector<cint16_t> golden_dft_6;
    std::vector<cint16_t> golden_dft_7;
    std::vector<cint16_t> in_fir_0;
    std::vector<cint16_t> in_fir_1;
    std::vector<cint16_t> in_fir_2;
    std::vector<cint16_t> in_fir_3;
    std::vector<cint16_t> in_fir_4;
    std::vector<cint16_t> in_fir_5;
    std::vector<cint16_t> in_fir_6;
    std::vector<cint16_t> in_fir_7;
    std::vector<cint16_t> out_fir_0;
    std::vector<cint16_t> out_fir_1;
    std::vector<cint16_t> out_fir_2;
    std::vector<cint16_t> out_fir_3;
    std::vector<cint16_t> out_fir_4;
    std::vector<cint16_t> out_fir_5;
    std::vector<cint16_t> out_fir_6;
    std::vector<cint16_t> out_fir_7;
    std::vector<cint16_t> golden_fir_0;
    std::vector<cint16_t> golden_fir_1;
    std::vector<cint16_t> golden_fir_2;
    std::vector<cint16_t> golden_fir_3;
    std::vector<cint16_t> golden_fir_4;
    std::vector<cint16_t> golden_fir_5;
    std::vector<cint16_t> golden_fir_6;
    std::vector<cint16_t> golden_fir_7;

    std::string root_path = argv[2];
    std::string in_dft_0_path = root_path + "/dft_i_0.txt";
    std::string in_dft_1_path = root_path + "/dft_i_1.txt";
    std::string in_dft_2_path = root_path + "/dft_i_2.txt";
    std::string in_dft_3_path = root_path + "/dft_i_3.txt";
    std::string in_dft_4_path = root_path + "/dft_i_4.txt";
    std::string in_dft_5_path = root_path + "/dft_i_5.txt";
    std::string in_dft_6_path = root_path + "/dft_i_6.txt";
    std::string in_dft_7_path = root_path + "/dft_i_7.txt";
    std::string out_dft_0_path = root_path + "/dft_o_0.txt";
    std::string out_dft_1_path = root_path + "/dft_o_1.txt";
    std::string out_dft_2_path = root_path + "/dft_o_2.txt";
    std::string out_dft_3_path = root_path + "/dft_o_3.txt";
    std::string out_dft_4_path = root_path + "/dft_o_4.txt";
    std::string out_dft_5_path = root_path + "/dft_o_5.txt";
    std::string out_dft_6_path = root_path + "/dft_o_6.txt";
    std::string out_dft_7_path = root_path + "/dft_o_7.txt";
    std::string in_fir_0_path = root_path + "/fir_i_0.txt";
    std::string in_fir_1_path = root_path + "/fir_i_1.txt";
    std::string in_fir_2_path = root_path + "/fir_i_2.txt";
    std::string in_fir_3_path = root_path + "/fir_i_3.txt";
    std::string in_fir_4_path = root_path + "/fir_i_4.txt";
    std::string in_fir_5_path = root_path + "/fir_i_5.txt";
    std::string in_fir_6_path = root_path + "/fir_i_6.txt";
    std::string in_fir_7_path = root_path + "/fir_i_7.txt";
    std::string out_fir_0_path = root_path + "/fir_o_0.txt";
    std::string out_fir_1_path = root_path + "/fir_o_1.txt";
    std::string out_fir_2_path = root_path + "/fir_o_2.txt";
    std::string out_fir_3_path = root_path + "/fir_o_3.txt";
    std::string out_fir_4_path = root_path + "/fir_o_4.txt";
    std::string out_fir_5_path = root_path + "/fir_o_5.txt";
    std::string out_fir_6_path = root_path + "/fir_o_6.txt";
    std::string out_fir_7_path = root_path + "/fir_o_7.txt";

    read_data_from_file(in_dft_0_path, in_dft_0);
    read_data_from_file(in_dft_1_path, in_dft_1);
    read_data_from_file(in_dft_2_path, in_dft_2);
    read_data_from_file(in_dft_3_path, in_dft_3);
    read_data_from_file(in_dft_4_path, in_dft_4);
    read_data_from_file(in_dft_5_path, in_dft_5);
    read_data_from_file(in_dft_6_path, in_dft_6);
    read_data_from_file(in_dft_7_path, in_dft_7);
    read_data_from_file(out_dft_0_path, golden_dft_0);
    read_data_from_file(out_dft_1_path, golden_dft_1);
    read_data_from_file(out_dft_2_path, golden_dft_2);
    read_data_from_file(out_dft_3_path, golden_dft_3);
    read_data_from_file(out_dft_4_path, golden_dft_4);
    read_data_from_file(out_dft_5_path, golden_dft_5);
    read_data_from_file(out_dft_6_path, golden_dft_6);
    read_data_from_file(out_dft_7_path, golden_dft_7);
    read_data_from_file(in_fir_0_path, in_fir_0);
    read_data_from_file(in_fir_1_path, in_fir_1);
    read_data_from_file(in_fir_2_path, in_fir_2);
    read_data_from_file(in_fir_3_path, in_fir_3);
    read_data_from_file(in_fir_4_path, in_fir_4);
    read_data_from_file(in_fir_5_path, in_fir_5);
    read_data_from_file(in_fir_6_path, in_fir_6);
    read_data_from_file(in_fir_7_path, in_fir_7);
    read_data_from_file(out_fir_0_path, golden_fir_0);
    read_data_from_file(out_fir_1_path, golden_fir_1);
    read_data_from_file(out_fir_2_path, golden_fir_2);
    read_data_from_file(out_fir_3_path, golden_fir_3);
    read_data_from_file(out_fir_4_path, golden_fir_4);
    read_data_from_file(out_fir_5_path, golden_fir_5);
    read_data_from_file(out_fir_6_path, golden_fir_6);
    read_data_from_file(out_fir_7_path, golden_fir_7);
    out_dft_0.resize(golden_dft_0.size());
    out_dft_1.resize(golden_dft_1.size());
    out_dft_2.resize(golden_dft_2.size());
    out_dft_3.resize(golden_dft_3.size());
    out_dft_4.resize(golden_dft_4.size());
    out_dft_5.resize(golden_dft_5.size());
    out_dft_6.resize(golden_dft_6.size());
    out_dft_7.resize(golden_dft_7.size());
    out_fir_0.resize(golden_fir_0.size());
    out_fir_1.resize(golden_fir_1.size());
    out_fir_2.resize(golden_fir_2.size());
    out_fir_3.resize(golden_fir_3.size());
    out_fir_4.resize(golden_fir_4.size());
    out_fir_5.resize(golden_fir_5.size());
    out_fir_6.resize(golden_fir_6.size());
    out_fir_7.resize(golden_fir_7.size());

    // run test with test harness
    test_harness_mgr<36, 16, 4096> mgr(0, argv[1], {"vck190_test_harness_perf"}, {"aie_dut"}, REP_MODE, "vck190");
    std::vector<test_harness_args> args;
    args.push_back({channel_index(PLIO_01_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_dft_0.data()});
    args.push_back({channel_index(PLIO_03_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_dft_1.data()});
    args.push_back({channel_index(PLIO_05_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_dft_2.data()});
    args.push_back({channel_index(PLIO_07_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_dft_3.data()});
    args.push_back({channel_index(PLIO_09_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_dft_4.data()});
    args.push_back({channel_index(PLIO_11_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_dft_5.data()});
    args.push_back({channel_index(PLIO_13_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_dft_6.data()});
    args.push_back({channel_index(PLIO_15_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_dft_7.data()});
    args.push_back({channel_index(PLIO_17_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_fir_0.data()});
    args.push_back({channel_index(PLIO_19_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_fir_1.data()});
    args.push_back({channel_index(PLIO_21_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_fir_2.data()});
    args.push_back({channel_index(PLIO_23_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_fir_3.data()});
    args.push_back({channel_index(PLIO_25_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_fir_4.data()});
    args.push_back({channel_index(PLIO_27_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_fir_5.data()});
    args.push_back({channel_index(PLIO_29_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_fir_6.data()});
    args.push_back({channel_index(PLIO_31_TO_AIE), in_sz, 1, 0, 0, 0, (char*)in_fir_7.data()});
    args.push_back({channel_index(PLIO_02_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_dft_0.data()});
    args.push_back({channel_index(PLIO_04_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_dft_1.data()});
    args.push_back({channel_index(PLIO_06_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_dft_2.data()});
    args.push_back({channel_index(PLIO_08_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_dft_3.data()});
    args.push_back({channel_index(PLIO_10_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_dft_4.data()});
    args.push_back({channel_index(PLIO_12_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_dft_5.data()});
    args.push_back({channel_index(PLIO_14_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_dft_6.data()});
    args.push_back({channel_index(PLIO_16_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_dft_7.data()});
    args.push_back({channel_index(PLIO_18_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_fir_0.data()});
    args.push_back({channel_index(PLIO_20_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_fir_1.data()});
    args.push_back({channel_index(PLIO_22_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_fir_2.data()});
    args.push_back({channel_index(PLIO_24_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_fir_3.data()});
    args.push_back({channel_index(PLIO_26_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_fir_4.data()});
    args.push_back({channel_index(PLIO_28_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_fir_5.data()});
    args.push_back({channel_index(PLIO_30_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_fir_6.data()});
    args.push_back({channel_index(PLIO_32_FROM_AIE), out_sz, 1, 0, 0, 0, (char*)out_fir_7.data()});
    mgr.runAIEGraph(0, 2);
    mgr.runTestHarness(args);
    mgr.waitForRes(10000);

    int NUM_SAMPLES = out_sz / (sizeof(int16_t) * 2); // complex type
    int errorCount = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        int16_t err_re_dft_0 = abs(real(golden_dft_0[i]) - real(out_dft_0[i]));
        int16_t err_im_dft_0 = abs(imag(golden_dft_0[i]) - imag(out_dft_0[i]));
        int16_t err_re_dft_1 = abs(real(golden_dft_1[i]) - real(out_dft_1[i]));
        int16_t err_im_dft_1 = abs(imag(golden_dft_1[i]) - imag(out_dft_1[i]));
        int16_t err_re_dft_2 = abs(real(golden_dft_2[i]) - real(out_dft_2[i]));
        int16_t err_im_dft_2 = abs(imag(golden_dft_2[i]) - imag(out_dft_2[i]));
        int16_t err_re_dft_3 = abs(real(golden_dft_3[i]) - real(out_dft_3[i]));
        int16_t err_im_dft_3 = abs(imag(golden_dft_3[i]) - imag(out_dft_3[i]));
        int16_t err_re_dft_4 = abs(real(golden_dft_4[i]) - real(out_dft_4[i]));
        int16_t err_im_dft_4 = abs(imag(golden_dft_4[i]) - imag(out_dft_4[i]));
        int16_t err_re_dft_5 = abs(real(golden_dft_5[i]) - real(out_dft_5[i]));
        int16_t err_im_dft_5 = abs(imag(golden_dft_5[i]) - imag(out_dft_5[i]));
        int16_t err_re_dft_6 = abs(real(golden_dft_6[i]) - real(out_dft_6[i]));
        int16_t err_im_dft_6 = abs(imag(golden_dft_6[i]) - imag(out_dft_6[i]));
        int16_t err_re_dft_7 = abs(real(golden_dft_7[i]) - real(out_dft_7[i]));
        int16_t err_im_dft_7 = abs(imag(golden_dft_7[i]) - imag(out_dft_7[i]));
        int16_t err_re_fir_0 = abs(real(golden_fir_0[i]) - real(out_fir_0[i]));
        int16_t err_im_fir_0 = abs(imag(golden_fir_0[i]) - imag(out_fir_0[i]));
        int16_t err_re_fir_1 = abs(real(golden_fir_1[i]) - real(out_fir_1[i]));
        int16_t err_im_fir_1 = abs(imag(golden_fir_1[i]) - imag(out_fir_1[i]));
        int16_t err_re_fir_2 = abs(real(golden_fir_2[i]) - real(out_fir_2[i]));
        int16_t err_im_fir_2 = abs(imag(golden_fir_2[i]) - imag(out_fir_2[i]));
        int16_t err_re_fir_3 = abs(real(golden_fir_3[i]) - real(out_fir_3[i]));
        int16_t err_im_fir_3 = abs(imag(golden_fir_3[i]) - imag(out_fir_3[i]));
        int16_t err_re_fir_4 = abs(real(golden_fir_4[i]) - real(out_fir_4[i]));
        int16_t err_im_fir_4 = abs(imag(golden_fir_4[i]) - imag(out_fir_4[i]));
        int16_t err_re_fir_5 = abs(real(golden_fir_5[i]) - real(out_fir_5[i]));
        int16_t err_im_fir_5 = abs(imag(golden_fir_5[i]) - imag(out_fir_5[i]));
        int16_t err_re_fir_6 = abs(real(golden_fir_6[i]) - real(out_fir_6[i]));
        int16_t err_im_fir_6 = abs(imag(golden_fir_6[i]) - imag(out_fir_6[i]));
        int16_t err_re_fir_7 = abs(real(golden_fir_7[i]) - real(out_fir_7[i]));
        int16_t err_im_fir_7 = abs(imag(golden_fir_7[i]) - imag(out_fir_7[i]));
        if ((err_re_dft_0 > CHANNELIZER_TOLERANCE) || (err_im_dft_0 > CHANNELIZER_TOLERANCE) || (err_re_dft_1 > CHANNELIZER_TOLERANCE) ||
            (err_im_dft_1 > CHANNELIZER_TOLERANCE) || (err_re_dft_2 > CHANNELIZER_TOLERANCE) || (err_im_dft_2 > CHANNELIZER_TOLERANCE) ||
            (err_re_dft_3 > CHANNELIZER_TOLERANCE) || (err_im_dft_3 > CHANNELIZER_TOLERANCE) || (err_re_dft_4 > CHANNELIZER_TOLERANCE) ||
            (err_im_dft_4 > CHANNELIZER_TOLERANCE) || (err_re_dft_5 > CHANNELIZER_TOLERANCE) || (err_im_dft_5 > CHANNELIZER_TOLERANCE) ||
            (err_re_dft_6 > CHANNELIZER_TOLERANCE) || (err_im_dft_6 > CHANNELIZER_TOLERANCE) || (err_re_dft_7 > CHANNELIZER_TOLERANCE) ||
            (err_im_dft_7 > CHANNELIZER_TOLERANCE) || (err_re_fir_0 > CHANNELIZER_TOLERANCE) || (err_im_fir_0 > CHANNELIZER_TOLERANCE) ||
            (err_re_fir_1 > CHANNELIZER_TOLERANCE) || (err_im_fir_1 > CHANNELIZER_TOLERANCE) || (err_re_fir_2 > CHANNELIZER_TOLERANCE) ||
            (err_im_fir_2 > CHANNELIZER_TOLERANCE) || (err_re_fir_3 > CHANNELIZER_TOLERANCE) || (err_im_fir_3 > CHANNELIZER_TOLERANCE) ||
            (err_re_fir_4 > CHANNELIZER_TOLERANCE) || (err_im_fir_4 > CHANNELIZER_TOLERANCE) || (err_re_fir_5 > CHANNELIZER_TOLERANCE) ||
            (err_im_fir_5 > CHANNELIZER_TOLERANCE) || (err_re_fir_6 > CHANNELIZER_TOLERANCE) || (err_im_fir_6 > CHANNELIZER_TOLERANCE) ||
            (err_re_fir_7 > CHANNELIZER_TOLERANCE) || (err_im_fir_7 > CHANNELIZER_TOLERANCE)) {
            errorCount++;
        }
    }
    if (errorCount)
        printf("Test failed with %d errors\n", errorCount);
    else
        printf("TEST PASSED\n");

    return errorCount;
}
