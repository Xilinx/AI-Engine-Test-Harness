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

template <int used_in_plio, int used_out_plio>
class occupyUnusedPLIO : public graph {
   public:
    kernel k_in[16 - used_in_plio];
    kernel k_out[16 - used_out_plio];
    input_plio pl_in[16 - used_in_plio];
    output_plio pl_out[16 - used_out_plio];

    occupyUnusedPLIO(std::vector<std::string> used_in_plio_names, std::vector<std::string> used_out_plio_names) {
        {
            int k = 0;
            for (int i = 0; i < 16; i++) {
                if (std::count(used_in_plio_names.begin(), used_in_plio_names.end(),
                               vck190_test_harness::in_names[i]) == 0) {
                    pl_in[k] =
                        input_plio::create(vck190_test_harness::in_names[i], adf::plio_128_bits, "data/dummy.txt", 250);
                    k_in[k] = kernel::create(dummy_in);
                    source(k_in[k]) = "dummy_kernel.cc";
                    runtime<ratio>(k_in[k]) = 0.01;
                    connect<stream> net(pl_in[k].out[0], k_in[k].in[0]);
                    k++;
                }
            }
        }
        {
            int k = 0;
            for (int i = 0; i < 16; i++) {
                if (std::count(used_out_plio_names.begin(), used_out_plio_names.end(),
                               vck190_test_harness::out_names[i]) == 0) {
                    pl_out[k] = output_plio::create(vck190_test_harness::out_names[i], adf::plio_128_bits,
                                                    "data/dummy.txt", 250);
                    k_out[k] = kernel::create(dummy_out);
                    source(k_out[k]) = "dummy_kernel.cc";
                    runtime<ratio>(k_out[k]) = 0.01;
                    connect<stream> net(k_out[k].out[0], pl_out[k].in[0]);
                    k++;
                }
            }
        }
    }
};
}

#endif
