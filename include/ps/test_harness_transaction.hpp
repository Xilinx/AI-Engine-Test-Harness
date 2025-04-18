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

#ifndef _TEST_HARNESS_TRANSACTION_HPP_
#define _TEST_HARNESS_TRANSACTION_HPP_

#include <atomic>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <map>
#include <vector>
#include <cstring>
#include <sstream>

#include "test_harness_logging.hpp"
#include "test_harness_utils.hpp"
#include "test_harness_exception.hpp"

namespace test_harness {

enum TransactionStatus: uint8_t {
    TRANSACTION_CREATED,
    TRANSACTION_READY,
    TRANSACTION_RUNNING,
    TRANSACTION_TIMEOUT,
    TRANSACTION_DONE
};

class test_harness_transactions {
    public:
        test_harness_transactions(unsigned int trans_id, TestMode mode, 
                                    unsigned int num_channels, 
                                    unsigned int word_size, 
                                    unsigned int depth,
                                    unsigned int memWidth,
                                    uint32_t timeout = 0) : trans_id(trans_id), mode(mode), num_channels(num_channels), 
                                                            word_size(word_size), depth(depth), mMemWidth(memWidth),
                                                            result_valid(true), mTimeOut(timeout) {
            reset();
        }

        bool isResultValid();
        void printPerf();
        int64_t getDelayBwtween(channel_index, channel_index);
        void handleInputs(const std::vector<test_harness_args>& args);
        void handleOutputs();

        void setStatus(TransactionStatus status) {
            this->status = status;
        }
        TransactionStatus getStatus() {
            return this->status;
        }

        void setTimeOut(uint32_t timeout) {
            this->mTimeOut = timeout;
        }

        uint32_t getTimeOut() {
            return this->mTimeOut;
        }

        size_t getMemoryUsage();

    private:
        void clearData();
        void reset();
        void check_arguments(const std::vector<test_harness_args>& args);

    public:
        unsigned trans_id;
        std::vector<uint64_t> cfg_ptr;
        std::vector<uint64_t> perf_ptr;
        std::vector<char> to_aie_ptr;
        std::vector<char> from_aie_ptr;
        const unsigned int mMonitorTimeout = 1000;

    private:
        std::atomic<TransactionStatus> status;
        TestMode mode;
        std::vector<unsigned int> from_mem_word_offset;

        std::vector<test_harness_args> args_rec;
        std::map<channel_index, std::pair<uint64_t, uint64_t>> perf_map;

        unsigned int num_channels;
        unsigned int word_size;
        unsigned int depth;
        unsigned int mMemWidth;
        
