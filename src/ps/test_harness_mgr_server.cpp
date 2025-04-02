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

#include "test_harness_mgr_server.hpp"
#include "test_harness_exception.hpp"
#include "test_harness_logging.hpp"
#include "test_harness_transaction.hpp"

#include <chrono>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <thread>

namespace fs = std::filesystem;

namespace test_harness {

const size_t test_harness_mgr_server::memorySizeMax = 1024 * 1024 * 1024; // 1 GB

test_harness_mgr_server::test_harness_mgr_server(test_harness_server* server,
        const std::string& device_name, 
        unsigned int num_channels, 
        unsigned int depth, unsigned int word_size, unsigned int mem_width) : test_harness_mgr_base(device_name), server(server), memorySize(0) {
            test_harness_mgr_base::setup(num_channels, depth, word_size, mem_width);
            received_files.clear();
            verifyDevice();
            init();
        }

void test_harness_mgr_server::verifyDevice() {
    try {
        // verify PL configuration
        std::string client_req_device;
        server->recvData(client_req_device);
        if(this->device.compare(client_req_device) != 0 ) {
            server->sendData(false);
            server->sendData(this->device);
            std::stringstream ss;
            ss << "Device mismatch in PL configuration, expected: <" << this->device << "> "
               << "received: <" << client_req_device << ">.";
            throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS, ss.str());
        }
        
        // propagate server setup to client
        server->sendData(true);
        server->sendData(num_channels);
        server->sendData(word_size);
        server->sendData(depth);
        server->sendData(memWidth);
    } catch (const SocketException& e) {
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_CREATION_FAILURE,
                    "Failed to verify the PL configuration on the server due to error " + std::string(e.what()));
    }
}

void test_harness_mgr_server::init() {
    try{
        unsigned int device_index;
        std::vector<std::string> pl_kernel_names;
        std::vector<std::string> graph_names;
        std::string xclbin_name;
        
        server->recvData(device_index);
        server->recvVec(graph_names);
        server->recvVec(graph_started);
        server->recvVec(pl_kernel_names);
        fs::path user_xclbin_path = fs::current_path() / fs::path("xclbins");
        fs::create_directories(user_xclbin_path);
        server->recvData(user_xclbin_path);
        received_files.push_back(user_xclbin_path);

        if(this->device.compare("vek280") == 0) {
            fs::path pl_xclbin_path;
            // check if envrionment variable SERVER_ROOT is set
            char* server_root = std::getenv("SERVER_ROOT");
            if(server_root == nullptr) {
                pl_xclbin_path = "/run/media/mmcblk0p1/vek280_test_harness.xclbin";
            } else {
                pl_xclbin_path = fs::path(server_root) / fs::path("vek280_test_harness.xclbin");
            }
            fastObj = std::make_unique<fastXM>(device_index, pl_xclbin_path, user_xclbin_path, pl_kernel_names, graph_names);
        } else if (this->device.compare("vck190") == 0) {
            fastObj = std::make_unique<fastXM>(device_index, user_xclbin_path, pl_kernel_names, graph_names);
        }
        server->sendData(memorySizeMax);
    } catch (const SocketException& e) {
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_CREATION_FAILURE,
                    "Failed to create test harness manager on the server due to error " + std::string(e.what()));
    }
}

void test_harness_mgr_server::runTestHarness() {
    try {
        size_t trans_size;
        server->recvData(trans_size);
        if(memorySize + trans_size > memorySizeMax) {
            server->sendData(false);
            return;
        }
        server->sendData(true);
        uint8_t umode;
        server->recvData(umode);
        TestMode mode = static_cast<TestMode>(umode);
        if(mode != FUNC_MODE && mode != PERF_MODE) {
            throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                    "Invalid test mode. Please provide a valid test mode: <FUNC_MODE = 0/PERF_MODE = 1>.");
        }

        unsigned int trans_id;
        server->recvData(trans_id);
        if(id2trans.find(trans_id) != id2trans.end()) {
            throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                    "Transaction ID already exists. Please provide a unique transaction ID.");
        }
        uint32_t timeout;
        server->recvData(timeout);
        auto* trans = new test_harness_transactions(trans_id, mode, num_channels, 
                                                                word_size, depth, timeout);
        transactions.push_back(trans);
        id2trans.emplace(trans_id, trans);
    
        server->recvVec(trans->cfg_ptr);
        server->recvVec(trans->to_aie_ptr);
        size_t from_aie_size;
        server->recvData(from_aie_size);
        trans->from_aie_ptr.resize(from_aie_size);
        trans->setStatus(TRANSACTION_READY);
        auto memUse = trans->getMemoryUsage();
        if(memUse != trans_size) {
            throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                    "Transaction size mismatch. Please provide a valid transaction size.");
        }
        memorySize += memUse;
        pl_threads.emplace(trans, std::thread(&test_harness_mgr_server::runPL, this, trans));

        test_harness_logger::getLogger().debug("Transaction " + std::to_string(trans_id) + " is created and ready to run.");
        test_harness_logger::getLogger().debug("Current server memory usage: " + std::to_string(memorySize) + " bytes for " + std::to_string(transactions.size()) + " transactions.");
        // test_harness_logger::getLogger().debug("cfg_file_" + std::to_string(trans->trans_id) + ".bin", trans->cfg_ptr);
        // test_harness_logger::getLogger().debug("to_aie_file_" + std::to_string(trans->trans_id) + ".bin", trans->to_aie_ptr);
    } catch (const SocketException& e) {
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_RUN_FAILURE,
                                    "Failed to receive the configurations from the client due to error " + std::string(e.what()));
    }
}

