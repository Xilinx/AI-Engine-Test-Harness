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

#ifndef _VCK190_TEST_HARNESS_GRAPH_PORT_NAME_HPP_
#define _VCK190_TEST_HARNESS_GRAPH_PORT_NAME_HPP_

#include <vector>
#include <string>

static std::vector<std::string> used_pl_in_name;
static std::vector<std::string> used_pl_out_name;

namespace vck190_test_harness {

static std::vector<std::string> in_names = {
    "Column_12_TO_AIE", "Column_13_TO_AIE", "Column_14_TO_AIE", "Column_15_TO_AIE",
    "Column_16_TO_AIE", "Column_17_TO_AIE", "Column_18_TO_AIE", "Column_19_TO_AIE",
    "Column_20_TO_AIE", "Column_21_TO_AIE", "Column_22_TO_AIE", "Column_23_TO_AIE",
    "Column_24_TO_AIE", "Column_25_TO_AIE", "Column_26_TO_AIE", "Column_27_TO_AIE"};
static std::vector<std::string> out_names = {
    "Column_28_FROM_AIE", "Column_29_FROM_AIE", "Column_30_FROM_AIE", "Column_31_FROM_AIE",
    "Column_32_FROM_AIE", "Column_33_FROM_AIE", "Column_34_FROM_AIE", "Column_35_FROM_AIE",
    "Column_36_FROM_AIE", "Column_37_FROM_AIE", "Column_38_FROM_AIE", "Column_39_FROM_AIE",
    "Column_40_FROM_AIE", "Column_41_FROM_AIE", "Column_42_FROM_AIE", "Column_43_FROM_AIE"};

static void register_input_plio(std::string s) {
    used_pl_in_name.push_back(s);
}

static void register_output_plio(std::string s) {
    used_pl_out_name.push_back(s);
}
}
#endif
