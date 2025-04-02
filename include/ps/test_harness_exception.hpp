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

#ifndef __TEST_HARNESS_EXCEPTION_HPP__
#define __TEST_HARNESS_EXCEPTION_HPP__

#include <exception>
#include <string>

namespace test_harness {

enum class SocketStatus: uint8_t {
    SOCKET_CREATE_FAILURE,
    SOCKET_BIND_FAILURE,
    SOCKET_LISTEN_FAILURE,
    SOCKET_ACCEPT_FAILURE,
    SOCKET_CONNECT_FAILURE,
    SOCKET_SEND_FAILURE,
    SOCKET_RECEIVE_FAILURE,
    SOCKET_CLOSE_FAILURE,
    FILE_OPEN_FAILURE,
    INVALID_IP_ADDRESS,
    INVALID_PORT_NUMBER
};

class SocketException : public std::exception {
public:
   
    SocketException(const SocketStatus status, const std::string& message) : status_(status), message_(message) {}
    ~SocketException() throw() {}
    const char* what() const throw() { return message_.c_str(); }
    SocketStatus status() const { return status_; }

private:
    SocketStatus status_;
    std::string message_;
};

enum class TestHarnessStatus: uint8_t {
    TEST_HARNESS_CREATION_FAILURE,
    TEST_HARNESS_INTERNAL_FAILURE,
    TEST_HARNESS_NOT_CREATED_FAILURE,
    TEST_HARNESS_RUN_GRAPH_FAILURE,
    TEST_HARNESS_RUN_FAILURE,
    TEST_HARNESS_WAIT_FOR_RES_FAILURE,
    TEST_HARNESS_GET_LOG_FILES_FAILURE,
    UNKNOWN_REQUEST,
    INVALID_OPERATIONS,
    INVALID_ARGUMENTS
};

class TestHarnessException : public std::exception {
public:
   
    TestHarnessException(const TestHarnessStatus status, const std::string& message) : status_(status), message_(message) {}
    ~TestHarnessException() throw() {}
    const char* what() const throw() { return message_.c_str(); }
    TestHarnessStatus status() const { return status_; }

private:
    TestHarnessStatus status_;
    std::string message_;
};

} // namespace test_harness

#endif // __TEST_HARNESS_EXCEPTION_HPP__