        bool result_valid;
        std::atomic<uint32_t> mTimeOut;

};

inline size_t test_harness_transactions::getMemoryUsage() {
    size_t size = 0;
    size += cfg_ptr.size() * sizeof(uint64_t);
    size += perf_ptr.size() * sizeof(uint64_t);
    size += to_aie_ptr.size() * sizeof(char);
    size += from_aie_ptr.size() * sizeof(char);
    return size;
}

inline void test_harness_transactions::handleInputs(const std::vector<test_harness_args>& args) {
    reset();
    check_arguments(args);
    this->cfg_ptr[0] = this->mode;
    
    if (this->mode == FUNC_MODE) {
        this->result_valid = true;
        size_t total_to_byte = 0;
        size_t total_from_byte = 0;
        for (size_t i = 0; i < args_rec.size(); i++) {        
            // to accumulate the buffer size for each group of streams for FUNC_MODE
            if ((args_rec[i].idx >= PLIO_01_TO_AIE) && (args_rec[i].idx < PLIO_01_TO_AIE + num_channels)) {
                total_to_byte += (args_rec[i].size_in_byte + mMemWidth - 1) / mMemWidth * mMemWidth;
            } else if ((args_rec[i].idx >= PLIO_01_FROM_AIE) && (args_rec[i].idx < PLIO_01_FROM_AIE + num_channels)) {
                total_from_byte += (args_rec[i].size_in_byte + mMemWidth - 1) / mMemWidth * mMemWidth;
            }
            
            if(args_rec[i].repetition != 1) {
                throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS, "repetition is not supported for the function mode in the test harness.");
            }
        }
        size_t to_mem_word_offt = 0;
        size_t from_mem_word_offt = 0;
        this->to_aie_ptr.resize(total_to_byte);
        this->from_aie_ptr.resize(total_from_byte);
        for (size_t i = 0; i < args_rec.size(); i++) {
            int bias = 0;
            int chn = 0;
            if (args_rec[i].idx >= PLIO_01_TO_AIE && args_rec[i].idx < PLIO_01_TO_AIE + num_channels) {
                bias = 0;
                chn = args_rec[i].idx - PLIO_01_TO_AIE;
                this->cfg_ptr[1 + num_channels * bias + 0 * num_channels + chn] = args_rec[i].delay;
                this->cfg_ptr[1 + num_channels * bias + 1 * num_channels + chn] = args_rec[i].size_in_byte / word_size;
                this->cfg_ptr[1 + num_channels * bias + 2 * num_channels + chn] = args_rec[i].repetition;  // should be always be 1
                this->cfg_ptr[1 + num_channels * bias + 3 * num_channels + chn] = to_mem_word_offt;
                memcpy(this->to_aie_ptr.data() + to_mem_word_offt * mMemWidth, args_rec[i].data, args_rec[i].size_in_byte);
                to_mem_word_offt += (args_rec[i].size_in_byte + mMemWidth - 1) / mMemWidth;
            } else if (args_rec[i].idx >= PLIO_01_FROM_AIE && args_rec[i].idx < PLIO_01_FROM_AIE + num_channels) {
                bias = 4;
                chn = args_rec[i].idx - PLIO_01_FROM_AIE;
                this->cfg_ptr[1 + num_channels * bias + 0 * num_channels + chn] = args_rec[i].delay;
                this->cfg_ptr[1 + num_channels * bias + 1 * num_channels + chn] = args_rec[i].size_in_byte / word_size;
                this->cfg_ptr[1 + num_channels * bias + 2 * num_channels + chn] = args_rec[i].repetition;  // should be always be 1
                this->cfg_ptr[1 + num_channels * bias + 3 * num_channels + chn] = from_mem_word_offt;
                this->from_mem_word_offset[chn] = from_mem_word_offt;
                from_mem_word_offt += (args_rec[i].size_in_byte + mMemWidth - 1) / mMemWidth;
            } else {
                assert("Wrong channel index is used in the test harness.");
            }
        }
    } else if (this->mode == PERF_MODE) {
        assert(mMemWidth % word_size == 0);
        assert((word_size * depth) % mMemWidth == 0);
        this->to_aie_ptr.resize(num_channels * word_size * depth);
        this->from_aie_ptr.resize(num_channels * word_size * depth);
        for (size_t i = 0; i <  args_rec.size(); i++) {
            int bias = 0;
            int chn = 0;
            if (args_rec[i].idx >= PLIO_01_TO_AIE && args_rec[i].idx < PLIO_01_TO_AIE + num_channels) {
                bias = 0;
                chn = args_rec[i].idx - PLIO_01_TO_AIE;
                auto offsets = word_size * depth / mMemWidth * chn;
                if (args_rec[i].size_in_byte > word_size * depth) {
                    this->result_valid = false;
                    memcpy(this->to_aie_ptr.data() + offsets * mMemWidth, args_rec[i].data, word_size * depth);
                } else {
                    memcpy(this->to_aie_ptr.data() + offsets * mMemWidth, args_rec[i].data, args_rec[i].size_in_byte);
                }
                this->cfg_ptr[1 + num_channels * bias + 0 * num_channels + chn] = args_rec[i].delay;
                this->cfg_ptr[1 + num_channels * bias + 1 * num_channels + chn] = args_rec[i].size_in_byte / word_size;
                this->cfg_ptr[1 + num_channels * bias + 2 * num_channels + chn] = args_rec[i].repetition;
                this->cfg_ptr[1 + num_channels * bias + 3 * num_channels + chn] = offsets;
            } else if(args_rec[i].idx >= PLIO_01_FROM_AIE && args_rec[i].idx < PLIO_01_FROM_AIE + num_channels) {
                bias = 4;
                chn = args_rec[i].idx - PLIO_01_FROM_AIE;
                auto offsets = word_size * depth / mMemWidth * chn;
                this->cfg_ptr[1 + num_channels * bias + 0 * num_channels + chn] = args_rec[i].delay;
                this->cfg_ptr[1 + num_channels * bias + 1 * num_channels + chn] = args_rec[i].size_in_byte / word_size;
                this->cfg_ptr[1 + num_channels * bias + 2 * num_channels + chn] = args_rec[i].repetition;
                this->cfg_ptr[1 + num_channels * bias + 3 * num_channels + chn] = offsets;
                this->from_mem_word_offset[chn] = offsets;
            }
        }
    }
    status = TRANSACTION_READY;
}

