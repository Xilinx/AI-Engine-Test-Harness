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

#include <algorithm>
#include <cstdint>
#include <deque>
#include <map>
#include <mutex>
#include <thread>
#include "test_harness_exception.hpp"
#include "test_harness_logging.hpp"
#include "test_harness_mgr_base.hpp"
#include "test_harness_client.hpp"
#include "test_harness_transaction.hpp"

namespace test_harness {

class test_harness_mgr_client : public test_harness_mgr_base, public test_harness_mgr_if {
   public:
   /**
    * @brief Constructor to initialize the test harness manager client.
    * @param xclbin_file_path: Path to the xclbin file.
    * @param graph_name: Names of the graphs.
    * @param device_name: Name of the device.
    * @param timeout_miliseconds: Timeout in milliseconds for internet connection.
    * @param device_index: Index of the device.
   */
    test_harness_mgr_client(std::string xclbin_file_path,
                            const std::vector<std::string>& graph_name,
                            const std::string& device_name, 
                            const uint64_t timeout_miliseconds = 0,
                            unsigned int device_index = 0);
                            
    ~test_harness_mgr_client();

    /**
    * @brief Run the test harness.
    * @param mode: Test mode.
    * @param args: Arguments for the test harness.
    * @param timeout: Timeout in milliseconds for the test harness transaction, 0 for run to finish.
    */
    void runTestHarness(TestMode mode, const std::vector<test_harness_args>& args, uint32_t timeout = 0) override;

    /**
    * @brief Run the AIE graph.
    * @param g_idx: Index of the graph.
    * @param iters: Number of iterations.
    */
    void runAIEGraph(unsigned int g_idx, unsigned int iters) override;

    /**
    * @brief Wait for the result.
    * @param timeout_millisec: Timeout in milliseconds.
    * @param num_trans: Wait for the given number of transactions, 0 for all.
    */
    void waitForRes(uint32_t timeout_millisec, uint32_t num_trans = 0) override;

    /**
    * @brief Check if the result is valid.
    * @param idx: Index of the transaction.
    * @return True if the result is valid, false otherwise.
    */
    bool isResultValid(int32_t t_idx = -1) override;

    /**
    * @brief Print the performance.
    * @param idx: Index of the transaction.
    */
    void printPerf(int32_t t_idx = -1) override;

    /**
    * @brief Get the delay between the channels.
    * @param from: Index of the source channel.
    * @param to: Index of the destination channel.
    * @param idx: Index of the transaction.
    * @return Delay between the channels.
    */
    int64_t getDelayBetween(channel_index, channel_index, int32_t t_idx = -1);

    uint64_t getServerTimeOut();
    void setServerTimeOut(uint64_t timeout_seconds);

    template<typename ArgType>
    void writeGraphPort(unsigned int g_idx, const std::string& port_name, ArgType& arg);
    template<typename ArgType>
    void readGraphPort(unsigned int g_idx, const std::string& port_name, ArgType& arg);

    private:
    void createTestHarness(int device_index,
                            std::string xclbin_file_path,
                            std::vector<std::string> graph_name);
    void sendXrtConfigs();
    void getLogFiles();
    void waitForTestHarness(test_harness_transactions* trans);
    void waitForGraph(uint32_t timeout_millisec);
    bool queryTransactionStatus(test_harness_transactions*);
    int32_t sanctifyTransIdx(int32_t idx);

   private:
    std::unique_ptr<test_harness_client> client;
    std::vector<test_harness_transactions*> transactions;
    std::deque<test_harness_transactions*> pending_transactions;
    std::map<test_harness_transactions*, std::thread> transaction_threads;
    bool xrt_config;
    std::vector<bool> graph_started;
    size_t max_memory_usage;
    std::mutex req_mtx, query_mtx;
};


inline
test_harness_mgr_client::test_harness_mgr_client(std::string xclbin_file_path,
                                                        const std::vector<std::string>& graph_name,
                                                        const std::string& device_name,
                                                        const uint64_t timeout_miliseconds,
                                                        unsigned int device_index): test_harness_mgr_base(device_name), xrt_config(false) {
    test_harness_logger::getLogger().info("Initializing the test harness manager.");
    const char* ip_port = std::getenv("SERVER_IP_PORT");
    if(ip_port == nullptr) {    
        test_harness_logger::getLogger().info("Environment variable SERVER_IP_PORT is not set, try to connect 127.0.0.1:8080 as the test harness manager server.");
        client = std::make_unique<test_harness_client>("127.0.0.1:8080");
    } else {
        test_harness_logger::getLogger().info("Connecting to the test harness manager server at " + std::string(ip_port));
        client = std::make_unique<test_harness_client>(ip_port);
    }

    bool available = false;
    if(timeout_miliseconds != 0) {
        client->setTimeOut(timeout_miliseconds);
        test_harness_logger::getLogger().info("Setting the timeout to " + std::to_string(timeout_miliseconds) + " milliseconds.");
    }

    try {
        client->recvData(available);
        client->sendData(true);
        client->setTimeOut(0);
    } catch (const SocketException& e) {
        test_harness_logger::getLogger().error(e.what());
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_CREATION_FAILURE,
                                "Failed to create the test harness manager due to timeout.");
    }
    
