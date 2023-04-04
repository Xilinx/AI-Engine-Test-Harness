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

//#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
//#include <fstream>

#include "vck190_test_harness_mgr.hpp"

#define DATA_SIZE 16

using namespace vck190_test_harness;

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace std;

void load_input(string fname, vector<int>& data) {
    data.clear();
    fstream file(fname, ios::in);

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream str(line);
            for (int i = 0; i < 4; i++) {
                int tmp;
                str >> tmp;
                data.push_back(tmp);
            }
        }
    }
}

int main(int argc, char** argv) {
    // Prepare data
    const int in_ch = 16;
    const int out_ch = 16;
    int in_sz = 256 * 4;
    int out_sz = 256 * 4;
    char* in_data[in_ch];
    char* out_data[out_ch];

    for (int i = 0; i < in_ch; i++) {
        in_data[i] = (char*)malloc(in_sz);
    }
    for (int i = 0; i < out_ch; i++) {
        out_data[i] = (char*)malloc(out_sz);
    }
    // run test with test harness
    test_harness_mgr mgr(0, argv[1], {"G"});
    std::vector<test_harness_args> args;
    for (int i = 0; i < in_ch; i++) {
        args.push_back({channel_index(Column_12_TO_AIE + i), in_sz, 8, 0, (char*)in_data[0]});
    }
    for (int i = 0; i < out_ch; i++) {
        args.push_back({channel_index(Column_28_FROM_AIE + i), out_sz, 1, 0, (char*)out_data[0]});
    }
    mgr.runGraph(0, 1);
    mgr.runTestHarness(args);
    mgr.waitForRes(10000);

    //
    for (int i = 0; i < in_ch; i++) {
        free(in_data[i]);
    }
    for (int i = 0; i < out_ch; i++) {
        free(out_data[i]);
    }

    return 0;
}
