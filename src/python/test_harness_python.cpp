/*
 * MIT License
 *
 * Copyright (C) 2023-2025 Advanced Micro Devices, Inc.
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
 
#include "test_harness_mgr_client.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <cstdint>
#include <vector>
#include <string>

namespace py = pybind11;

template<typename T>
class test_harness_args: public test_harness::test_harness_args {
    public:
        test_harness_args(test_harness::channel_index channel, uint64_t size_in_bytes, uint64_t repetition, uint64_t delay, py::array_t<T> data)
            : test_harness::test_harness_args(channel, size_in_bytes, repetition, delay, (char*)data.mutable_data()) {
                size_t size = data.size() * sizeof(T);
                if(size != size_in_bytes) {
                    std::cout << "size_in_bytes: " << size_in_bytes << " and data size is " << size << std::endl;
                    throw std::runtime_error("Size of data array does not match size_in_bytes");
                }
            }
};

PYBIND11_MODULE(test_harness_python, m) {
    py::class_<test_harness::test_harness_mgr_client>(m, "test_harness_python")
        .def(py::init<const std::string &, 
                const std::vector<std::string>&, 
                const std::string&, const uint64_t, const unsigned int&>())
        .def("runTestHarness", &test_harness::test_harness_mgr_client::runTestHarness)
        .def("waitForRes", &test_harness::test_harness_mgr_client::waitForRes)
        .def("isResultValid", &test_harness::test_harness_mgr_client::isResultValid)
        .def("printPerf", &test_harness::test_harness_mgr_client::printPerf)
        .def("runAIEGraph", &test_harness::test_harness_mgr_client::runAIEGraph)
        .def("getDelayBetween", &test_harness::test_harness_mgr_client::getDelayBetween);

    py::class_<test_harness::test_harness_args>(m, "test_harness_args")
        .def(py::init<test_harness::channel_index, uint64_t, uint64_t, uint64_t, char*>());

    py::class_<test_harness_args<int64_t>, test_harness::test_harness_args>(m, "test_harness_args_int64_t")
        .def(py::init<test_harness::channel_index, uint64_t, uint64_t, uint64_t, py::array_t<int64_t>>());

    py::class_<test_harness_args<int32_t>, test_harness::test_harness_args>(m, "test_harness_args_int32_t")
        .def(py::init<test_harness::channel_index, uint64_t, uint64_t, uint64_t, py::array_t<int32_t>>());

    py::class_<test_harness_args<int16_t>, test_harness::test_harness_args>(m, "test_harness_args_int16_t")
        .def(py::init<test_harness::channel_index, uint64_t, uint64_t, uint64_t, py::array_t<int16_t>>());

    py::class_<test_harness_args<int8_t>, test_harness::test_harness_args>(m, "test_harness_args_int8_t")
        .def(py::init<test_harness::channel_index, uint64_t, uint64_t, uint64_t, py::array_t<int8_t>>());

    py::class_<test_harness_args<uint64_t>, test_harness::test_harness_args>(m, "test_harness_args_uint64_t")
        .def(py::init<test_harness::channel_index, uint64_t, uint64_t, uint64_t, py::array_t<uint64_t>>());

    py::class_<test_harness_args<uint32_t>, test_harness::test_harness_args>(m, "test_harness_args_uint32_t")
        .def(py::init<test_harness::channel_index, uint64_t, uint64_t, uint64_t, py::array_t<uint32_t>>());

    py::class_<test_harness_args<uint16_t>, test_harness::test_harness_args>(m, "test_harness_args_uint16_t")
        .def(py::init<test_harness::channel_index, uint64_t, uint64_t, uint64_t, py::array_t<uint16_t>>());

    py::class_<test_harness_args<uint8_t>, test_harness::test_harness_args>(m, "test_harness_args_uint8_t")
        .def(py::init<test_harness::channel_index, uint64_t, uint64_t, uint64_t, py::array_t<uint8_t>>());

    py::class_<test_harness_args<double>, test_harness::test_harness_args>(m, "test_harness_args_double")
        .def(py::init<test_harness::channel_index, uint64_t, uint64_t, uint64_t, py::array_t<double>>());

    py::class_<test_harness_args<float>, test_harness::test_harness_args>(m, "test_harness_args_float")
        .def(py::init<test_harness::channel_index, uint64_t, uint64_t, uint64_t, py::array_t<float>>());

    py::enum_<test_harness::channel_index>(m, "channel_index")
        .value("PLIO_01_TO_AIE", test_harness::channel_index::PLIO_01_TO_AIE)
        .value("PLIO_02_TO_AIE", test_harness::channel_index::PLIO_02_TO_AIE)
        .value("PLIO_03_TO_AIE", test_harness::channel_index::PLIO_03_TO_AIE)
        .value("PLIO_04_TO_AIE", test_harness::channel_index::PLIO_04_TO_AIE)
        .value("PLIO_05_TO_AIE", test_harness::channel_index::PLIO_05_TO_AIE)
        .value("PLIO_06_TO_AIE", test_harness::channel_index::PLIO_06_TO_AIE)
        .value("PLIO_07_TO_AIE", test_harness::channel_index::PLIO_07_TO_AIE)
        .value("PLIO_08_TO_AIE", test_harness::channel_index::PLIO_08_TO_AIE)
        .value("PLIO_09_TO_AIE", test_harness::channel_index::PLIO_09_TO_AIE)
        .value("PLIO_10_TO_AIE", test_harness::channel_index::PLIO_10_TO_AIE)
        .value("PLIO_11_TO_AIE", test_harness::channel_index::PLIO_11_TO_AIE)
        .value("PLIO_12_TO_AIE", test_harness::channel_index::PLIO_12_TO_AIE)
        .value("PLIO_13_TO_AIE", test_harness::channel_index::PLIO_13_TO_AIE)
        .value("PLIO_14_TO_AIE", test_harness::channel_index::PLIO_14_TO_AIE)
        .value("PLIO_15_TO_AIE", test_harness::channel_index::PLIO_15_TO_AIE)
        .value("PLIO_16_TO_AIE", test_harness::channel_index::PLIO_16_TO_AIE)
        .value("PLIO_17_TO_AIE", test_harness::channel_index::PLIO_17_TO_AIE)
        .value("PLIO_18_TO_AIE", test_harness::channel_index::PLIO_18_TO_AIE)
        .value("PLIO_19_TO_AIE", test_harness::channel_index::PLIO_19_TO_AIE)
        .value("PLIO_20_TO_AIE", test_harness::channel_index::PLIO_20_TO_AIE)
        .value("PLIO_21_TO_AIE", test_harness::channel_index::PLIO_21_TO_AIE)
        .value("PLIO_22_TO_AIE", test_harness::channel_index::PLIO_22_TO_AIE)
        .value("PLIO_23_TO_AIE", test_harness::channel_index::PLIO_23_TO_AIE)
        .value("PLIO_24_TO_AIE", test_harness::channel_index::PLIO_24_TO_AIE)
        .value("PLIO_25_TO_AIE", test_harness::channel_index::PLIO_25_TO_AIE)
        .value("PLIO_26_TO_AIE", test_harness::channel_index::PLIO_26_TO_AIE)
        .value("PLIO_27_TO_AIE", test_harness::channel_index::PLIO_27_TO_AIE)
        .value("PLIO_28_TO_AIE", test_harness::channel_index::PLIO_28_TO_AIE)
        .value("PLIO_29_TO_AIE", test_harness::channel_index::PLIO_29_TO_AIE)
        .value("PLIO_30_TO_AIE", test_harness::channel_index::PLIO_30_TO_AIE)
        .value("PLIO_31_TO_AIE", test_harness::channel_index::PLIO_31_TO_AIE)
        .value("PLIO_32_TO_AIE", test_harness::channel_index::PLIO_32_TO_AIE)
        .value("PLIO_33_TO_AIE", test_harness::channel_index::PLIO_33_TO_AIE)
        .value("PLIO_34_TO_AIE", test_harness::channel_index::PLIO_34_TO_AIE)
        .value("PLIO_35_TO_AIE", test_harness::channel_index::PLIO_35_TO_AIE)
        .value("PLIO_36_TO_AIE", test_harness::channel_index::PLIO_36_TO_AIE)
        .value("PLIO_01_FROM_AIE", test_harness::channel_index::PLIO_01_FROM_AIE)
        .value("PLIO_02_FROM_AIE", test_harness::channel_index::PLIO_02_FROM_AIE)
        .value("PLIO_03_FROM_AIE", test_harness::channel_index::PLIO_03_FROM_AIE)
        .value("PLIO_04_FROM_AIE", test_harness::channel_index::PLIO_04_FROM_AIE)
        .value("PLIO_05_FROM_AIE", test_harness::channel_index::PLIO_05_FROM_AIE)
        .value("PLIO_06_FROM_AIE", test_harness::channel_index::PLIO_06_FROM_AIE)
        .value("PLIO_07_FROM_AIE", test_harness::channel_index::PLIO_07_FROM_AIE)
        .value("PLIO_08_FROM_AIE", test_harness::channel_index::PLIO_08_FROM_AIE)
        .value("PLIO_09_FROM_AIE", test_harness::channel_index::PLIO_09_FROM_AIE)
        .value("PLIO_10_FROM_AIE", test_harness::channel_index::PLIO_10_FROM_AIE)
        .value("PLIO_11_FROM_AIE", test_harness::channel_index::PLIO_11_FROM_AIE)
        .value("PLIO_12_FROM_AIE", test_harness::channel_index::PLIO_12_FROM_AIE)
        .value("PLIO_13_FROM_AIE", test_harness::channel_index::PLIO_13_FROM_AIE)
        .value("PLIO_14_FROM_AIE", test_harness::channel_index::PLIO_14_FROM_AIE)
        .value("PLIO_15_FROM_AIE", test_harness::channel_index::PLIO_15_FROM_AIE)
        .value("PLIO_16_FROM_AIE", test_harness::channel_index::PLIO_16_FROM_AIE)
        .value("PLIO_17_FROM_AIE", test_harness::channel_index::PLIO_17_FROM_AIE)
        .value("PLIO_18_FROM_AIE", test_harness::channel_index::PLIO_18_FROM_AIE)
        .value("PLIO_19_FROM_AIE", test_harness::channel_index::PLIO_19_FROM_AIE)
        .value("PLIO_20_FROM_AIE", test_harness::channel_index::PLIO_20_FROM_AIE)
        .value("PLIO_21_FROM_AIE", test_harness::channel_index::PLIO_21_FROM_AIE)
        .value("PLIO_22_FROM_AIE", test_harness::channel_index::PLIO_22_FROM_AIE)
        .value("PLIO_23_FROM_AIE", test_harness::channel_index::PLIO_23_FROM_AIE)
        .value("PLIO_24_FROM_AIE", test_harness::channel_index::PLIO_24_FROM_AIE)
        .value("PLIO_25_FROM_AIE", test_harness::channel_index::PLIO_25_FROM_AIE)
        .value("PLIO_26_FROM_AIE", test_harness::channel_index::PLIO_26_FROM_AIE)
        .value("PLIO_27_FROM_AIE", test_harness::channel_index::PLIO_27_FROM_AIE)
        .value("PLIO_28_FROM_AIE", test_harness::channel_index::PLIO_28_FROM_AIE)
        .value("PLIO_29_FROM_AIE", test_harness::channel_index::PLIO_29_FROM_AIE)
        .value("PLIO_30_FROM_AIE", test_harness::channel_index::PLIO_30_FROM_AIE)
        .value("PLIO_31_FROM_AIE", test_harness::channel_index::PLIO_31_FROM_AIE)
        .value("PLIO_32_FROM_AIE", test_harness::channel_index::PLIO_32_FROM_AIE)
        .value("PLIO_33_FROM_AIE", test_harness::channel_index::PLIO_33_FROM_AIE)
        .value("PLIO_34_FROM_AIE", test_harness::channel_index::PLIO_34_FROM_AIE)
        .value("PLIO_35_FROM_AIE", test_harness::channel_index::PLIO_35_FROM_AIE)
        .value("PLIO_36_FROM_AIE", test_harness::channel_index::PLIO_36_FROM_AIE);


    py::enum_<test_harness::TestMode>(m, "TestMode")
        .value("FUNC_MODE", test_harness::TestMode::FUNC_MODE)
        .value("PERF_MODE", test_harness::TestMode::PERF_MODE);


} // namespace test_harness