    sendXrtConfigs();
    createTestHarness(device_index, xclbin_file_path, graph_name);
}

inline
void test_harness_mgr_client::createTestHarness(int device_index, std::string xclbin_file_path, std::vector<std::string> graph_name) {
    std::vector<std::string> pl_kernel_names{this->device + "_test_harness"};
    std::lock_guard<std::mutex> lock(req_mtx);
    try {
        client->sendReq("test_harness_mgr");
    } catch (const SocketException& e) {
        test_harness_logger::getLogger().log(test_harness_logger::level::ERROR, e.what());
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_CREATION_FAILURE,
                                   "Failed to start the remote call to create the test_harness_mgr.");
    }

    try {
        // verify PL configurations
        client->sendData(this->device);
        bool res;
        client->recvData(res);
        if(!res) {
            std::string server_req_device;
            client->recvData(server_req_device);
            std::stringstream ss;
            ss << "Failed to create the test harness manager due to the test harness configuration mismatch on the server.\n";
            ss << "device name on the server side is: <" << server_req_device << ">." << std::endl;
            ss << "Please make sure the configurations on the client side match with the configurations on the server side.";
            throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_CREATION_FAILURE, ss.str());
        }

        // receiving server configurations
        unsigned int serverN, serverW, serverD, serverM;
        client->recvData(serverN);
        client->recvData(serverW);
        client->recvData(serverD);
        client->recvData(serverM);
        this->setup(serverN, serverD, serverW, serverM);

        // sending configurations
        client->sendData(device_index);
        client->sendVec(graph_name);
        graph_started.resize(graph_name.size(), false);
        client->sendVec(graph_started);
        client->sendVec(pl_kernel_names);
        client->sendData(fs::path(xclbin_file_path));
        client->recvData(max_memory_usage);
    } catch (const SocketException& e) {
        test_harness_logger::getLogger().log(test_harness_logger::level::ERROR, e.what());
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_CREATION_FAILURE,
                                   "Failed to create the test harness manager.");
    }
}


inline
void test_harness_mgr_client::runTestHarness(TestMode mode, const std::vector<test_harness_args>& args, uint32_t timeout) {
    static unsigned int trans_id = 0;
    test_harness_logger::getLogger().log(test_harness_logger::level::INFO, "Running the test harness.");
    if (!std::any_of(graph_started.begin(), graph_started.end(), [](bool v) { return v; })) {
        test_harness_logger::getLogger().log(test_harness_logger::level::WARNING, 
            "You're trying to call 'runTestHarness' before calling 'runAIEGraph'. This might lead to result of 'printPerf' to be fluctuated. It is strongly recommended to call 'runAIEGraph' before 'runTestHarness'.");
    }

    if(mode != FUNC_MODE && mode != PERF_MODE) {
        throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                "Invalid test mode. Please provide a valid test mode: <FUNC_MODE = 0/PERF_MODE = 1>.");
    }
    try {
        test_harness_transactions* trans = new test_harness_transactions(trans_id++, mode, 
                                                                        num_channels, word_size, depth, memWidth, timeout);
        transactions.push_back(trans);
        trans->handleInputs(args);
        if(trans->getStatus() != TRANSACTION_READY) {
            throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                    "Invalid test arguments. Please provide valid test arguments.");
        }

        size_t memorySize = trans->getMemoryUsage();
        if(memorySize > max_memory_usage) {
            throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                    "The memory usage of the test harness transaction exceeds the server available memory. Please reduce the memory usage by e.g., partition the transaction.");
        }

        for(auto& trans: pending_transactions) {
            memorySize += trans->getMemoryUsage();
        }

        if(memorySize > max_memory_usage) {
            test_harness_logger::getLogger().log(test_harness_logger::level::WARNING, 
                "The memory usage of the test harness exceeds the server available memory. Server may stall, please call waitForRes to wait for the transactions to reduce the memory usage.");
        }

        std::lock_guard<std::mutex> lock(req_mtx);
        client->sendReq("runTestHarness");
        client->sendData(trans->getMemoryUsage());
        bool trans_accepted;
        client->recvData(trans_accepted);
        if(!trans_accepted) {
            throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                    "The memory usage of the test harness transaction exceeds the server available memory. Please reduce the memory usage by waiting till other transactions done.");
        }
        client->sendData(static_cast<uint8_t>(mode));
        client->sendData(trans->trans_id);
        client->sendData(trans->getTimeOut());
        client->sendVec(trans->cfg_ptr);
        client->sendVec(trans->to_aie_ptr);
        client->sendData(trans->from_aie_ptr.size());
        trans->setStatus(TRANSACTION_RUNNING);
        pending_transactions.push_back(trans);
        transaction_threads.emplace(trans, std::thread(&test_harness_mgr_client::waitForTestHarness, this, trans));
    } catch (const SocketException& e) {
        test_harness_logger::getLogger().log(test_harness_logger::level::ERROR, e.what());
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_RUN_FAILURE,
                                    "Failed to send the arguments to the server.");
    }
}

