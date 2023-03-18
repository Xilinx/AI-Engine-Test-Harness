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

#include "ocl.hpp"

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
    char* in_data[16];
    int in_size[16];
    char* out_data[16];

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 2; j++) {
            vector<int> raw;
            string fname("data/PhaseIn_" + to_string(i) + "_" + to_string(j) + ".txt");
            load_input(fname, raw);
            std::cout << "data.size = " << raw.size() << std::endl;

            in_size[i * 2 + j] = raw.size();
            in_data[i * 2 + j] = (char*)malloc(vector.size() * 2);
            out_data[i * 2 + j] = (char*)malloc(vector.size() * 2);
            for (int k = 0; k < raw.size(); k++) {
                int tmp = raw[k];
                memcpy(in_data[i * 2 + j] + k * 2, &tmp, 2);
            }
        }
    }

    // run test with test harness
    test_harness_mgr mgr(0, "vck190_test_harness.xclbin", {"G"});
    std::vector<test_harness_args> args;
    for (int i = 0; i < 16; i++) {
        args.push_back({channel_index(Column_12_TO_AIE + i), in_size[i] * 2, 1, 0, (char*)in_data[i]});
        args.push_back({channel_index(Column_28_FROM_AIE + i), out_size[i] * 2, 1, 0, (char*)out_data[i]});
    }
    mgr.runTestHarness(args);
    mgr.runGraph(0, 1);
    mgr.waitForRes(10000);

    //
    for (int i = 0; i < 16; i++) {
        free(in_data);
        free(out_data);
    }

    return 0;
}
