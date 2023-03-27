/*
 * (c) Copyright 2021-2022 Xilinx, Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <adf.h>
#include "system_settings.h"

#include "graph.h"

std::vector<std::string> cust_in = {"Column_12_TO_AIE", "Column_13_TO_AIE", "Column_14_TO_AIE", "Column_15_TO_AIE"};
std::vector<std::string> cust_out = {"Column_28_FROM_AIE"};

TopGraph G;
vck190_test_harness::occupyUnusedPLIO<4, 1> dummyGraph(cust_in, cust_out);

int main() {
    G.init();
    G.run(NFRAMES);
    G.end();

    return (0);
}