inline
void test_harness_mgr_client::runAIEGraph(unsigned int g_idx, unsigned int iters) {
    test_harness_logger::getLogger().log(test_harness_logger::level::INFO, "Running the AIE graph.");
    // send args to remote and call runAIEGraph remotely
    if(g_idx >= graph_started.size()) {
        throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                "Invalid graph index. Please provide a valid graph index.");
    }
    if(graph_started[g_idx]) {
        throw TestHarnessException(TestHarnessStatus::INVALID_OPERATIONS,
                "The AIE graph is already running. Please stop the AIE graph before calling 'runAIEGraph'.");
    }
    std::lock_guard<std::mutex> lock(req_mtx);
    try {
        client->sendReq("runAIEGraph");
        client->sendData(g_idx);
        client->sendData(iters);
        client->recvVec(this->graph_started);
        assert(this->graph_started[g_idx]);
    } catch (const SocketException& e) {
        test_harness_logger::getLogger().log(test_harness_logger::level::ERROR, e.what());
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_RUN_GRAPH_FAILURE,
                                   "Failed to send the run the AIE graph on the server.");
    }
}

inline 
void test_harness_mgr_client::waitForRes(uint32_t timeout_millisec, uint32_t all) {
    test_harness_logger::getLogger().log(test_harness_logger::level::INFO, "Waiting for the result.");
    bool waitforAll = all == 0 ? true : false;
    // wait for the result from remote
    uint64_t timeout_in_seconds = (timeout_millisec + 999) / 1000;
    try {
        uint64_t server_timeout = getServerTimeOut();

        if(timeout_in_seconds != 0 && server_timeout != 0) {
            setServerTimeOut(timeout_in_seconds + server_timeout);
        }
        for(auto& t: pending_transactions) {
            t->setTimeOut(timeout_millisec);
            transaction_threads[t].join();
            pending_transactions.pop_front();
            transaction_threads.erase(t);
            all--;
            if(all == 0) break;
        }

        if(waitforAll) {
            waitForGraph(timeout_millisec);
        }

        setServerTimeOut(server_timeout);
    } catch (const SocketException& e) {
        test_harness_logger::getLogger().log(test_harness_logger::level::ERROR, e.what());
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_WAIT_FOR_RES_FAILURE,
                                   "Failed to get and set the server timeout.");
    }
    
}

inline void test_harness_mgr_client::waitForGraph(uint32_t timeout_millisec) {
    std::lock_guard<std::mutex> lock(req_mtx);
    client->sendReq("waitForGraph");
    client->sendData(timeout_millisec);
    client->recvVec(graph_started);
}

inline bool test_harness_mgr_client::queryTransactionStatus(test_harness_transactions* trans) {
    test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, "Querying for the transaction: " + std::to_string(trans->trans_id));
    std::lock_guard<std::mutex> lock(req_mtx);
    client->sendReq("queryTransactionStatus");
    client->sendData(trans->trans_id);
    uint8_t ustatus;
    client->recvData(ustatus);
    auto status = static_cast<TransactionStatus>(ustatus);
    if(status == TRANSACTION_DONE || status == TRANSACTION_TIMEOUT) {
        return true;
    }
    return false;
}

inline void test_harness_mgr_client::waitForTestHarness(test_harness_transactions* trans){
    assert(trans != nullptr);
    test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, "Waiting for the transaction " + std::to_string(trans->trans_id) + " to be done.");
    query_mtx.lock();
    while(trans->getTimeOut() == 0) {
        if(queryTransactionStatus(trans)) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(trans->mMonitorTimeout));
    }

    req_mtx.lock();
    query_mtx.unlock();
    client->sendReq("waitForTestHarness");
    client->sendData(trans->trans_id);
    client->sendData(trans->getTimeOut());
    uint8_t status;
    client->recvData(status);
    trans->setStatus(static_cast<TransactionStatus>(status));
    client->recvVec(trans->from_aie_ptr);
    client->recvVec(trans->perf_ptr);
    req_mtx.unlock();
    trans->handleOutputs();
}