void test_harness_mgr_server::runPL(test_harness_transactions* trans) {
    while(transactions.front() != trans)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    const std::lock_guard<std::mutex> lock(pl_mtx);
    std::vector<plKernelArg> pl_args;
    unsigned int args_index = 0;
    pl_args.push_back({args_index++, true, trans->cfg_ptr.size() * sizeof(uint64_t), (char*)trans->cfg_ptr.data(), 0});
    pl_args.push_back({args_index++, true, trans->perf_ptr.size() * sizeof(uint64_t), (char*)trans->perf_ptr.data(), 0});
    pl_args.push_back({args_index++, true, trans->to_aie_ptr.size(), trans->to_aie_ptr.data(), 0});
    pl_args.push_back({args_index++, true, trans->from_aie_ptr.size(), trans->from_aie_ptr.data(), 0});
    test_harness_logger::getLogger().debug("Started to run the test harness, PL is locked for transaction " + std::to_string(trans->trans_id));
    fastObj->runPL(0, pl_args);
    trans->setStatus(TRANSACTION_RUNNING);
    while(trans->getTimeOut() == 0) {
        auto res = fastObj->waitForTestHarness(std::chrono::milliseconds(trans->mMonitorTimeout));
        if(res) {
            fastObj->fetchRes();
            trans->setStatus(TransactionStatus::TRANSACTION_DONE);
            test_harness_logger::getLogger().debug("Transaction " + std::to_string(trans->trans_id) + " done and PL is unlocked.");
            return;
        }
    }
    assert(trans->getTimeOut() != 0);
    auto res = fastObj->waitForTestHarness(std::chrono::milliseconds(trans->getTimeOut()));
    if(res)
        trans->setStatus(TransactionStatus::TRANSACTION_DONE);
    else
        trans->setStatus(TransactionStatus::TRANSACTION_TIMEOUT);
    fastObj->fetchRes();
    test_harness_logger::getLogger().debug("Transaction " + std::to_string(trans->trans_id) + " done and PL is unlocked.");
}

void test_harness_mgr_server::runAIEGraph() {
    try {
        unsigned int g_idx, iters;
        server->recvData(g_idx);
        server->recvData(iters);
        test_harness_logger::getLogger().debug("Running graph " + std::to_string(g_idx) + " for " + std::to_string(iters) + " iterations.");
        fastObj->runGraph(g_idx, iters);
        this->graph_started[g_idx] = true;
        server->sendVec(this->graph_started);
    } catch (const SocketException& e) {
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_RUN_GRAPH_FAILURE,
                                   "Failed to run AIE graph on the server due to error " + std::string(e.what()));
    }
}

void test_harness_mgr_server::waitForGraph() {
    try {
        int timeout_millisec;
        server->recvData(timeout_millisec);
        for(int i=0;i<graph_started.size();i++) {
            if(graph_started[i])
                fastObj->waitForGraph(i, std::chrono::milliseconds(timeout_millisec));
            graph_started[i] = false;
        }
        server->sendVec(graph_started);
    } catch (const SocketException& e) {
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_RUN_FAILURE,
                                   "Failed to wait for AIE graph on the server due to error " + std::string(e.what()));
    }
}

void test_harness_mgr_server::queryTransactionStatus() {
    try {
         unsigned int trans_id;
        server->recvData(trans_id);
        auto it = id2trans.find(trans_id);
        if(it == id2trans.end()) {
            throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                    "Transaction ID does not exist. Please provide a valid transaction ID.");
        }
        server->sendData(static_cast<uint8_t>(it->second->getStatus()));
        test_harness_logger::getLogger().debug(
            "Querying for the transaction: " + std::to_string(trans_id) + ", and the status is: " + std::to_string(static_cast<uint8_t>(it->second->getStatus())));
    } catch (const SocketException& e) {
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_RUN_FAILURE,
                                   "Failed to query the transaction status on the server due to error " + std::string(e.what()));
    }
}


