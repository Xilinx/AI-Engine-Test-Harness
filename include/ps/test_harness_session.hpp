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

#ifndef __TEST_HARNESS_SESSION_HPP__
#define __TEST_HARNESS_SESSION_HPP__

#include <sys/types.h>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <atomic>

// The test harness APIs
#include "test_harness_mgr_server.hpp"

namespace test_harness {

class test_harness_session
{
public:
    test_harness_session(test_harness_server *server, uint64_t monitorInterval);
    ~test_harness_session() = default;
    void run();
    void monitor();
    void setActive(bool active);
    bool isActive();
    void setTimeOut(uint64_t timeout);

private:
    test_harness_server *server;
    std::unique_ptr<test_harness_mgr_server> mgr;
    std::atomic<uint64_t> monitorInterval;
    std::atomic<bool> mActive;
    std::atomic<bool> mDone;
    std::atomic<bool> mRunning;
};
} // namespace test_harness


#endif // __TEST_HARNESS_SESSION_HPP__