/*
 * MIT License
 *
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
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

#ifndef _TEST_HARNESS_MGR_HPP_
#define _TEST_HARNESS_MGR_HPP_

/*
 * xrt native api based manager
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <cstring>
#include "xrt.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_bo.h"
#include "xrt/xrt_graph.h"

#define MAX_MEM_CAPACITY (512 * 1024)

enum TestMode { FUNC_MODE = 0, PERF_MODE, REP_MODE };

namespace test_harness {

struct plKernelArg {
    unsigned int arg_idx;
    bool isBO;
    unsigned int size_in_byte;
    char* data;
    unsigned int scalar;
    bool need_realloc;
};

class fastXM {
   private:
    xrtDeviceHandle d_hdl;
    xuid_t uuid;
    std::vector<xrtKernelHandle> pl_k_hdl;
    std::vector<std::vector<xrtBufferHandle> > bo_hdl;
    std::vector<std::vector<char*> > bo_src_ptr;
    std::vector<std::vector<unsigned int> > bo_src_size;
    std::vector<std::vector<xrtRunHandle> > pl_k_r_hdl;
    std::vector<xrtGraphHandle> g_hdl;

   public:
    fastXM(unsigned int device_index,
           std::string xclbin_file_path,
           std::vector<std::string> pl_kernel_name,
           std::vector<std::string> graph_name) {
        d_hdl = xrtDeviceOpen(device_index);
        if (d_hdl == nullptr) throw std::runtime_error("[ERROR]: No valid device handle found! Failed to init device!");
        xrtDeviceLoadXclbinFile(d_hdl, xclbin_file_path.c_str());
        xrtDeviceGetXclbinUUID(d_hdl, uuid);

        for (int i = 0; i < pl_kernel_name.size(); i++) {
            pl_k_hdl.push_back(xrtPLKernelOpen(d_hdl, uuid, pl_kernel_name[i].c_str()));
        }
        bo_hdl.resize(pl_k_hdl.size());
        bo_src_ptr.resize(pl_k_hdl.size());
        bo_src_size.resize(pl_k_hdl.size());
        pl_k_r_hdl.resize(pl_k_hdl.size());

        for (int i = 0; i < graph_name.size(); i++) {
            xrtGraphHandle tmp = xrtGraphOpen(d_hdl, uuid, graph_name[i].c_str());
            if (tmp == NULL) {
                std::cout << "[ERROR]: graph '" << graph_name[i] << "' is not found! Failed to init device!" << std::endl;
                std::cout << "Please provide check and use correct graph name!" << std::endl;
                exit(1);
            } else {
                g_hdl.push_back(tmp);
            }
        }
    }

    void runPL(unsigned int kernel_index, std::vector<plKernelArg> argument) {
        const int& kk = kernel_index;
        auto tmp_pl_k_r_hdl = xrtRunOpen(pl_k_hdl[kk]);

        xrtBufferHandle tmp_bo_hdl, sub_bo_hdl;
        char* tmp_bo_ptr;
        char* sub_bo_ptr;
        for (int i = 0; i < argument.size(); i++) {
            if (argument[i].isBO) {
                if(argument[i].need_realloc) {
                    tmp_bo_hdl = xrtBOAlloc(d_hdl, argument[i].size_in_byte, 0,
                                                 xrtKernelArgGroupId(pl_k_hdl[kk], argument[i].arg_idx));
                    tmp_bo_ptr = reinterpret_cast<char*>(xrtBOMap(tmp_bo_hdl));
                    memcpy(tmp_bo_ptr, argument[i].data, argument[i].size_in_byte);
                    xrtBOSync(tmp_bo_hdl, XCL_BO_SYNC_BO_TO_DEVICE, argument[i].size_in_byte, 0);
                    xrtRunSetArg(tmp_pl_k_r_hdl, argument[i].arg_idx, tmp_bo_hdl);
                    bo_hdl[kk].push_back(tmp_bo_hdl);
                    bo_src_ptr[kk].push_back(argument[i].data);
                    bo_src_size[kk].push_back(argument[i].size_in_byte);
                } else {
                    sub_bo_hdl = xrtBOSubAlloc(tmp_bo_hdl, argument[i].size_in_byte, 0);
                    sub_bo_ptr = reinterpret_cast<char*>(xrtBOMap(sub_bo_hdl));
                    memcpy(sub_bo_ptr, argument[i].data, argument[i].size_in_byte);
                    xrtBOSync(sub_bo_hdl, XCL_BO_SYNC_BO_TO_DEVICE, argument[i].size_in_byte, 0);
                    xrtRunSetArg(tmp_pl_k_r_hdl, argument[i].arg_idx, sub_bo_hdl);
                    bo_hdl[kk].push_back(sub_bo_hdl);
                    bo_src_ptr[kk].push_back(argument[i].data);
                    bo_src_size[kk].push_back(argument[i].size_in_byte);
                }
            } else {
                xrtRunSetArg(tmp_pl_k_r_hdl, argument[i].arg_idx, argument[i].scalar);
            }
        }
        pl_k_r_hdl[kk].push_back(tmp_pl_k_r_hdl);
        xrtRunStart(tmp_pl_k_r_hdl);
    }

    void runGraph(unsigned int g_idx, unsigned int iters) { xrtGraphRun(g_hdl[g_idx], iters); }

    void waitDone(int graph_timeout_millisec) {
        for (int i = 0; i < pl_k_r_hdl.size(); i++) {
            for (int j = 0; j < pl_k_r_hdl[i].size(); j++) {
                xrtRunWait(pl_k_r_hdl[i][j]);
            }
        }
        for (int i = 0; i < g_hdl.size(); i++) {
            xrtGraphWaitDone(g_hdl[i], graph_timeout_millisec);
        }
    }

    void fetchRes() {
        for (int i = 0; i < bo_hdl.size(); i++) {
            for (int j = 0; j < bo_hdl[i].size(); j++) {
                xrtBOSync(bo_hdl[i][j], XCL_BO_SYNC_BO_FROM_DEVICE, bo_src_size[i][j], 0);
                memcpy(bo_src_ptr[i][j], xrtBOMap(bo_hdl[i][j]), bo_src_size[i][j]);
            }
        }
    }

    void clear() {
        for (int i = 0; i < pl_k_hdl.size(); i++) {
            for (int j = 0; j < pl_k_r_hdl.size(); j++) {
                xrtRunClose(pl_k_r_hdl[i][j]);
            }
            for (int j = 0; j < bo_hdl[i].size(); j++) {
                xrtBOFree(bo_hdl[i][j]);
            }
            bo_src_ptr.clear();
            bo_src_size.clear();
        }
    }

    virtual ~fastXM() {
        clear();
        for (int i = 0; i < pl_k_hdl.size(); i++) {
            xrtKernelClose(pl_k_hdl[i]);
        }
        for (int i = 0; i < g_hdl.size(); i++) {
            xrtGraphClose(g_hdl[i]);
        }
        xrtDeviceClose(d_hdl);
    }
};

enum channel_index {
    PLIO_01_TO_AIE,
    PLIO_02_TO_AIE,
    PLIO_03_TO_AIE,
    PLIO_04_TO_AIE,
    PLIO_05_TO_AIE,
    PLIO_06_TO_AIE,
    PLIO_07_TO_AIE,
    PLIO_08_TO_AIE,
    PLIO_09_TO_AIE,
    PLIO_10_TO_AIE,
    PLIO_11_TO_AIE,
    PLIO_12_TO_AIE,
    PLIO_13_TO_AIE,
    PLIO_14_TO_AIE,
    PLIO_15_TO_AIE,
    PLIO_16_TO_AIE,
    PLIO_17_TO_AIE,
    PLIO_18_TO_AIE,
    PLIO_19_TO_AIE,
    PLIO_20_TO_AIE,
    PLIO_21_TO_AIE,
    PLIO_22_TO_AIE,
    PLIO_23_TO_AIE,
    PLIO_24_TO_AIE,
    PLIO_25_TO_AIE,
    PLIO_26_TO_AIE,
    PLIO_27_TO_AIE,
    PLIO_28_TO_AIE,
    PLIO_29_TO_AIE,
    PLIO_30_TO_AIE,
    PLIO_31_TO_AIE,
    PLIO_32_TO_AIE,
    PLIO_33_TO_AIE,
    PLIO_34_TO_AIE,
    PLIO_35_TO_AIE,
    PLIO_36_TO_AIE,
    PLIO_01_FROM_AIE,
    PLIO_02_FROM_AIE,
    PLIO_03_FROM_AIE,
    PLIO_04_FROM_AIE,
    PLIO_05_FROM_AIE,
    PLIO_06_FROM_AIE,
    PLIO_07_FROM_AIE,
    PLIO_08_FROM_AIE,
    PLIO_09_FROM_AIE,
    PLIO_10_FROM_AIE,
    PLIO_11_FROM_AIE,
    PLIO_12_FROM_AIE,
    PLIO_13_FROM_AIE,
    PLIO_14_FROM_AIE,
    PLIO_15_FROM_AIE,
    PLIO_16_FROM_AIE,
    PLIO_17_FROM_AIE,
    PLIO_18_FROM_AIE,
    PLIO_19_FROM_AIE,
    PLIO_20_FROM_AIE,
    PLIO_21_FROM_AIE,
    PLIO_22_FROM_AIE,
    PLIO_23_FROM_AIE,
    PLIO_24_FROM_AIE,
    PLIO_25_FROM_AIE,
    PLIO_26_FROM_AIE,
    PLIO_27_FROM_AIE,
    PLIO_28_FROM_AIE,
    PLIO_29_FROM_AIE,
    PLIO_30_FROM_AIE,
    PLIO_31_FROM_AIE,
    PLIO_32_FROM_AIE,
    PLIO_33_FROM_AIE,
    PLIO_34_FROM_AIE,
    PLIO_35_FROM_AIE,
    PLIO_36_FROM_AIE
};

struct test_harness_args {
    channel_index idx;
    unsigned int size_in_byte;
    unsigned int repetition;
    uint64_t delay;
    unsigned int in_byte_offt;
    unsigned int out_byte_offt;
    char* data;
};

template <unsigned int N, unsigned int W, unsigned int D>
class test_harness_mgr : public fastXM {
   public:
    /*
     * test_harness_mgr() - Loads the xclbin on the devide and initializes the
     * various test harness runtime objects
     *
     * @tparam N
     * Number of PLIOs (for input/output respectively)
     * @tparam W
     * PLIO bit-width in bytes
     * @tparam D
     * Depth of the 128-bit width cache in each PLIO channel
     *
     * @param device_index
     * The device id of the testing board, typically it will be zero
     * @param xclbin_file_path
     * The name, including its full path, to the xclbin file to be tested
     * @param pl_krl_name
     * The vector of PL kernel names in the pre-built XSA and packaged in the xclbin file
     * @param graph_name
     * The vector of graph names in the libadf.a and packaged in the xclbin file
     * @param test_mode
     * Testing mode, including functional, performance, and repetition
     * @param device_type
     * vck190 or vek280, others are not supported
     */
    test_harness_mgr(unsigned int device_index, std::string xclbin_file_path, std::vector<std::string> pl_krl_name, std::vector<std::string> graph_name, uint64_t test_mode, std::string device_type) : fastXM(device_index, xclbin_file_path, pl_krl_name, graph_name) {
        mode = test_mode;
        device.assign(device_type);
        if (device_type.compare("vck190") == 0) {
            if (test_mode == FUNC_MODE) {
                to_aie_ptr = (char*)malloc(MAX_MEM_CAPACITY);
                from_aie_ptr = (char*)malloc(MAX_MEM_CAPACITY);
            } else if (test_mode == PERF_MODE || test_mode == REP_MODE) {
                to_aie_ptr = (char*)malloc(N * W * D);
                from_aie_ptr = (char*)malloc(N * W * D);
            } else {
                std::cout << "[ERROR]: Only functional, performance, and repetition testing modes are supported on vck190 device.\n";
            }
        } else if (device_type.compare("vek280") == 0) {
            if (test_mode == REP_MODE) {
                to_aie_ptr = (char*)malloc(N * W * D);
                from_aie_ptr = (char*)malloc(N * W * D);
            } else {
                std::cout << "[ERROR]: Only repetition mode is supported on vek280 device.\n";
            }
        } else {
            std::cout << "[ERROR]: Only vck190 or vek280 are supported by AIE test harness.\n";
        }
       
        cfg_ptr = (uint64_t*)malloc((N * 2 * 4 + 1) * sizeof(uint64_t));
        perf_ptr = (uint64_t*)malloc(N * 2 * sizeof(uint64_t));
        graph_started = false;

        reset_cfg();
    }

    void reset_cfg() {
        for (int i = 0; i < (N * 2 * 4 + 1); i++) {
            cfg_ptr[i] = 0;
        }
        for (int i = 0; i < N * 2; i++) {
            perf_ptr[i] = 0;
        }
        args_rec.clear();
    }

    void runTestHarness(std::vector<test_harness_args> args) {
        reset_cfg();

        if (!graph_started) {
            std::cout << "Warning: you're trying to call 'runTestHarness' before "
                         "calling 'runAIEGraph'."
                      << std::endl;
            std::cout << "This might lead to result of 'printPerf' to be fluctuated." << std::endl;
            std::cout << "It is strongly recommended to call 'runAIEGraph' before "
                         "'runTestHarness'."
                      << std::endl;
        }

        bool frame_size_valid = true;
        for (int i = 0; i < args.size(); i++) {
            std::cout << "Check frame size CH[" << i << "]:    ";
            if (mode == FUNC_MODE) {
                if (args[i].idx <= PLIO_36_TO_AIE) {
                    if (!check_frame_size(args[i].size_in_byte, args[i].in_byte_offt)) {
                        frame_size_valid = false;
                    }
                } else {
                    if (!check_frame_size(args[i].size_in_byte, args[i].out_byte_offt)) {
                        frame_size_valid = false;
                    }
                }
            } else {
                if (!check_frame_size(args[i].size_in_byte)) {
                    frame_size_valid = false;
                }
            }
        }

        if (frame_size_valid) {
            cfg_ptr[0] = mode;
            if ((device.compare("vck190") == 0) && mode == FUNC_MODE) {
                for (int i = 0; i < args.size(); i++) {
                    args_rec.push_back(args[i]);
                    {
                        int bias = 0;
                        int chn = 0;
                        if (args[i].idx <= PLIO_36_TO_AIE) {
                            bias = 0;
                            chn = args[i].idx - PLIO_01_TO_AIE;
                        } else {
                            bias = 4;
                            chn = args[i].idx - PLIO_01_FROM_AIE;
                        }

                        cfg_ptr[1 + N * bias + 0 * N + chn] = args[i].delay;
                        cfg_ptr[1 + N * bias + 1 * N + chn] = args[i].size_in_byte / W;
                        cfg_ptr[1 + N * bias + 2 * N + chn] = args[i].repetition;
                        cfg_ptr[1 + N * bias + 3 * N + chn] = args[i].in_byte_offt / W;
                    }

                    {
                        if (args[i].idx <= PLIO_36_TO_AIE) {
                            memcpy(to_aie_ptr + args[i].in_byte_offt, args[i].data, args[i].size_in_byte);
                        }
                    }
                }

                this->runPL(0, {{0, true, (N * 2 * 4 + 1) * sizeof(uint64_t), (char*)cfg_ptr, 0, true},
                                {1, true, N * 2 * sizeof(uint64_t), (char*)perf_ptr, 0, true},
                                {2, true, MAX_MEM_CAPACITY, to_aie_ptr, 0, true},
                                {3, true, MAX_MEM_CAPACITY, to_aie_ptr, 0, false},
                                {4, true, MAX_MEM_CAPACITY, to_aie_ptr, 0, false},
                                {5, true, MAX_MEM_CAPACITY, to_aie_ptr, 0, false},
                                {6, true, MAX_MEM_CAPACITY, to_aie_ptr, 0, false},
                                {7, true, MAX_MEM_CAPACITY, to_aie_ptr, 0, false},
                                {8, true, MAX_MEM_CAPACITY, from_aie_ptr, 0, true},
                                {9, true, MAX_MEM_CAPACITY, from_aie_ptr, 0, false},
                                {10, true, MAX_MEM_CAPACITY, from_aie_ptr, 0, false},
                                {11, true, MAX_MEM_CAPACITY, from_aie_ptr, 0, false},
                                {12, true, MAX_MEM_CAPACITY, from_aie_ptr, 0, false},
                                {13, true, MAX_MEM_CAPACITY, from_aie_ptr, 0, false}});
            } else if ((device.compare("vck190") == 0) && mode == PERF_MODE) {
                for (int i = 0; i < args.size(); i++) {
                    args_rec.push_back(args[i]);
                    {
                        int bias = 0;
                        int chn = 0;
                        if (args[i].idx <= PLIO_36_TO_AIE) {
                            bias = 0;
                            chn = args[i].idx - PLIO_01_TO_AIE;
                        } else {
                            bias = 4;
                            chn = args[i].idx - PLIO_01_FROM_AIE;
                        }

                        cfg_ptr[1 + N * bias + 0 * N + chn] = args[i].delay;
                        cfg_ptr[1 + N * bias + 1 * N + chn] = args[i].size_in_byte / W;
                        cfg_ptr[1 + N * bias + 2 * N + chn] = args[i].repetition;
                        cfg_ptr[1 + N * bias + 3 * N + chn] = args[i].in_byte_offt / W;
                    }

                    {
                        if (args[i].idx <= PLIO_36_TO_AIE) {
                            int bias = (args[i].idx - PLIO_01_TO_AIE) * W * D;
                            if (args[i].size_in_byte > W * D) {
                                memcpy(to_aie_ptr + bias, args[i].data, W * D);
                            } else {
                                memcpy(to_aie_ptr + bias, args[i].data, args[i].size_in_byte);
                            }
                        }
                    }
                }

                this->runPL(0, {{0, true, (N * 2 * 4 + 1) * sizeof(uint64_t), (char*)cfg_ptr, 0, true},
                                {1, true, N * 2 * sizeof(uint64_t), (char*)perf_ptr, 0, true},
                                {2, true, N * W * D, to_aie_ptr, 0, true},
                                {3, true, N * W * D, from_aie_ptr, 0, true}});
            } else if (mode == REP_MODE) {
                for (int i = 0; i < args.size(); i++) {
                    args_rec.push_back(args[i]);
                    {
                        int bias = 0;
                        int chn = 0;
                        if (args[i].idx <= PLIO_36_TO_AIE) {
                            bias = 0;
                            chn = args[i].idx - PLIO_01_TO_AIE;
                        } else {
                            bias = 4;
                            chn = args[i].idx - PLIO_01_FROM_AIE;
                        }

                        cfg_ptr[1 + N * bias + 0 * N + chn] = args[i].delay;
                        cfg_ptr[1 + N * bias + 1 * N + chn] = args[i].size_in_byte / W;
                        cfg_ptr[1 + N * bias + 2 * N + chn] = args[i].repetition;
                        cfg_ptr[1 + N * bias + 3 * N + chn] = args[i].in_byte_offt / W;
                    }

                    {
                        if (args[i].idx <= PLIO_36_TO_AIE) {
                            int bias = (args[i].idx - PLIO_01_TO_AIE) * W * D;
                            if (args[i].size_in_byte > W * D) {
                                memcpy(to_aie_ptr + bias, args[i].data, W * D);
                            } else {
                                memcpy(to_aie_ptr + bias, args[i].data, args[i].size_in_byte);
                            }
                        }
                    }
                }

                this->runPL(0, {{0, true, (N * 2 * 4 + 1) * sizeof(uint64_t), (char*)cfg_ptr, 0, true},
                                {1, true, N * 2 * sizeof(uint64_t), (char*)perf_ptr, 0, true},
                                {2, true, N * W * D, to_aie_ptr, 0, true},
                                {3, true, N * W * D, from_aie_ptr, 0, true}});
            } else {
                std::cout << "[ERROR]: Please use the valid device (VCK190/VEK280) with the supported testing modes.\n";
                std::cout << "[INFO]: FUNC_MODE, PERF_MODE, and REP_MODE are supported on VCK190.\n";
                std::cout << "[INFO]: Only REP_MODE is supported on VEK280.\n";
            }
        } else {
            std::cout << "[ERROR]: Arguments for test harness is not valid, won't run "
                         "test harness!"
                      << std::endl;
        }
    }

    void runAIEGraph(unsigned int g_idx, unsigned int iters) {
        runGraph(g_idx, iters);
        graph_started = true;
    }

    void waitForRes(int graph_timeout_millisec) {
        this->waitDone(graph_timeout_millisec);
        graph_started = false;
        this->fetchRes();
        for (int i = 0; i < args_rec.size(); i++) {
            if (mode == FUNC_MODE) {
                if (args_rec[i].idx >= PLIO_01_FROM_AIE) {
                    memcpy(args_rec[i].data, from_aie_ptr + args_rec[i].out_byte_offt, args_rec[i].size_in_byte);
                }
            } else if (mode == REP_MODE) {
                if (args_rec[i].idx >= PLIO_01_FROM_AIE) {
                    int bias = (args_rec[i].idx - PLIO_01_FROM_AIE) * W * D;
                    memcpy(args_rec[i].data, from_aie_ptr + bias, args_rec[i].size_in_byte);
                }
            } else { // mode == PERF_MODE
                if (args_rec[i].idx >= PLIO_01_FROM_AIE) {
                    int bias = (args_rec[i].idx - PLIO_01_FROM_AIE) * W * D;
                    if (args_rec[i].size_in_byte > W * D) {
                        memcpy(args_rec[i].data, from_aie_ptr + bias, W * D);
                    } else {
                        memcpy(args_rec[i].data, from_aie_ptr + bias, args_rec[i].size_in_byte);
                    }
                }
            }
        }
        printPerf();
    }

    void printPerf() {
        if (mode == FUNC_MODE) {
            std::cout << "[INFO]: Performance profiling is not valid in functional testing mode.\n";
        } else if (mode == REP_MODE || mode == PERF_MODE) {
            for (int i = 0; i < args_rec.size(); i++) {
                if (args_rec[i].idx <= PLIO_36_TO_AIE) {
                    int chn = args_rec[i].idx - PLIO_01_TO_AIE;
                    std::cout << "PLIO_" << chn + 1 << "_TO_AIE starts from cycle[" << args_rec[i].delay
                              << "], ends at cycle[" << perf_ptr[chn] << "]." << std::endl;
                }
            }
            for (int i = 0; i < args_rec.size(); i++) {
                if (args_rec[i].idx >= PLIO_01_FROM_AIE) {
                    int chn = args_rec[i].idx - PLIO_01_FROM_AIE;
                    std::cout << "PLIO_" << chn + 1 << "_FROM_AIE starts from cycle[" << args_rec[i].delay
                              << "], ends at cycle[" << perf_ptr[chn + N] << "]." << std::endl;
                }
            }
        }
    }

    ~test_harness_mgr() {
        free(cfg_ptr);
        free(perf_ptr);
        free(to_aie_ptr);
        free(from_aie_ptr);
    }

   private:
    bool graph_started;
    uint64_t* cfg_ptr;
    uint64_t* perf_ptr;
    char* to_aie_ptr;
    char* from_aie_ptr;
    uint64_t mode;
    std::string device;
    std::vector<test_harness_args> args_rec;

    // for REP_MODE + PERF_MODE
    bool check_frame_size(unsigned int sz) {
        std::cout << "N = " << N << ", W = " << W << ", D = " << D << std::endl;
        if ((mode == REP_MODE) && (sz > (W * D))) {
            std::cout << "[ERROR]: Frame size " << sz << " > " << D * W
                      << ", is not valid because it's beyond capacity of one channel." << std::endl;
            return false;
        }
        if (sz % W != 0) {
            std::cout << "[ERROR]: Frame size = " << sz << ", is not valid because it is not divisible by 16"
                      << std::endl;
            return false;
        }
        return true;
    }

    // for FUNC_MODE only
    bool check_frame_size(unsigned int sz, unsigned int offt) {
        std::cout << "N = " << N << ", W = " << W << ", D = " << D << std::endl;
        if (offt + sz > MAX_MEM_CAPACITY) {
            std::cout << "[ERROR]: Exceeding maximum memory capacity " << (double)(MAX_MEM_CAPACITY) / 1024.0 / 1024.0 << " MB. Requiring " << (double)(offt + sz) / 1024.0 / 1024.0 << "MB.\n";
            return false;
        }
        if (sz % W != 0) {
            std::cout << "[ERROR]: Frame size = " << sz << ", is not valid because it is not divisible by 16"
                      << std::endl;
            return false;
        }
        return true;
    }
};
}
#endif
