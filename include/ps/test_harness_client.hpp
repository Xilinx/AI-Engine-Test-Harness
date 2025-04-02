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

#ifndef _TEST_HARNESS_CLIENT_HPP_
#define _TEST_HARNESS_CLIENT_HPP_

#include <cstdint>
#include <string>
#include "test_harness_sockets.hpp"

namespace test_harness {

class test_harness_client : public test_harness_sockets {
    public:
        test_harness_client(const std::string& ip_port_str);
        test_harness_client(const std::string& ip, uint16_t port);
        bool isValidIpV4(const std::string& ip);
        uint16_t getPort() const { return mPort; }
        std::string getIp() const { return mIp; }
    private:
        void connect();
    private:
        uint16_t mPort;
        std::string mIp;
};

inline bool test_harness_client::isValidIpV4(const std::string& ip) {
        std::string ip_copy = ip;
        size_t pos = 0;
        int count = 0;
        while ((pos = ip_copy.find(".")) != std::string::npos) {
                std::string token = ip_copy.substr(0, pos);
                if (token.empty()) {
                return false;
                }
                int num = std::stoi(token);
                if (num < 0 || num > 255) {
                return false;
                }
                ip_copy.erase(0, pos + 1);
                count++;
        }
        if (count != 3) {
                return false;
        }
        if (ip_copy.empty()) {
                return false;
        }
        int num = std::stoi(ip_copy);
        if (num < 0 || num > 255) {
                return false;
        }
        return true;
}

inline test_harness_client::test_harness_client(const std::string& ip_port_str) {
    auto pos = ip_port_str.find(":");
    mIp = ip_port_str.substr(0, pos);

    if (!isValidIpV4(mIp)) {
        throw SocketException(SocketStatus::INVALID_IP_ADDRESS, "[ERROR]: Invalid IP address: " + mIp);
    }
    
    try {
        mPort = std::stoi(ip_port_str.substr(pos + 1));
    } catch (std::exception& e) {
        throw SocketException(SocketStatus::INVALID_PORT_NUMBER, "[ERROR]: Invalid port number " + ip_port_str.substr(pos + 1));
    }
    connect();
}

inline test_harness_client::test_harness_client(const std::string& ip, uint16_t port): mPort(port), mIp(ip) {
    connect();
}

inline void test_harness_client::connect() {
    this->mSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->mSockfd < 0) {
        throw SocketException(SocketStatus::SOCKET_CREATE_FAILURE, "[ERROR]: Error creating socket");
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(mPort);
    serv_addr.sin_addr.s_addr = inet_addr(mIp.c_str());
    if (::connect(this->mSockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        throw SocketException(SocketStatus::SOCKET_CONNECT_FAILURE, "[ERROR]: Connection to " + mIp + ':' + std::to_string(mPort) +" failed");
    }    
}


} // namespace test_harness

#endif