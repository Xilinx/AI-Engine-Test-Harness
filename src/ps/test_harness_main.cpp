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

#include <unistd.h>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <sys/wait.h>

// The test harness APIs
#include "test_harness_exception.hpp"
#include "test_harness_main.hpp"

namespace fs = std::filesystem;
using namespace test_harness;

int main(int argc, char* argv[]) {    
    assert(argc == 4 && "Usage: test_harness_server port timeout_in_seconds reset_flag");
    uint16_t port = std::atoi(argv[1]);
    uint64_t timeout = std::atoi(argv[2]);
    bool reset = std::atoi(argv[3]);

    fs::path curExec = fs::path(argv[0]);
    fs::path newExec = fs::absolute(curExec.parent_path()) / fs::path("test_harness_session");


    try {
        const fs::path logFile = fs::current_path() / fs::path("test_harness_server.log");
        test_harness_logger::getLogger().setLogFile(logFile.string());
        test_harness_server mainServer(port);
        test_harness_main testServer(&mainServer);
        testServer.setSessionExecPath(newExec);
        testServer.setResetFlag(reset);
        testServer.setTimeOut(timeout);
        testServer.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}

test_harness_main::test_harness_main(test_harness_server* server){
    this->server = server;
    mSessionId = 0;
    mResetFlag = false;
    mSessionTimeOut = 0;
}

void test_harness_main::setSessionExecPath(const fs::path& newExec) {
    this->sessionExecPath = newExec;
}

void test_harness_main::setResetFlag(bool reset) {
    this->mResetFlag = reset;
}

void test_harness_main::setTimeOut(uint64_t timeout) {
    this->mSessionTimeOut = timeout;
}

void test_harness_main::syncWorkDir(const fs::path& sessPath) {
    std::vector<fs::path> files;
    for (const auto& entry : fs::directory_iterator(sessPath)) {  
        if (fs::is_regular_file(entry.status())) {
            test_harness_logger::getLogger().debug("Adding log file: " + entry.path().string());
            files.push_back(entry.path());
        }
    }
    server->sendVec(files);
}

std::vector<fs::path> test_harness_main::getXrtConfigs() {
    std::vector<fs::path> files;
    server->recvVec(files);
    return files;
}

fs::path test_harness_main::createSessionPath() {
    fs::path sessPath;
    int count = 0;
    while(true) { // loop to create a session working directory
        sessPath = fs::current_path() / fs::path("session_" + std::to_string(mSessionId++));
        // Change the working directory to the session directory
        try {
            fs::remove_all(sessPath);
            fs::create_directories(sessPath);
        } catch (const fs::filesystem_error& e) {
            std::cerr << e.what() << std::endl;
            sleep(5);
            count++;
            if (count > 5) {
                throw std::runtime_error("Failed to create session directory");
            }
            continue;
        }
        break;
    }
    return sessPath;
}

void test_harness_main::runSession() {
    fs::path sessPath = createSessionPath();
    fs::path curWorkDir = fs::current_path();
    fs::current_path(sessPath);
    auto config = getXrtConfigs();
    std::string command = sessionExecPath.string() + ' ' + std::to_string(server->getSocketFd()) + ' ' + std::to_string(mSessionTimeOut);
    test_harness_logger::getLogger().debug("Starting a test harness session " + std::to_string(mSessionId) 
                                                + " in " + sessPath.string() + " with command: " + command);
    int ret = system(command.c_str()); // start the test harness session in a new process
    
    if(config.size() > 0) {
        for(const auto& file : config) {
            fs::remove_all(file);
        }
        if(ret == 0) syncWorkDir(sessPath);
    }

    fs::current_path(curWorkDir);
#ifndef DEBUG
    fs::remove_all(sessPath);
#endif
    if(ret != 0) {
        test_harness_logger::getLogger().log(test_harness_logger::level::ERROR, "Test harness session failed with exit code " + std::to_string(ret));
    }
}

void test_harness_main::run() {
    while(true) {
        auto pid = fork();
        if(pid == 0) {
            if(mResetFlag) system("xbutil reset --force");
            return;
        } else if (pid < 0) {
            throw std::runtime_error("Failed to fork a new process");
        } else {
            waitpid(pid, NULL, 0);
        }
        
        test_harness_logger::getLogger().log(test_harness_logger::level::INFO, "Waiting for connection");
        bool res = server->accept();
        
        if(res) {
            // confirm the connection with the client
            bool resp = false;
            try {
                server->sendData(true);
                server->recvData(resp);
            } catch (const std::exception& e) {
                server->closeConnection();
                continue;
            }
            // start a new session
            runSession();
            server->closeConnection();
        }
    }
}