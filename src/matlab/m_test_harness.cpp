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
        if (inputs.size() != 5) {
            displayError("Seven inputs required.");
        }

        if (outputs.size() > 1) {
            displayError("Too many outputs specified.");
        }

        int index = -1;
        // first argument is a string
        if (inputs[++index].getType() != data::ArrayType::MATLAB_STRING) {
            displayError("First input must be a string of the path to the xclbin.");
        }
        std::string xclbin_file_path = std::string(inputs[index][0]);

        // second argument is a list of strings
        if (inputs[++index].getType() != data::ArrayType::MATLAB_STRING) {
            displayError("Second input must be a list of strings of the graph names.");
        }
        const data::Array& graphNamesList = inputs[index];
        std::vector<std::string> graphNames;
        for (size_t i = 0; i < graphNamesList.getNumberOfElements(); i++) {
            graphNames.push_back(std::string(graphNamesList[i]));
        }

        // third argument is a strings
        if (inputs[++index].getType() != data::ArrayType::MATLAB_STRING) {
            displayError("third input must be a string of the device name.");
        }
        std::string device_name = std::string(inputs[index][0]);
        
        // fourth argument is an integer
        if (inputs[++index].getType() != data::ArrayType::UINT32) {
            displayError("fourth input must be an Integer.");
        }
        uint32_t timeout = inputs[index][0];

        // Fifth argument is an integer
        if (inputs[++index].getType() != data::ArrayType::UINT32) {
            displayError("fifth input must be an Integer.");
        }
        uint32_t device_index = inputs[index][0];

        // Create a new `test_harness_mgr_client` object
        void* mgr_client = nullptr;
        try {
            mgr_client = new test_harness::test_harness_mgr_client(
                xclbin_file_path, graphNames, device_name, timeout, device_index);
        } catch (const test_harness::TestHarnessException& e) {
            displayError(e.what());
        }
        // assign mgr_client to the output
        data::ArrayFactory factory;
        data::Array res = factory.createArray({1, 1}, {reinterpret_cast<std::uintptr_t>(mgr_client)});

        outputs[0] = res;
    }
};
