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

#ifndef _TEST_HARNESS_SOCKETS_HPP_
#define _TEST_HARNESS_SOCKETS_HPP_

#include "test_harness_exception.hpp"
#include "test_harness_logging.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/un.h>

namespace fs=std::filesystem;

namespace test_harness {

class test_harness_sockets {
   public:
    test_harness_sockets() = default;
    ssize_t recvData(char* buffer, ssize_t d_size);
    ssize_t sendData(const char* buffer, size_t d_size);

    ssize_t sendReq(const std::string& str);
    std::string recvReq();

    template <typename T> ssize_t sendData(const T& data);
    template <typename T> ssize_t recvData(T& buffer);

    ssize_t sendData(const std::string& str);
    ssize_t recvData(std::string& str);

    ssize_t sendData(const fs::path& path);
    ssize_t recvData(fs::path& path);

    template <typename T, typename Allocator = std::allocator<T>> ssize_t sendVec(const std::vector<T, Allocator>& buffer);
    template <typename T, typename Allocator = std::allocator<T>> ssize_t recvVec(std::vector<T, Allocator>& buffer);

    ssize_t sendVec(const std::vector<std::string>& buffer);
    ssize_t recvVec(std::vector<std::string>& buffer);

    ssize_t sendVec(const std::vector<fs::path>& buffer);
    ssize_t recvVec(std::vector<fs::path>& buffer);

    ssize_t sendVec(const std::vector<bool>& buffer);
    ssize_t recvVec(std::vector<bool>& buffer);

    int getSocketFd() const { return mSockfd; }

    void setTimeOut(uint64_t milliseconds) {
        timeval timeout;
        timeout.tv_sec = milliseconds / 1000;
        timeout.tv_usec = (milliseconds % 1000) * 1000;
        setsockopt(mSockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    }
    
    ~test_harness_sockets();

   protected:
    std::string prettyByte(size_t size);
    int mSockfd;
};

template <typename T>
ssize_t test_harness_sockets::sendData(const T& data) {
    return sendData((char*)&data, sizeof(T));
}

template <typename T>
ssize_t test_harness_sockets::recvData(T& buffer) {
    return recvData((char*)&buffer, sizeof(T));
}

template <typename T, typename Allocator>
ssize_t test_harness_sockets::sendVec(const std::vector<T, Allocator>& buffer) {
    ssize_t totalSize = 0;
    size_t size = buffer.size();
    totalSize += sendData((const char*)&size, sizeof(size_t));
    totalSize += sendData((char*)buffer.data(), buffer.size() * sizeof(T));
    return totalSize;
}

template <typename T, typename Allocator>
ssize_t test_harness_sockets::recvVec(std::vector<T, Allocator>& buffer) {
    ssize_t totalSize = 0;
    ssize_t size = 0;
    totalSize += recvData((char*)&size, sizeof(size_t));
    buffer.resize(size);
    totalSize += recvData((char*)buffer.data(), buffer.size() * sizeof(T));
    return totalSize;
}

inline ssize_t test_harness_sockets::sendVec(const std::vector<bool>& buffer) {
    ssize_t totalSize = 0;
    size_t size = buffer.size();
    totalSize += sendData((const char*)&size, sizeof(size_t));
    for(size_t i = 0; i < size; i++) {
        bool val = buffer[i];
        totalSize += sendData((const char*)&val, sizeof(bool));
    }
    return totalSize;
}

inline ssize_t test_harness_sockets::recvVec(std::vector<bool>& buffer) {
    ssize_t totalSize = 0;
    size_t size = 0;
    totalSize += recvData((char*)&size, sizeof(size_t));
    buffer.resize(size);
    for(size_t i = 0; i < size; i++) {
        bool val;
        totalSize += recvData((char*)&val, sizeof(bool));
        buffer[i] = val;
    }
    return totalSize;
}

inline std::string test_harness_sockets::prettyByte(size_t size) {
    std::string unit[] = {"B", "KB", "MB", "GB", "TB"};
    size_t i = 0;
    while (size >= 1024 && i < 4) {
        size /= 1024;
        i++;
    }
    return std::to_string(size) + unit[i];
}

inline ssize_t test_harness_sockets::recvData(char* buffer, ssize_t d_size) {
    test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, "Receiving " + prettyByte(d_size));
    ssize_t size = 0;
    while (size < d_size) {
        auto received = recv(mSockfd, buffer + size, d_size - size, 0);
        if (received < 0) {
            test_harness_logger::getLogger().log(test_harness_logger::level::ERROR, "Failed to receive data");
            throw SocketException(SocketStatus::SOCKET_RECEIVE_FAILURE, "[ERROR]: Failed to receive data");
        } else if (received == 0) {            
            throw SocketException(SocketStatus::SOCKET_CLOSE_FAILURE, "[ERROR]: Connection closed");
        } else {
            size += received;
        }
    }
    test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, "Totally received " + prettyByte(size));
    if(size != d_size) {
        throw SocketException(SocketStatus::SOCKET_RECEIVE_FAILURE, "[ERROR]: Failed to receive all data");
    }
    return size;
}

inline ssize_t test_harness_sockets::sendData(const char* buffer, size_t d_size) {
    test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, "Sending " + prettyByte(d_size));
    size_t size = send(mSockfd, buffer, d_size, 0);
    if(size != d_size) {
        throw SocketException(SocketStatus::SOCKET_SEND_FAILURE, "[ERROR]: Failed to send all data");
    }

    test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, "Totally sent " + prettyByte(size));
    return size;
}

