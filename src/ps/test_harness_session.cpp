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

#include "test_harness_session.hpp"
#include "test_harness_exception.hpp"
#include "test_harness_utils.hpp"

#include <filesystem>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <cassert>
#include <cstring>
#include <atomic>
#include <chrono>
#include <string>
#include <signal.h>

using namespace test_harness;

int main(int argc, char* argv[]) {
    assert(argc == 3 && "Usage: ./test_harness_server socket_path timeout");
    try {
        const fs::path logPath = fs::current_path() / fs::path("test_harness_session.log");
        test_harness_logger::getLogger().setLogFile(logPath.string());
        test_harness_server testServer(argv[1]);
        test_harness_session session(&testServer, std::stoul(argv[2]));
        auto runThread = std::thread(&test_harness_session::run, &session);
        auto monitorThread = std::thread(&test_harness_session::monitor, &session);
        monitorThread.join();
        runThread.join();
    } catch (const TestHarnessException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return static_cast<uint8_t>(e.status());
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

test_harness_session::test_harness_session(test_harness_server *server, 
                                            uint64_t monitorInterval) : server(server), 
                                                                        monitorInterval(monitorInterval),
                                                                        mActive(false),
                                                                        mDone(false) {
}

void test_harness_session::monitor() {
    if (monitorInterval != 0) {
        test_harness_logger::getLogger().log(test_harness_logger::level::WARNING, 
                    "Watchdog is monitoring, the current session will be killed if no active request within " + std::to_string(monitorInterval) + " seconds.");
        uint64_t count = monitorInterval;
        while(true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (mDone) {
                return;
            }

            if(isActive()) {
                count = monitorInterval;
                setActive(false);
            } else {
                count--;
            }

            if(count == 0) {
                break;
            }
        }
       
        test_harness_logger::getLogger().log(test_harness_logger::level::WARNING, "Watchdog is exiting and destroying the session.");
        mgr.reset();
        throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_RUN_FAILURE, "test harness manager timeout.");
    }
    test_harness_logger::getLogger().log(test_harness_logger::level::WARNING, "The watchdog is disabled.");
}

void test_harness_session::setActive(bool active) {
    mActive = active;
}

bool test_harness_session::isActive() {
    return mActive;
}

void test_harness_session::setTimeOut(uint64_t timeout) {
    if(timeout == 0) {
        throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS, "Invalid timeout value.");
    }
    monitorInterval = timeout;
}

void test_harness_session::run() {
    try{
        int unknownReqCount = 0;
        while (true) {
            test_harness_logger::getLogger().log(test_harness_logger::level::INFO, "Waiting for request");
            auto request = server->recvReq();
            if (request == "test_harness_mgr") {
                // parse the string request as var/d_size/op
                if(mgr) {
                    mgr.reset();
                    throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_NOT_CREATED_FAILURE, "test harness manager is already created.");
                }
                test_harness_logger::getLogger().log(test_harness_logger::level::INFO, "Creating test_harness_mgr");
                setActive(true);
                mgr = std::make_unique<test_harness_mgr_server>(server, mc_stringfy(PARAM_DEVICE), PARAM_CHANNELS, PARAM_DEPTH, PARAM_WIDTH, PARAM_MEM_WIDTH);
            } else if (request == "runTestHarness") {
                if (!mgr) throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_NOT_CREATED_FAILURE, "test harness manager is not created.");
                test_harness_logger::getLogger().log(test_harness_logger::level::INFO, "Running test harness");
                setActive(true);
                mgr->runTestHarness();
            } else if (request == "runAIEGraph") {
                if (!mgr) throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_NOT_CREATED_FAILURE, "test harness manager is not created.");
                test_harness_logger::getLogger().log(test_harness_logger::level::INFO, "Running AIE graph");
                setActive(true);
                mgr->runAIEGraph();
            } else if (request == "waitForTestHarness") {
                if (!mgr) throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_NOT_CREATED_FAILURE, "test harness manager is not created.");
                test_harness_logger::getLogger().log(test_harness_logger::level::INFO, "Waiting for test harness transaction done");
                setActive(true);
                mgr->waitForTestHarness();
            }  else if (request == "waitForGraph") {
                if (!mgr) throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_NOT_CREATED_FAILURE, "test harness manager is not created.");
                test_harness_logger::getLogger().info("Waiting for graph done");
                setActive(true);
                mgr->waitForGraph();
            } else if (request == "queryTransactionStatus") {
                if (!mgr) throw TestHarnessException(TestHarnessStatus::TEST_HARNESS_NOT_CREATED_FAILURE, "test harness manager is not created.");
                test_harness_logger::getLogger().debug("Querying the transaction status");
                mgr->queryTransactionStatus();
            } else if (request == "destroy") {
                mgr.reset();
                break;
            } else if (request == "setTimeOut") {
                test_harness_logger::getLogger().debug("Setting timeout");
                uint64_t timeout;
                server->recvData(timeout);
                setTimeOut(timeout);
            } else if (request == "getTimeOut") {
                test_harness_logger::getLogger().debug("Getting the timeout");
                server->sendData(monitorInterval);
            } else {
                test_harness_logger::getLogger().log(test_harness_logger::level::ERROR, "Unknown request: " + request);
                unknownReqCount++;
                if (unknownReqCount > 10)
                    throw TestHarnessException(TestHarnessStatus::UNKNOWN_REQUEST, "Existing due to too many unknown requests.");
            }
        }
    } catch (const SocketException& e) {
        std::cerr << e.what() << std::endl;
    } catch (const TestHarnessException& e) {
        std::cerr << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    mDone = true;
}