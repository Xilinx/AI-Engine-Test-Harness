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
            displayError("Four inputs are required.");
        }

        if (outputs.size() != 1) {
            displayError("One output is required.");
        }

        // first argument is an integer
        if (inputs[0].getType() != data::ArrayType::UINT64) {
            displayError("First input must be the test_harness object handle.");
        }

        uintptr_t handle = inputs[0][0];

        // second argument is an integer
        if (inputs[1].getType() != data::ArrayType::ENUM) {
            displayError("Second input must be the index of the test harness transactions.");
        }
        data::TypedArray<uint16_t> numericInput0 = matlabPtr->feval(u"uint16", 1, {inputs[1]})[0];
        test_harness::channel_index idx0 = static_cast<test_harness::channel_index>((uint16_t)numericInput0[0]);
        
        // third argument is an integer
        if (inputs[2].getType() != data::ArrayType::ENUM) {
            displayError("Third input must be the index of the test harness channel.");
        }

        data::TypedArray<uint16_t> numericInput1 = matlabPtr->feval(u"uint16", 1, {inputs[2]})[0];
        test_harness::channel_index idx1 = static_cast<test_harness::channel_index>((uint16_t)numericInput1[0]);

        // fourth argument is an integer
        if (inputs[3].getType() != data::ArrayType::INT32) {
            displayError("Fourth input must be the index of the transactions.");
        }
        int32_t t_id = inputs[3][0];

        try {
            auto mgr_client =
                reinterpret_cast<test_harness::test_harness_mgr_client*>(handle);
            auto delay = mgr_client->getDelayBetween(idx0, idx1, t_id);

            // assign mgr_client to the output
            data::Array res = factory.createArray({1, 1}, {delay});
            outputs[0] = res;
        } catch (std::exception& e) {
            displayError(e.what());
        }
    }
};
