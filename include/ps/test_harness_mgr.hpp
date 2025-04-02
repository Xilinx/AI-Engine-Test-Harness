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

#ifndef _TEST_HARNESS_MGR_HPP_
#define _TEST_HARNESS_MGR_HPP_

/*
 * xrt native api based manager
 */

#include "test_harness_logging.hpp"
#include "test_harness_mgr_base.hpp"
#include "test_harness_fastxm.hpp"
#include "test_harness_transaction.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>

#ifndef PARAM_DEVICE
#error "PARAM_DEVICE is not defined. Please define PARAM_DEVICE macro in your code."
#endif

namespace test_harness {

namespace fs = std::filesystem;

/*
* @param device_index
 * The device id of the testing board, typically it will be zero
 * @param xclbin_file_path
 * The name, including its full path, to the xclbin file to be tested.
 * Valid xclbin name list: vck190_test_harness_func, vck190_test_harness_perf, vek280_test_harness.
 * @param graph_name
 * The vector of graph names in the libadf.a and packaged in the xclbin file
 */

class test_harness_mgr : public test_harness_mgr_base, public test_harness_mgr_if {
   public:
    test_harness_mgr(const fs::path& xclbin_file_path, 
                    const std::vector<std::string>& graph_name, 
                    const std::string& device_name = mc_stringfy(PARAM_DEVICE),
                    unsigned int device_index = 0,
                    unsigned int num_channels = PARAM_CHANNELS, 
                    unsigned int word_size = PARAM_WIDTH, 
                    unsigned int depth = PARAM_DEPTH,
                    unsigned int memWidth = PARAM_MEM_WIDTH);
    
    void runTestHarness(TestMode mode, const std::vector<test_harness_args>& args, uint32_t timeout = 0) override;
    void runAIEGraph(unsigned int g_idx, unsigned int iters) override;
    void waitForRes(uint32_t timeout_millisec = 0, uint32_t all = 0) override; // @all is not supported in the legacy mode
    void printPerf(int32_t idx = -1) override;   // @idx is not supported in the legacy mode
    bool isResultValid(int32_t idx = -1) override;  // @idx is not supported in the legacy mode
    ~test_harness_mgr() = default;

   private:
    std::unique_ptr<fastXM> fastObj;
    std::vector<test_harness_args> args_rec;
    std::unique_ptr<test_harness_transactions> trans;
    bool graph_started;

    // for REP_MODE + PERF_MODE
};

inline test_harness_mgr::test_harness_mgr(const fs::path& xclbin_file_path, const std::vector<std::string>& graph_name, 
                    const std::string& device_name, unsigned int device_index, 
                    unsigned int num_channels, unsigned int word_size, unsigned int depth, unsigned int memWidth): test_harness_mgr_base(device_name) {
    this->setup(num_channels, depth, word_size, memWidth);
    graph_started = false;
    std::vector<std::string> pl_kernel_name = {this->device + "_test_harness"};
    fastObj = std::make_unique<fastXM>(device_index, xclbin_file_path, pl_kernel_name, graph_name);
}

inline void test_harness_mgr::runAIEGraph(unsigned int g_idx, unsigned int iters)  {
    fastObj->runGraph(g_idx, iters);
    this->graph_started = true;
}

inline void test_harness_mgr::waitForRes(uint32_t timeout_millisec, uint32_t all)  {
    if(trans && trans->getStatus() == TransactionStatus::TRANSACTION_RUNNING) {
        auto res = fastObj->waitForTestHarness(std::chrono::milliseconds(timeout_millisec));
        if(res){
            fastObj->fetchRes();
            trans->handleOutputs();
            trans->setStatus(TransactionStatus::TRANSACTION_DONE);
        } else {
            test_harness_logger::getLogger().log(test_harness_logger::level::WARNING, "Timeout occurred while waiting for the test harness to complete.");
            trans->setStatus(TransactionStatus::TRANSACTION_TIMEOUT);
        }
    } else {
        throw TestHarnessException(TestHarnessStatus::INVALID_OPERATIONS,
                "No transaction object found. Please call 'runTestHarness' before calling 'waitForTestHarness'.");
    }
    if(graph_started) {
        fastObj->waitForGraph(std::chrono::milliseconds(timeout_millisec));
        this->graph_started = false;
    }
}

inline void test_harness_mgr::runTestHarness(TestMode mode, const std::vector<test_harness_args>& args, uint32_t timeout) {
    if (!this->graph_started) {
        test_harness_logger::getLogger().log(test_harness_logger::level::WARNING, 
            "You're trying to call 'runTestHarness' before calling 'runAIEGraph'. This might lead to result of 'printPerf' to be fluctuated. It is strongly recommended to call 'runAIEGraph' before 'runTestHarness'.");
    }
    
    if(mode != FUNC_MODE && mode != PERF_MODE) {
        throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                "Invalid test mode. Please provide a valid test mode: <FUNC_MODE = 0/PERF_MODE = 1>.");
    }

    if(trans != nullptr && trans->getStatus() == TransactionStatus::TRANSACTION_RUNNING) {
        throw TestHarnessException(TestHarnessStatus::INVALID_OPERATIONS,
                "Transaction object already exists. Please call 'waitForTestHarness' before calling 'runTestHarness'.");
    }

    trans = std::make_unique<test_harness_transactions>(0, mode, num_channels, word_size, depth, memWidth);
    trans->handleInputs(args);
    assert(trans->getStatus() == TransactionStatus::TRANSACTION_READY);
    std::vector<plKernelArg> pl_args;
    unsigned int args_index = 0;
    
    pl_args.push_back({args_index++, true, trans->cfg_ptr.size() * sizeof(uint64_t), (char*)trans->cfg_ptr.data(), 0});
    pl_args.push_back({args_index++, true, trans->perf_ptr.size() * sizeof(uint64_t), (char*)trans->perf_ptr.data(), 0});
    pl_args.push_back({args_index++, true, trans->to_aie_ptr.size(), trans->to_aie_ptr.data(), 0});
    pl_args.push_back({args_index++, true, trans->from_aie_ptr.size(), trans->from_aie_ptr.data(), 0});
    
    fastObj->runPL(0, pl_args);
    trans->setStatus(TransactionStatus::TRANSACTION_RUNNING);
}

inline bool test_harness_mgr::isResultValid(int32_t idx) {
    if(trans)
        return trans->isResultValid();
    else
        throw TestHarnessException(TestHarnessStatus::INVALID_OPERATIONS,
                "No transaction object found. Please call 'runTestHarness' before calling 'isResultValid'.");
}

inline void test_harness_mgr::printPerf(int32_t idx) {
    if(trans)
        trans->printPerf();
    else
        throw TestHarnessException(TestHarnessStatus::INVALID_OPERATIONS,
                "No transaction object found. Please call 'runTestHarness' before calling 'printPerf'.");
}

}
#endif
