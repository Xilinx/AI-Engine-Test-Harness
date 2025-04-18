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

#ifndef _TEST_HARNESS_LOGGING_HPP_
#define _TEST_HARNESS_LOGGING_HPP_

#include <iostream>
#include <fstream>
#include <mutex>
#include <vector>
#include "test_harness_exception.hpp"

namespace test_harness {
class test_harness_logger {
    public:
        // Get the singleton instance of the test_harness_logger
        static test_harness_logger& getLogger();

        // Enumeration for log levels  
        enum class level {  
            INFO,  
            WARNING,  
            ERROR,  
            DEBUGGING
        }; 

        // Set the log file path  
        void setLogFile(const std::string& filePath);
        // Delete copy constructor and assignment operator to prevent copies  
        test_harness_logger(const test_harness_logger&) = delete;  
        test_harness_logger& operator=(const test_harness_logger&) = delete;  
        // Log a message to standard output and the log file  
        void log(level level, const std::string& message);
        void info(const std::string& message) { log(level::INFO, message); }
        void warning(const std::string& message) { log(level::WARNING, message); }
        void error(const std::string& message) { log(level::ERROR, message); }
        void debug(const std::string& message) { log(level::DEBUGGING, message); }
        // Dump data to a binary file
        template<typename T, typename Alloc> void dumpBinary(level level, const std::string& filename, const std::vector<T, Alloc>& data);
        template<typename T, typename Alloc> void debug(const std::string& filename, const std::vector<T, Alloc>& data) { dumpBinary(level::DEBUGGING, filename, data); }
        template<typename T, typename Alloc> void info(const std::string& filename, const std::vector<T, Alloc>& data) { dumpBinary(level::INFO, filename, data); }
        template<typename T, typename Alloc> void warning(const std::string& filename, const std::vector<T, Alloc>& data) { dumpBinary(level::WARNING, filename, data); }
        template<typename T, typename Alloc> void error(const std::string& filename, const std::vector<T, Alloc>& data) { dumpBinary(level::ERROR, filename, data); }
        
    private:
        // Constructor  
        test_harness_logger() = default;  
        // Destructor  
        ~test_harness_logger();
        // Convert log level to string  
        std::string logLevelToString(level level);
    
        // Get current date/time, format is YYYY-MM-DD.HH:mm:ss  
        std::string currentDateTime();

    private:
        // Mutex to protect the log file  
        std::mutex mMtx;  
        // Log file stream
        std::ofstream mLogFile;
};

inline test_harness_logger& test_harness_logger::getLogger() {  
    static test_harness_logger logger;  
    return logger;  
}

inline void test_harness_logger::setLogFile(const std::string& filePath)  {  
    std::lock_guard<std::mutex> lock(mMtx);  
    if (mLogFile.is_open()) {  
        mLogFile.close();  
    }

    mLogFile.open(filePath, std::ios::out | std::ios::app);  
    
    if (!mLogFile.is_open()) {  
        std::cerr << "Failed to open log file: " << filePath << std::endl;  
    }
}

inline void test_harness_logger::log(level level, const std::string &message) {  
    std::lock_guard<std::mutex> lock(mMtx);
#ifndef DEBUG
    if(level == level::DEBUGGING) {
        return;
    }
#endif
    std::string levelStr = logLevelToString(level);
    std::string logMessage = "[" + currentDateTime() + "] [" + levelStr + "] " + message;  
    std::cout << logMessage  << std::endl;  
    if (mLogFile.is_open()) {  
        mLogFile << logMessage  << std::endl;  
    }
}

template<typename T, typename Alloc>
inline void test_harness_logger::dumpBinary(level level, const std::string &filename, const std::vector<T, Alloc> &data) {
    std::lock_guard<std::mutex> lock(mMtx);
#ifndef DEBUG
    if(level == level::DEBUGGING) {
        return;
    }
#endif
    log(level, "Dumping data to file: " + filename);
    std::fstream(filename, std::ios::out | std::ios::binary).write((char*)data.data(), data.size() * sizeof(T));
}

inline test_harness_logger::~test_harness_logger() {  
    if (mLogFile.is_open()) {  
        mLogFile.close();  
    }  
}

inline std::string test_harness_logger::logLevelToString(level level) {  
    switch (level) {  
        case level::INFO:  
            return "INFO";  
        case level::WARNING:  
            return "WARNING";  
        case level::ERROR:  
            return "ERROR";  
        case level::DEBUGGING:  
            return "DEBUG";  
        default:  
            throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS, "Unknown log level");
    }  
}

inline std::string test_harness_logger::currentDateTime() {  
    std::time_t now = std::time(nullptr);  
    char buf[80];  
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %X", std::localtime(&now));  
    return buf;  
}

}



#endif