inline ssize_t test_harness_sockets::sendData(const std::string &str) {
    ssize_t size = str.size();
    return sendData((const char*)&size, sizeof(ssize_t)) + sendData(str.c_str(), size);
}

inline ssize_t test_harness_sockets::recvData(std::string &str) {
    ssize_t size = 0;
    size_t rsize = recvData((char*)&size, sizeof(ssize_t));
    if(rsize < sizeof(ssize_t)) {
        return rsize;
    }
    str.resize(size);
    rsize += recvData(str.data(), size);
    return rsize;
}

inline ssize_t test_harness_sockets::sendData(const fs::path& filepath) {
    ssize_t size = 0;
    if(!fs::exists(filepath)) {
        throw SocketException(SocketStatus::FILE_OPEN_FAILURE, "[ERROR]: File " + filepath.string() + " does not exist");
    }
    if(!fs::is_regular_file(filepath)) {
        throw SocketException(SocketStatus::FILE_OPEN_FAILURE, "[ERROR]: " + filepath.string() + " is not a regular file");
    }

    std::fstream stream(filepath.string(), std::ios::in | std::ios::binary);
    if (!stream.is_open()) {
        throw SocketException(SocketStatus::FILE_OPEN_FAILURE, "[ERROR]: Unable to open file " + filepath.string());
    }

    std::vector<uint8_t> file((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    
    test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, "Sending file: " + filepath.string());

    const std::string& filename = filepath.filename().string();
    size += sendData(filename);
    size += sendVec(file);
    stream.close();
    return size;
}

inline ssize_t test_harness_sockets::recvData(fs::path& filepath) {
    ssize_t size = 0;
    std::string filename;
    size += recvData(filename);

    if(filename.empty()) {
        throw SocketException(SocketStatus::FILE_OPEN_FAILURE, "[ERROR]: Invalid filename");
    }
    if(filepath.empty()) {
        filepath = fs::current_path();
    } else if(!fs::exists(filepath)) {
        fs::create_directories(filepath);
    }
    
    if(fs::is_directory(filepath)) {
        filepath = filepath / fs::path(filename);
    } else {
        filepath = filepath.parent_path() / fs::path(filename);
    }

    std::fstream stream(filepath.string(), std::ios::out | std::ios::binary);
    if (!stream.is_open()) {
        throw SocketException(SocketStatus::FILE_OPEN_FAILURE, "[ERROR]: Unable to open file " + filepath.string());
    }
    test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, "Receiving file: " + filepath.string());
    std::vector<uint8_t> filecontent;
    size += recvVec(filecontent);
    stream.write((char*)filecontent.data(), filecontent.size());
    stream.close();
    return size;
}

inline ssize_t test_harness_sockets::sendReq(const std::string &str) {
    size_t s_size = sendData(str);
    if (s_size != str.size() + sizeof(ssize_t)) {
        throw SocketException(SocketStatus::SOCKET_SEND_FAILURE, "[ERROR]: Failed to send request " + str);
    } else {
        test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, "Sent request with " + prettyByte(s_size));
    }
    return s_size;
}

inline std::string test_harness_sockets::recvReq() {
    std::string res;
    auto r_size = recvData(res);
    if (r_size < 0) {
        throw SocketException(SocketStatus::SOCKET_RECEIVE_FAILURE, "[ERROR]: Failed to receive data");
    } else if (r_size == 0) {
        throw SocketException(SocketStatus::SOCKET_CLOSE_FAILURE, "[ERROR]: Connection closed");
    } else {
        test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, "Received request with " + prettyByte(r_size));
        return res;
    }
}

inline ssize_t test_harness_sockets::sendVec(const std::vector<std::string>& buffer) {
    ssize_t totalSize = 0;
    size_t size = buffer.size();
    test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, "Sending string vector with size: " + std::to_string(size));
    totalSize += sendData((const char*)&size, sizeof(size_t));
    for(size_t i = 0; i < size; i++) {
        totalSize += sendData(buffer[i]);
    }
    return totalSize;
}

inline ssize_t test_harness_sockets::recvVec(std::vector<std::string>& buffer) {
    ssize_t totalSize = 0;
    size_t size = 0;
    totalSize += recvData((char*)&size, sizeof(size_t));
    buffer.resize(size);
    for(size_t i = 0; i < size; i++) {
        totalSize += recvData(buffer[i]);
    }
    return totalSize;
}

inline ssize_t test_harness_sockets::sendVec(const std::vector<fs::path>& files) {
    ssize_t totalSize = sendData(files.size());
    test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, "Sending file vector with size: " + std::to_string(files.size()));
    for(auto &path : files) {
        totalSize += sendData(path);
    }
    return totalSize;
}

inline ssize_t test_harness_sockets::recvVec(std::vector<fs::path>& files) {
    size_t size = 0;
    ssize_t totalSize = recvData(size);
    files.resize(size);
    test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, "Receiving file vector with size: " + std::to_string(size));
    for(auto &path : files) {
        totalSize += recvData(path);
    }
    return totalSize;
}

inline test_harness_sockets::~test_harness_sockets() {
    if (mSockfd != -1) {
        close(mSockfd);
    }
}


} // namespace test_harness

#endif // _TEST_HARNESS_SOCKETS_HPP_