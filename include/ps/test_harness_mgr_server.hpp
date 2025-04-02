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

#ifndef _TEST_HARNESS_MGR_HPP_
#define _TEST_HARNESS_MGR_HPP_

#include <cstdint>
#include <mutex>
#include <deque>
#include <thread>
#include <unordered_map>
#include "test_harness_mgr_base.hpp"
#include "test_harness_fastxm.hpp"
#include "test_harness_server.hpp"
#include "test_harness_transaction.hpp"

namespace test_harness {

class test_harness_mgr_server : public test_harness_mgr_base {
   public:
    /*
     * test_harness_mgr_server() - Loads the xclbin on the devide and initializes the
     * various test harness runtime objects
     *
     * @tparam N
     * Number of PLIOs (for input/output respectively)
     * @tparam W
     * PLIO bit-width in bytes
     * @tparam D
     * Depth of the 128-bit width cache in each PLIO channel
     *
     * @param device_index
     * The this->device id of the testing board, typically it will be zero
     * @param xclbin_file_path
     * The name, including its full path, to the xclbin file to be tested.
     * Valid xclbin name list: vck190_test_harness_func, vck190_test_harness_perf, vek280_test_harness.
     * @param graph_name
     * The vector of graph names in the libadf.a and packaged in the xclbin file
     */
    test_harness_mgr_server(test_harness_server* server,
                const std::string& device_name,
                unsigned int num_channels,
                unsigned int depth, 
                unsigned int word_size,
                unsigned int mem_width);
    void verifyDevice();
    void init();
    ~test_harness_mgr_server();
    void runTestHarness();
    void runPL(test_harness_transactions*);
    void runAIEGraph();
    void waitForTestHarness();
    void waitForGraph();
    void queryTransactionStatus();
    void readGraphPort();
    void writeGraphPort();

   private:
    std::mutex pl_mtx;
    std::unique_ptr<fastXM> fastObj;
    test_harness_server* server;
    std::vector<fs::path> received_files;
    std::deque<test_harness_transactions*> transactions;
    std::map<test_harness_transactions*, std::thread> pl_threads;
    std::unordered_map<unsigned int, test_harness_transactions*> id2trans;
    std::vector<bool> graph_started;

    std::atomic<uint64_t> memorySize;
    static const size_t memorySizeMax;
};
} // namespace test_harness
#endif