void test_harness_mgr_server::waitForTestHarness() {
    if(pl_threads.empty()) {
        throw TestHarnessException(TestHarnessStatus::INVALID_OPERATIONS,
                "No PL thread is running. Please run the test harness before calling 'waitForRes'.");
    }
    
    auto trans = transactions.front();
    auto it = pl_threads.find(trans);
    if(it == pl_threads.end() || pl_threads.size() != transactions.size()) {
        throw TestHarnessException(TestHarnessStatus::INVALID_OPERATIONS,
                "Fatal errors in the server, the tread of the current transaction is missing.");
    }
    try {
        uint32_t tran_id;
        server->recvData(tran_id);
        if(tran_id != trans->trans_id) {
            throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                    "Transaction ID mismatch. Please provide a valid transaction ID.");
        }
        uint32_t timeout_millisec;
        server->recvData(timeout_millisec);

        test_harness_logger::getLogger().debug(
            "Waiting for results with timeout " + std::to_string(timeout_millisec) + " ms and for test harness transaction id: " + std::to_string(tran_id));
        trans->setTimeOut(timeout_millisec);
        it->second.join();
        server->sendData(static_cast<uint8_t>(trans->getStatus()));
        server->sendVec(trans->from_aie_ptr);
        server->sendVec(trans->perf_ptr);
        pl_threads.erase(it);
        transactions.pop_front();
        memorySize-=trans->getMemoryUsage();
        
        // test_harness_logger::getLogger().debug("from_aie_ptr_" + std::to_string(trans->trans_id) + ".bin", trans->from_aie_ptr);
        // test_harness_logger::getLogger().debug("perf_" + std::to_string(trans->trans_id) + ".bin", trans->perf_ptr);
        test_harness_logger::getLogger().info("Transaction " + std::to_string(tran_id) + " is done and results are sent back to the client.");
        delete trans;
    } catch (const SocketException& e) {
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_WAIT_FOR_RES_FAILURE,
                                   "Failed to wait for results on the server due to error " + std::string(e.what()));
    }
}

test_harness_mgr_server::~test_harness_mgr_server() {
    fastObj.reset();
    for(auto trans: transactions)
        delete trans;
    transactions.clear();
    for(auto& file : received_files)
        fs::remove_all(file);
}

void test_harness_mgr_server::readGraphPort() {
    try {
        unsigned int g_idx, d_size;
        std::string port_name;
        server->recvData(g_idx);
        server->recvData(port_name);
        server->recvData(d_size);
        std::vector<uint8_t> port_data(d_size);
        switch(d_size) {
            case 1:
                uint8_t data;
                fastObj->readGraphPort(g_idx, port_name, data);
                memcpy(port_data.data(), &data, d_size);
                break;
            case 2:
                uint16_t data16;
                fastObj->readGraphPort(g_idx, port_name, data16);
                memcpy(port_data.data(), &data16, d_size);
                break;
            case 4:
                uint32_t data32;
                fastObj->readGraphPort(g_idx, port_name, data32);
                memcpy(port_data.data(), &data32, d_size);
                break;
            case 8:
                uint64_t data64;
                fastObj->readGraphPort(g_idx, port_name, data64);
                memcpy(port_data.data(), &data64, d_size);
                break;
            default:
                throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                                "The data size is out of range.");
        }
        server->sendVec(port_data);
    } catch (const SocketException& e) {
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_RUN_FAILURE,
                                   "Failed to read AIE graph port on the server due to error " + std::string(e.what()));
    }
}

void test_harness_mgr_server::writeGraphPort() {
    try {
        unsigned int g_idx, d_size;
        std::string port_name;
        server->recvData(g_idx);
        server->recvData(port_name);
        std::vector<uint8_t> port_data;
        server->recvVec(port_data);
        switch(d_size) {
            case 1:
                uint8_t data;
                memcpy(&data, port_data.data(), d_size);
                fastObj->writeGraphPort(g_idx, port_name, data);
                break;
            case 2:
                uint16_t data16;
                memcpy(&data16, port_data.data(), d_size);
                fastObj->writeGraphPort(g_idx, port_name, data16);
                break;
            case 4:
                uint32_t data32;
                memcpy(&data32, port_data.data(), d_size);
                fastObj->writeGraphPort(g_idx, port_name, data32);
                break;
            case 8:
                uint64_t data64;
                memcpy(&data64, port_data.data(), d_size);
                fastObj->writeGraphPort(g_idx, port_name, data64);
                break;
            default:
                throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS,
                                "The data size is out of range.");
        }
    } catch (const SocketException& e) {
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_RUN_FAILURE,
                                   "Failed to write AIE graph port on the server due to error " + std::string(e.what()));
    }
}

}

// namespace test_harness