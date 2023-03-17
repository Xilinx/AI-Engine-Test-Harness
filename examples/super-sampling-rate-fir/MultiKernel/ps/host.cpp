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

int main(int argc, char** argv) {
    // Prepare data
    const int num = DATA_SIZE;
    const int size_in_byte = DATA_SIZE * sizeof(int);
    int dataInput[num], dataOutput[num], golden[num];
    for (int i = 0; i < num; i++) {
        dataInput[i] = rand() % 100;
        dataOutput[i] = 0;
        golden[i] = dataInput[i] * 2;
        std::cout << "Input: " << dataInput[i] << ", Output: " << dataOutput[i] << std::endl;
    }

    // run test with test harness
    test_harness_mgr mgr(0, "krnl_adder.xclbin", {"addergraph"});
    std::vector<test_harness_args> args;
    for (int i = 0; i < 12; i++) {
        args.push_back({channel_index(Column_12_TO_AIE + i), num * sizeof(int), 4, 25, (char*)dataInput});
        args.push_back({channel_index(Column_28_FROM_AIE + i), num * sizeof(int), 4, 25, (char*)dataOutput});
    }
    mgr.runTestHarness(args);
    mgr.runGraph(0, 1);
    mgr.waitForRes(10000);

    // Verify
    bool pass = true;
    for (int i = 0; i < num; i++) {
        if (dataOutput[i] != golden[i]) {
            pass = false;
        }
        std::cout << "Output = " << dataOutput[i] << ", Golden = " << golden[i] << std::endl;
    }

    if (pass) {
        std::cout << "TEST PASSED, RC=0" << std::endl;
    } else {
        std::cout << "TEST FAILED" << std::endl;
    }
}
