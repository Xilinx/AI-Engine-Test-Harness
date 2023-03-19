/*
 * Copyright 2022 Xilinx, Inc.
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
 */

#ifndef _VCK190_TEST_HARNESS_GRAPH_HPP_
#define _VCK190_TEST_HARNESS_GRAPH_HPP_

#include <vector>
#include <string>
#include <adf.h>
#include <adf/stream/types.h>
#include "vck190_test_harness_port_name.hpp"

using namespace adf;
using namespace std;

void dummy_out(output_stream_int32* out);
void dummy_in(input_stream_int32* in);

namespace vck190_test_harness {

class graphUnusedPLIO : public graph {
   public:
    std::vector<kernel> k_in;
    std::vector<kernel> k_out;
    std::vector<input_plio> pl_in;
    std::vector<output_plio> pl_out;

    graphUnusedPLIO() {
        std::vector<std::string> unused_pl_in_name(0);
        std::vector<std::string> unused_pl_out_name(0);
        for (int i = 0; i < 16; i++) {
            if (std::count(used_pl_in_name.begin(), used_pl_in_name.end(), vck190_test_harness::in_names[i]) == 0) {
                unused_pl_in_name.push_back(vck190_test_harness::in_names[i]);
            }
            if (std::count(used_pl_out_name.begin(), used_pl_out_name.end(), vck190_test_harness::out_names[i]) == 0) {
                unused_pl_out_name.push_back(vck190_test_harness::out_names[i]);
            }
        }
        k_in.resize(unused_pl_in_name.size());
        k_out.resize(unused_pl_out_name.size());
        pl_in.resize(unused_pl_in_name.size());
        pl_out.resize(unused_pl_out_name.size());

        for (int i = 0; i < pl_in.size(); i++) {
            pl_in[i] = input_plio::create(unused_pl_in_name[i], adf::plio_128_bits, "data/dummy.txt", 250);
            k_in[i] = kernel::create(dummy_in);
            source(k_in[i]) = "dummy_kernel.cc";
            runtime<ratio>(k_in[i]) = 0.01;
            connect<stream> net(pl_in[i].out[0], k_in[i].in[0]);
        }

        for (int i = 0; i < pl_out.size(); i++) {
            pl_out[i] = output_plio::create(unused_pl_out_name[i], adf::plio_128_bits, "data/dummy.txt", 250);
            k_out[i] = kernel::create(dummy_out);
            source(k_out[i]) = "dummy_kernel.cc";
            runtime<ratio>(k_out[i]) = 0.01;
            connect<stream> net(k_out[i].out[0], pl_out[i].in[0]);
        }
    }
};
}

#endif
