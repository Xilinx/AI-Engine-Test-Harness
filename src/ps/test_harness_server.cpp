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
#include "test_harness_logging.hpp"
#include "test_harness_server.hpp"

using namespace test_harness;

test_harness_server::test_harness_server(const uint16_t port): mPort(port) {
    sockaddr_in serverAddress;
    // create socket handler
    mServerSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (mServerSockfd == -1) {
        throw SocketException(SocketStatus::SOCKET_CREATE_FAILURE, "[ERROR]: Failed to create socket");
    }

    test_harness_logger::getLogger().debug("Socket created");

    // create server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to the address
    if (bind(mServerSockfd, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        throw SocketException(SocketStatus::SOCKET_BIND_FAILURE, "[ERROR]: Failed to bind socket at port " + std::to_string(port));
    }

    // listen for incoming connections
    if (listen(mServerSockfd, 3) < 0) {
        throw SocketException(SocketStatus::SOCKET_LISTEN_FAILURE, "[ERROR]: Failed to listen");
    }
    test_harness_logger::getLogger().log(test_harness_logger::level::INFO, "Bind and listen to the port " + std::to_string(port));
}

test_harness_server::test_harness_server(const char* socket) {
    sockaddr_un addr;
    // create socket handler
    this->mSockfd = atoi(socket);
}

uint16_t test_harness_server::getPort() const { return mPort; }

test_harness_server::~test_harness_server() { ::close(mServerSockfd); }

bool test_harness_server::accept() {
    sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    this->mSockfd = ::accept(mServerSockfd, (sockaddr*)&clientAddress, &clientAddressLen);
    if (this->mSockfd >= 0) {
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddress.sin_addr, clientIP, INET_ADDRSTRLEN);
        uint16_t clientPort = ntohs(clientAddress.sin_port);
        test_harness_logger::getLogger().log(test_harness_logger::level::INFO, "Accepted connection from " + std::string(clientIP) + ":" + std::to_string(clientPort));
        return true;
    } else {
        throw SocketException(SocketStatus::SOCKET_ACCEPT_FAILURE, "[ERROR]: Failed to accept connection");
        return false;
    }
}

void test_harness_server::close() {
    if (this->mSockfd != -1) {
        ::close(this->mSockfd);
    }
    if (mServerSockfd != -1) {
        ::close(mServerSockfd);
    }
}

void test_harness_server::closeConnection() {
    if (this->mSockfd != -1) {
        ::close(this->mSockfd);
    }
}
