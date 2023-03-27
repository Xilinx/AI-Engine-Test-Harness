/**
* Copyright (C) 2019-2021 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
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
    const int in_ch = 4;
    const int out_ch = 1;
    int in_sz = 512 * 4;
    int out_sz = 512 * 4;
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
        args.push_back({channel_index(Column_12_TO_AIE + i), (in_sz + i * 4 * 8), 1, 0, (char*)in_data[i]});
    }
    for (int j = 0; j < out_ch; j++) {
        args.push_back({channel_index(Column_28_FROM_AIE + j), out_sz, 1, 0, (char*)out_data[j]});
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
