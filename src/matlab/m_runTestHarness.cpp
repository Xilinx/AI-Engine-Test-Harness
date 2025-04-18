/*
 * MIT License
 *
 * Copyright (C) 2024-2025 Advanced Micro Devices, Inc.
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
 
#include "mex.hpp"
#include "test_harness_mgr_client.hpp"

using namespace matlab;

class MexFunction : public mex::Function {
   private:
    std::shared_ptr<engine::MATLABEngine> matlabPtr;

   public:
    MexFunction() { matlabPtr = getEngine(); }
    void displayError(std::string errorMessage) {
        data::ArrayFactory factory;
        matlabPtr->feval(u"error", 0, std::vector<data::Array>({factory.createScalar(errorMessage)}));
    }

    void operator()(mex::ArgumentList outputs, mex::ArgumentList inputs) {
        data::ArrayFactory factory;

        if (inputs.size() != 4) {
            displayError("Four inputs required.");
        }

        // first argument is an integer
        if (inputs[0].getType() != data::ArrayType::UINT64) {
            displayError("First input must be the test_harness object handle.");
        }

        uintptr_t handle = inputs[0][0];
        auto mgr_client =
            reinterpret_cast<test_harness::test_harness_mgr_client*>(handle);
        std::vector<test_harness::test_harness_args> args;

        // second argument is an integer
        if (inputs[1].getType() != data::ArrayType::UINT8) {
            displayError("First input must be an Integer.");
        }

        uint8_t const mode = inputs[1][0];
        if (mode > 1) {
            displayError("Second input must be 0 for function mode or 1 for performance mode.");
        }
        auto test_mode = static_cast<test_harness::TestMode>(mode);

        // thrid argument is an array of struct
        if (inputs[2].getType() != data::ArrayType::VALUE_OBJECT) {
            displayError("Thrid input must be an array of struct.");
        }

        const data::ObjectArray& inputArray = inputs[2];
        checkStructureElements(inputArray);
        size_t total_num_of_elements = inputArray.getNumberOfElements();

        for (int i = 0; i < total_num_of_elements; i++) {
            auto channel_id = static_cast<test_harness::channel_index>(
                (uint16_t)matlabPtr->getProperty(inputArray, i, u"channel_id")[0]);
            uint64_t size_in_byte = matlabPtr->getProperty(inputArray, i, u"size_in_bytes")[0];
            uint64_t repetitions = matlabPtr->getProperty(inputArray, i, u"repetitions")[0];
            uint64_t delay = matlabPtr->getProperty(inputArray, i, u"delay")[0];
            const data::Array& data = matlabPtr->getProperty(inputArray, i, u"data");
            char* data_ptr = nullptr;
            const auto type = data.getType();
            switch (type) {
                case data::ArrayType::INT8:
                    data_ptr = (char*)getDataPtr<int8_t>(data, size_in_byte);
                    break;
                case data::ArrayType::UINT8:
                    data_ptr = (char*)getDataPtr<uint8_t>(data, size_in_byte);
                    break;
                case data::ArrayType::INT16:
                    data_ptr = (char*)getDataPtr<int16_t>(data, size_in_byte);
                    break;
                case data::ArrayType::UINT16:
                    data_ptr = (char*)getDataPtr<uint16_t>(data, size_in_byte);
                    break;
                case data::ArrayType::INT32:
                    data_ptr = (char*)getDataPtr<int32_t>(data, size_in_byte);
                    break;
                case data::ArrayType::UINT32:
                    data_ptr = (char*)getDataPtr<uint32_t>(data, size_in_byte);
                    break;
                case data::ArrayType::INT64:
                    data_ptr = (char*)getDataPtr<int64_t>(data, size_in_byte);
                    break;
                case data::ArrayType::UINT64:
                    data_ptr = (char*)getDataPtr<uint64_t>(data, size_in_byte);
                    break;
                case data::ArrayType::SINGLE:
                    data_ptr = (char*)getDataPtr<float>(data, size_in_byte);
                    break;
                case data::ArrayType::DOUBLE:
                    data_ptr = (char*)getDataPtr<double>(data, size_in_byte);
                    break;
                default:
                    displayError("Data type not supported.");
                    break;
            }
            args.emplace_back(channel_id, size_in_byte, repetitions, delay, data_ptr);
        }

        // fourth argument is an integer
        if (inputs[3].getType() != data::ArrayType::UINT32) {
            displayError("Fourth input must be an Integer.");
        }
        uint32_t const timeout = inputs[3][0];
        
        mgr_client->runTestHarness(test_mode, args, timeout);
    }

    /* Make sure that the passed structure has valid data. */
    void checkStructureElements(const data::Array& inputArray) {
        data::ArrayFactory factory;
        std::vector<data::Array> args{inputArray, factory.createCharArray("test_harness_args")};
        data::TypedArray<bool> result = matlabPtr->feval(u"isa", args);
        for (auto res : result) {
            if (!res) {
                displayError("Each element of the inputs must be an object of \"test_harness_args\".");
            }
        }
    }

    template <typename T>
    const T* getDataPtr(const data::Array& arr, size_t size_in_byte = 0) {
        const matlab::data::TypedArray<T>& arr_t = arr;
        size_t total_size = arr_t.getNumberOfElements() * sizeof(T);
        if (0 != size_in_byte && size_in_byte != total_size) displayError("Size mismatch.");
        matlab::data::TypedIterator<const T> it(arr_t.begin());
        return it.operator->();
    }
};