inline void test_harness_transactions::handleOutputs() {
    for (size_t i = 0; i < args_rec.size(); i++) {
        if (this->mode == FUNC_MODE) {
            if((args_rec[i].idx >= PLIO_01_TO_AIE) && (args_rec[i].idx < num_channels + PLIO_01_TO_AIE)) {
                unsigned chn = args_rec[i].idx - PLIO_01_TO_AIE;
                perf_map[args_rec[i].idx] = std::make_pair(perf_ptr[chn], perf_ptr[chn + num_channels]);
            }
            if ((args_rec[i].idx >= PLIO_01_FROM_AIE) && (args_rec[i].idx < num_channels + PLIO_01_FROM_AIE)) {
                unsigned chn = args_rec[i].idx - PLIO_01_FROM_AIE;
                memcpy(args_rec[i].data, this->from_aie_ptr.data() + this->from_mem_word_offset[chn] * mMemWidth, args_rec[i].size_in_byte);
                perf_map[args_rec[i].idx] = std::make_pair(perf_ptr[chn + 2 * num_channels], perf_ptr[chn + 3 * num_channels]);
            }
        } else if (this->mode == PERF_MODE) {
            if (args_rec[i].idx >= PLIO_01_FROM_AIE && (args_rec[i].idx < num_channels + PLIO_01_FROM_AIE)) {
                unsigned chn = args_rec[i].idx - PLIO_01_FROM_AIE;
                if (args_rec[i].size_in_byte > word_size * depth) {
                    memcpy(args_rec[i].data, this->from_aie_ptr.data() + this->from_mem_word_offset[chn] * mMemWidth, word_size * depth);
                } else {
                    memcpy(args_rec[i].data, this->from_aie_ptr.data() + this->from_mem_word_offset[chn] * mMemWidth, args_rec[i].size_in_byte);
                }
                perf_map[args_rec[i].idx] = std::make_pair(perf_ptr[chn + 2 * num_channels], perf_ptr[chn + 3 * num_channels]);
            }
            if(args_rec[i].idx >= PLIO_01_TO_AIE && (args_rec[i].idx < num_channels + PLIO_01_TO_AIE)) {
                unsigned chn = args_rec[i].idx - PLIO_01_TO_AIE;
                perf_map[args_rec[i].idx] = std::make_pair(perf_ptr[chn], perf_ptr[chn + num_channels]);
            }
        }
    }
    clearData();
}

inline bool test_harness_transactions::isResultValid() {
    if(status == TRANSACTION_TIMEOUT) {
        test_harness_logger::getLogger().log(test_harness_logger::level::WARNING, 
            "The transaction is not valid due to the timeout.");    
        return false;
    } else if (status == TRANSACTION_DONE) {
        return this->result_valid;
    } else {
        test_harness_logger::getLogger().log(test_harness_logger::level::WARNING, 
            "The transaction is not finished yet.");
        return false;
    }
}