inline test_harness_mgr_client::~test_harness_mgr_client() {
    std::lock_guard<std::mutex> lock(req_mtx);
    for(auto trans: transactions)
        delete trans;
    transactions.clear();

    test_harness_logger::getLogger().info("Destroying the test harness manager.");
    client->sendReq("destroy");
    if(xrt_config) getLogFiles();
}

inline void test_harness_mgr_client::sendXrtConfigs() {
    std::vector<fs::path> config_files;
    fs::path xrt_config_file = fs::path("xrt.ini");
    if (fs::exists(xrt_config_file)) {
        config_files.push_back(xrt_config_file);
        xrt_config = true;
    } else {
        xrt_config = false;
    }
    client->sendVec(config_files);
}

inline 
void test_harness_mgr_client::getLogFiles() {
    try {
        std::vector<fs::path> log_file_paths;
        client->recvVec(log_file_paths);
        for (auto& log_file_path : log_file_paths) {
            test_harness_logger::getLogger().info("Received log file: " + log_file_path.string());
        }
    } catch (const SocketException& e) {
        test_harness_logger::getLogger().error(e.what());
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_GET_LOG_FILES_FAILURE,
                                "Failed to get the log files from the server.");
    }
}

inline
void test_harness_mgr_client::setServerTimeOut(uint64_t timeout_seconds) {
    if(timeout_seconds == 0) {
        throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                "Invalid timeout value. Please provide a valid timeout value in seconds.");
    }
    std::lock_guard<std::mutex> lock(req_mtx);
    client->sendReq("setTimeOut");
    client->sendData(timeout_seconds);
}

inline
uint64_t test_harness_mgr_client::getServerTimeOut() {
    std::lock_guard<std::mutex> lock(req_mtx);
    uint64_t timeout_seconds;
    client->sendReq("getTimeOut");
    client->recvData(timeout_seconds);
    return timeout_seconds;
}

inline bool test_harness_mgr_client::isResultValid(int32_t idx) {
    idx = sanctifyTransIdx(idx);
    assert(idx >= 0 && idx < (int32_t)transactions.size());
    return transactions[idx]->isResultValid();
}

inline void test_harness_mgr_client::printPerf(int32_t idx) {
    idx = sanctifyTransIdx(idx);
    assert(idx >= 0 && idx < (int32_t)transactions.size());
    transactions[idx]->printPerf();
}

inline int64_t test_harness_mgr_client::getDelayBetween(channel_index from, channel_index to, int32_t idx) {
    idx = sanctifyTransIdx(idx);
    assert(idx >= 0 && idx < (int32_t)transactions.size());
    return transactions[idx]->getDelayBwtween(from, to);
}

inline int32_t test_harness_mgr_client::sanctifyTransIdx(int32_t idx) {
    if(transactions.empty()) {
        throw TestHarnessException(TestHarnessStatus::INVALID_OPERATIONS,
                "No transactions is available. Please run the test harness first.");
    } else if (idx == -1) {
        for(auto it = transactions.rbegin(); it != transactions.rend(); ++it) {
            if((*it)->getStatus() == TRANSACTION_DONE) {\
                return transactions.size() - 1 - (it - transactions.rbegin());
            }
        }
        throw TestHarnessException(TestHarnessStatus::INVALID_OPERATIONS,
                "No transactions is done. Please wait for the test harness transaction done.");
    } else if(idx < 0 || (size_t)idx >= transactions.size()) {
        throw TestHarnessException(TestHarnessStatus::INVALID_OPERATIONS,
                "The given index is out of range. Please provide a valid index.");
    }
    return idx;
}

template<typename ArgType>
void test_harness_mgr_client::writeGraphPort(unsigned int g_idx, const std::string& port_name, ArgType& arg) {
    std::lock_guard<std::mutex> lock(req_mtx);
    client->sendReq("update port");
    client->sendData(g_idx);
    client->sendData(port_name);
    unsigned int d_size = sizeof(ArgType);
    if(d_size < 1 || d_size > 8)
        throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                                "The data size is out of range.");
    std::vector<uint8_t> data;
    data.resize(d_size);
    std::memcpy(data.data(), &arg, sizeof(ArgType));
    client->sendData(data);
}

template<typename ArgType>
void test_harness_mgr_client::readGraphPort(unsigned int g_idx, const std::string& port_name, ArgType& arg) {
    std::lock_guard<std::mutex> lock(req_mtx);
    client->sendReq("read port");
    client->sendData(g_idx);
    client->sendData(port_name);
    unsigned int d_size = sizeof(ArgType);
    if(d_size < 1 || d_size > 8)
        throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                                "The data size is out of range.");
    client->sendData(d_size);
    std::vector<uint8_t> data;
    client->recvData(data);
    std::memcpy(&arg, data.data(), sizeof(ArgType));
}

} // namespace test_harness
#endif