inline
void test_harness_transactions::printPerf() {
    if(status == TRANSACTION_TIMEOUT) {
        test_harness_logger::getLogger().log(test_harness_logger::level::WARNING, 
            "The performance information of this transaction is not available due to timeout.");
    } else if (status == TRANSACTION_DONE) {
        for (size_t i = 0; i < args_rec.size(); i++) {
            if (args_rec[i].idx >= PLIO_01_TO_AIE && args_rec[i].idx < PLIO_01_TO_AIE + num_channels) {
                int chn = args_rec[i].idx - PLIO_01_TO_AIE;
                std::stringstream ss;
                ss << "PLIO_" << chn + 1 << "_TO_AIE send the first data at cycle[" << perf_map[args_rec[i].idx].first
                            << "], ends at cycle[" << perf_map[args_rec[i].idx].second << "], and "
                            << "the throughputs is " 
                            << args_rec[i].size_in_byte * args_rec[i].repetition * 312.5e6 / (perf_map[args_rec[i].idx].second - perf_map[args_rec[i].idx].first) / 1024.0 / 1024.0 
                            << " MBps."
                            << std::endl;
                test_harness_logger::getLogger().log(test_harness_logger::level::INFO, ss.str());
            }
        }
        for (size_t i = 0; i < args_rec.size(); i++) {
            if (args_rec[i].idx >= PLIO_01_FROM_AIE && args_rec[i].idx < PLIO_01_FROM_AIE + num_channels) {
                int chn = args_rec[i].idx - PLIO_01_FROM_AIE;
                std::stringstream ss;
                ss << "PLIO_" << chn + 1 << "_FROM_AIE received the first data at cycle[" 
                            << perf_map[args_rec[i].idx].first
                            << "], ends at cycle[" << perf_map[args_rec[i].idx].second << "], and " 
                            << "the throughputs is " 
                            << args_rec[i].size_in_byte * args_rec[i].repetition * 312.5e6 / (perf_map[args_rec[i].idx].second - perf_map[args_rec[i].idx].first) / 1024.0 / 1024.0 
                            << " MBps."
                            << std::endl;
                test_harness_logger::getLogger().log(test_harness_logger::level::INFO, ss.str());
            }
        }
    } else {
        test_harness_logger::getLogger().log(test_harness_logger::level::WARNING, 
            "The performance information of this transaction is not available due to the transaction is not finished yet.");

    }
}

inline int64_t test_harness_transactions::getDelayBwtween(channel_index id_from, channel_index id_to) {
    if (status == TRANSACTION_DONE) {
        if(id_from < PLIO_01_TO_AIE || id_from >= PLIO_01_FROM_AIE + num_channels || (id_to >= PLIO_01_TO_AIE + num_channels && id_to < PLIO_01_FROM_AIE) ) {
            throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS, "Wrong channel index is used.");
        }
        if(id_to < PLIO_01_TO_AIE || id_to >= PLIO_01_FROM_AIE + num_channels || (id_from >= PLIO_01_TO_AIE + num_channels && id_from < PLIO_01_FROM_AIE) ) {
            throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS, "Wrong channel index is used.");
        }
        return perf_map[id_to].first - perf_map[id_from].first;
    } else {
        throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS, "The transaction is not finished yet.");
    }
} 

inline
void test_harness_transactions::check_arguments(const std::vector<test_harness_args>& args) {
    for (const test_harness_args& arg: args) {
        if ((arg.idx >= PLIO_01_TO_AIE && arg.idx < PLIO_01_TO_AIE + num_channels) || (arg.idx >= PLIO_01_FROM_AIE && arg.idx < PLIO_01_FROM_AIE + num_channels)) {
            if (arg.size_in_byte % word_size != 0) {
                throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS, "The size of the data should be multiple of the word size.");
            }
            args_rec.push_back(arg);
        } else {
            throw TestHarnessException(TestHarnessStatus::INVALID_ARGUMENTS, "Wrong channel index is used in the test harness.");
        }
    }
}

inline void test_harness_transactions::clearData() {
    this->to_aie_ptr.clear();
    this->from_aie_ptr.clear();
    this->from_mem_word_offset.clear();
    this->cfg_ptr.clear();
    this->perf_ptr.clear();
}

inline
void test_harness_transactions::reset() {
    clearData();
    mTimeOut = 0;
    args_rec.clear();
    this->cfg_ptr.resize(num_channels * 2 * 4 + 1);
    this->perf_ptr.resize(num_channels * 4);
    from_mem_word_offset.resize(num_channels);

    for (size_t i = 0; i < this->cfg_ptr.size(); i++) {
        this->cfg_ptr[i] = 0;
    }
    for (size_t i = 0; i < this->perf_ptr.size(); i++) {
        this->perf_ptr[i] = 0;
    }
    for (size_t i = 0; i < from_mem_word_offset.size(); i++) {
        this->from_mem_word_offset[i] = 0;
    }
    status = TRANSACTION_CREATED;
    result_valid = true;
}
}
#endif