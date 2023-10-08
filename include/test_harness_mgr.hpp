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

enum TestMode { FUNC_MODE = 0, PERF_MODE, REP_MODE };

namespace test_harness {

struct plKernelArg {
    unsigned int arg_idx;
    bool isBO;
    unsigned int size_in_byte;
    char* data;
    unsigned int scalar;
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

        for (int i = 0; i < argument.size(); i++) {
            if (argument[i].isBO) {
                if (argument[i].size_in_byte) {
                    auto tmp_bo_hdl = xrtBOAlloc(d_hdl, argument[i].size_in_byte, 0,
                                                 xrtKernelArgGroupId(pl_k_hdl[kk], argument[i].arg_idx));
                    char* tmp_bo_ptr = reinterpret_cast<char*>(xrtBOMap(tmp_bo_hdl));
                    memcpy(tmp_bo_ptr, argument[i].data, argument[i].size_in_byte);
                    xrtBOSync(tmp_bo_hdl, XCL_BO_SYNC_BO_TO_DEVICE, argument[i].size_in_byte, 0);
                    xrtRunSetArg(tmp_pl_k_r_hdl, argument[i].arg_idx, tmp_bo_hdl);
                    bo_hdl[kk].push_back(tmp_bo_hdl);
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
     * The name, including its full path, to the xclbin file to be tested.
     * Valid xclbin name list: vck190_test_harness_func, vck190_test_harness_perf, vek280_test_harness.
     * @param graph_name
     * The vector of graph names in the libadf.a and packaged in the xclbin file
     */
    test_harness_mgr(unsigned int device_index, std::string xclbin_file_path, std::vector<std::string> graph_name) : fastXM(device_index, xclbin_file_path, {xclbin_file_path.substr(xclbin_file_path.find_last_of("/") + 1).substr(0, xclbin_file_path.substr(xclbin_file_path.find_last_of("/") + 1).find("."))}, graph_name) {
        graph_started = false;
        result_valid = true;
        
        std::string xclbin_name = xclbin_file_path.substr(xclbin_file_path.find_last_of("/") + 1);
        if (std::string::npos != xclbin_name.find("vek280_test_harness")) {
            mode = REP_MODE;
            device.assign("vek280");
        } else if (std::string::npos != xclbin_name.find("vck190_test_harness_func")) {
            mode = FUNC_MODE;
            device.assign("vck190");
        } else if (std::string::npos != xclbin_name.find("vck190_test_harness_perf")) {
            // REP_MODE by default as it is more conservative than the PERF_MODE, will chagne the testing mode according to the buffer size that is required by the user in runTestHarness
            mode = REP_MODE; 
            device.assign("vck190");
        } else {
            std::cout << "[ERROR]: Please provide the valid xclbin name <vck190_test_harness_func.xclbin/vck190_test_harness_perf.xclbin/vek280_test_harness.xclbin>.\n";
        }
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
        if (!graph_started) {
            std::cout << "[WARNING]: you're trying to call 'runTestHarness' before "
                         "calling 'runAIEGraph'."
                      << std::endl;
            std::cout << "This might lead to result of 'printPerf' to be fluctuated." << std::endl;
            std::cout << "It is strongly recommended to call 'runAIEGraph' before "
                         "'runTestHarness'."
                      << std::endl;
        }
        
        // XXX: vek280 is not supporting PERF_MODE
        // Switch REP_MODE to PERF_MODE for vck190 if frame size is lager than capacity of URAM in each channel
        if ((device == "vck190") && mode == REP_MODE) {
            for (int i = 0; i < args.size(); i++) {
                if (args[i].size_in_byte > (W * D)) {
                    mode = PERF_MODE;
                    result_valid = false;
                    std::cout << "[WARNING]: channel " << i << " is larger than the URAM capacity, the result is not valid in this condition\n";
                }
            }
        }

        bool frame_aligned = true;
        unsigned int total_to_byte[6];
        unsigned int total_from_byte[6];
        for (int i = 0; i < 6; i++) {
            total_to_byte[i] = 0;
            total_from_byte[i] = 0;
        }
        for (int i = 0; i < args.size(); i++) {
            std::cout << "[INFO]: Check frame size CH[" << i << "]:    ";
            if (!check_frame_align(args[i].size_in_byte)) {
                frame_aligned = false;
            }
            // to accumulate the buffer size for each group of streams for FUNC_MODE
            if ((args[i].idx >= PLIO_01_TO_AIE) && (args[i].idx <= PLIO_06_TO_AIE)) {
                total_to_byte[0] += args[i].size_in_byte;
            } else if ((args[i].idx >= PLIO_07_TO_AIE) && (args[i].idx <= PLIO_12_TO_AIE)) {
                total_to_byte[1] += args[i].size_in_byte;
            } else if ((args[i].idx >= PLIO_13_TO_AIE) && (args[i].idx <= PLIO_18_TO_AIE)) {
                total_to_byte[2] += args[i].size_in_byte;
            } else if ((args[i].idx >= PLIO_19_TO_AIE) && (args[i].idx <= PLIO_24_TO_AIE)) {
                total_to_byte[3] += args[i].size_in_byte;
            } else if ((args[i].idx >= PLIO_25_TO_AIE) && (args[i].idx <= PLIO_30_TO_AIE)) {
                total_to_byte[4] += args[i].size_in_byte;
            } else if ((args[i].idx >= PLIO_31_TO_AIE) && (args[i].idx <= PLIO_36_TO_AIE)) {
                total_to_byte[5] += args[i].size_in_byte;
            } else if ((args[i].idx >= PLIO_01_FROM_AIE) && (args[i].idx <= PLIO_06_FROM_AIE)) {
                total_from_byte[0] += args[i].size_in_byte;
            } else if ((args[i].idx >= PLIO_07_FROM_AIE) && (args[i].idx <= PLIO_12_FROM_AIE)) {
                total_from_byte[1] += args[i].size_in_byte;
            } else if ((args[i].idx >= PLIO_13_FROM_AIE) && (args[i].idx <= PLIO_18_FROM_AIE)) {
                total_from_byte[2] += args[i].size_in_byte;
            } else if ((args[i].idx >= PLIO_19_FROM_AIE) && (args[i].idx <= PLIO_24_FROM_AIE)) {
                total_from_byte[3] += args[i].size_in_byte;
            } else if ((args[i].idx >= PLIO_25_FROM_AIE) && (args[i].idx <= PLIO_30_FROM_AIE)) {
                total_from_byte[4] += args[i].size_in_byte;
            } else {
                total_from_byte[5] += args[i].size_in_byte;
            }
        }

        cfg_ptr = (uint64_t*)malloc((N * 2 * 4 + 1) * sizeof(uint64_t));
        perf_ptr = (uint64_t*)malloc(N * 2 * sizeof(uint64_t));
        reset_cfg();
        if (frame_aligned) {
            cfg_ptr[0] = mode;
            if ((device.compare("vck190") == 0) && mode == FUNC_MODE) {
                unsigned int to_byte_offt[6];
                unsigned int from_byte_offt[6];
                for (int i = 0; i < 6; i++) {
                    to_aie_ptr[i] = (char*)malloc(total_to_byte[i]);
                    to_byte_offt[i] = 0;
                    from_aie_ptr[i] = (char*)malloc(total_from_byte[i]);
                    from_byte_offt[i] = 0;
                }
                for (int i = 0; i < args.size(); i++) {
                    args_rec.push_back(args[i]);
                    int bias = 0;
                    int chn = 0;
                    if (args[i].idx <= PLIO_36_TO_AIE) {
                        bias = 0;
                        chn = args[i].idx - PLIO_01_TO_AIE;
                        cfg_ptr[1 + N * bias + 0 * N + chn] = args[i].delay;
                        cfg_ptr[1 + N * bias + 1 * N + chn] = args[i].size_in_byte / W;
                        cfg_ptr[1 + N * bias + 2 * N + chn] = args[i].repetition;
                        if ((args[i].idx >= PLIO_01_TO_AIE) && (args[i].idx <= PLIO_06_TO_AIE)) {
                            cfg_ptr[1 + N * bias + 3 * N + chn] = to_byte_offt[0] / W;
                            memcpy(to_aie_ptr[0] + to_byte_offt[0], args[i].data, args[i].size_in_byte);
                            to_byte_offt[0] += args[i].size_in_byte;
                        } else if ((args[i].idx >= PLIO_07_TO_AIE) && (args[i].idx <= PLIO_12_TO_AIE)) {
                            cfg_ptr[1 + N * bias + 3 * N + chn] = to_byte_offt[1] / W;
                            memcpy(to_aie_ptr[1] + to_byte_offt[1], args[i].data, args[i].size_in_byte);
                            to_byte_offt[1] += args[i].size_in_byte;
                        } else if ((args[i].idx >= PLIO_13_TO_AIE) && (args[i].idx <= PLIO_18_TO_AIE)) {
                            cfg_ptr[1 + N * bias + 3 * N + chn] = to_byte_offt[2] / W;
                            memcpy(to_aie_ptr[2] + to_byte_offt[2], args[i].data, args[i].size_in_byte);
                            to_byte_offt[2] += args[i].size_in_byte;
                        } else if ((args[i].idx >= PLIO_19_TO_AIE) && (args[i].idx <= PLIO_24_TO_AIE)) {
                            cfg_ptr[1 + N * bias + 3 * N + chn] = to_byte_offt[3] / W;
                            memcpy(to_aie_ptr[3] + to_byte_offt[3], args[i].data, args[i].size_in_byte);
                            to_byte_offt[3] += args[i].size_in_byte;
                        } else if ((args[i].idx >= PLIO_25_TO_AIE) && (args[i].idx <= PLIO_30_TO_AIE)) {
                            cfg_ptr[1 + N * bias + 3 * N + chn] = to_byte_offt[4] / W;
                            memcpy(to_aie_ptr[4] + to_byte_offt[4], args[i].data, args[i].size_in_byte);
                            to_byte_offt[4] += args[i].size_in_byte;
                        } else {
                            cfg_ptr[1 + N * bias + 3 * N + chn] = to_byte_offt[5] / W;
                            memcpy(to_aie_ptr[5] + to_byte_offt[5], args[i].data, args[i].size_in_byte);
                            to_byte_offt[5] += args[i].size_in_byte;
                        }
                    } else {
                        bias = 4;
                        chn = args[i].idx - PLIO_01_FROM_AIE;
                        cfg_ptr[1 + N * bias + 0 * N + chn] = args[i].delay;
                        cfg_ptr[1 + N * bias + 1 * N + chn] = args[i].size_in_byte / W;
                        cfg_ptr[1 + N * bias + 2 * N + chn] = args[i].repetition;
                        if ((args[i].idx >= PLIO_01_FROM_AIE) && (args[i].idx <= PLIO_06_FROM_AIE)) {
                            cfg_ptr[1 + N * bias + 3 * N + chn] = from_byte_offt[0];
                            from_byte_offt[0] += args[i].size_in_byte / W;
                        } else if ((args[i].idx >= PLIO_07_FROM_AIE) && (args[i].idx <= PLIO_12_FROM_AIE)) {
                            cfg_ptr[1 + N * bias + 3 * N + chn] = from_byte_offt[1];
                            from_byte_offt[1] += args[i].size_in_byte / W;
                        } else if ((args[i].idx >= PLIO_13_FROM_AIE) && (args[i].idx <= PLIO_18_FROM_AIE)) {
                            cfg_ptr[1 + N * bias + 3 * N + chn] = from_byte_offt[2];
                            from_byte_offt[2] += args[i].size_in_byte / W;
                        } else if ((args[i].idx >= PLIO_19_FROM_AIE) && (args[i].idx <= PLIO_24_FROM_AIE)) {
                            cfg_ptr[1 + N * bias + 3 * N + chn] = from_byte_offt[3];
                            from_byte_offt[3] += args[i].size_in_byte / W;
                        } else if ((args[i].idx >= PLIO_25_FROM_AIE) && (args[i].idx <= PLIO_30_FROM_AIE)) {
                            cfg_ptr[1 + N * bias + 3 * N + chn] = from_byte_offt[4];
                            from_byte_offt[4] += args[i].size_in_byte / W;
                        } else {
                            cfg_ptr[1 + N * bias + 3 * N + chn] = from_byte_offt[5];
                            from_byte_offt[5] += args[i].size_in_byte / W;
                        }
                    }
                }

                this->runPL(0, {{0, true, (N * 2 * 4 + 1) * sizeof(uint64_t), (char*)cfg_ptr, 0},
                                {1, true, N * 2 * sizeof(uint64_t), (char*)perf_ptr, 0},
                                {2, true, total_to_byte[0], to_aie_ptr[0], 0},
                                {3, true, total_to_byte[1], to_aie_ptr[1], 0},
                                {4, true, total_to_byte[2], to_aie_ptr[2], 0},
                                {5, true, total_to_byte[3], to_aie_ptr[3], 0},
                                {6, true, total_to_byte[4], to_aie_ptr[4], 0},
                                {7, true, total_to_byte[5], to_aie_ptr[5], 0},
                                {8, true, total_from_byte[0], from_aie_ptr[0], 0},
                                {9, true, total_from_byte[1], from_aie_ptr[1], 0},
                                {10, true, total_from_byte[2], from_aie_ptr[2], 0},
                                {11, true, total_from_byte[3], from_aie_ptr[3], 0},
                                {12, true, total_from_byte[4], from_aie_ptr[4], 0},
                                {13, true, total_from_byte[5], from_aie_ptr[5], 0}});
            } else if ((device.compare("vck190") == 0) && mode == PERF_MODE) {
                to_aie_ptr[0] = (char*)malloc(N * W * D);
                from_aie_ptr[0] = (char*)malloc(N * W * D);
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
                        cfg_ptr[1 + N * bias + 3 * N + chn] = 0;
                    }

                    {
                        if (args[i].idx <= PLIO_36_TO_AIE) {
                            int bias = (args[i].idx - PLIO_01_TO_AIE) * W * D;
                            if (args[i].size_in_byte > W * D) {
                                memcpy(to_aie_ptr[0] + bias, args[i].data, W * D);
                            } else {
                                memcpy(to_aie_ptr[0] + bias, args[i].data, args[i].size_in_byte);
                            }
                        }
                    }
                }

                this->runPL(0, {{0, true, (N * 2 * 4 + 1) * sizeof(uint64_t), (char*)cfg_ptr, 0},
                                {1, true, N * 2 * sizeof(uint64_t), (char*)perf_ptr, 0},
                                {2, true, N * W * D, to_aie_ptr[0], 0},
                                {3, true, N * W * D, from_aie_ptr[0], 0}});
            } else if (mode == REP_MODE) {
                to_aie_ptr[0] = (char*)malloc(N * W * D);
                from_aie_ptr[0] = (char*)malloc(N * W * D);
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
                        cfg_ptr[1 + N * bias + 3 * N + chn] = 0;
                    }

                    {
                        if (args[i].idx <= PLIO_36_TO_AIE) {
                            int bias = (args[i].idx - PLIO_01_TO_AIE) * W * D;
                            if (args[i].size_in_byte > W * D) {
                                memcpy(to_aie_ptr[0] + bias, args[i].data, W * D);
                            } else {
                                memcpy(to_aie_ptr[0] + bias, args[i].data, args[i].size_in_byte);
                            }
                        }
                    }
                }

                this->runPL(0, {{0, true, (N * 2 * 4 + 1) * sizeof(uint64_t), (char*)cfg_ptr, 0},
                                {1, true, N * 2 * sizeof(uint64_t), (char*)perf_ptr, 0},
                                {2, true, N * W * D, to_aie_ptr[0], 0},
                                {3, true, N * W * D, from_aie_ptr[0], 0}});
            } else {
                std::cout << "[ERROR]: Please use the valid device (VCK190/VEK280) with the supported testing modes.\n";
                std::cout << "[INFO]: FUNC_MODE & PERF_MODE are supported on VCK190.\n";
                std::cout << "[INFO]: Only PERF_MODE (limited capacity) is supported on VEK280.\n";
            }
        } else {
            std::cout << "[ERROR]: Arguments for test harness is not valid, won't run "
                         "test harness!"
                      << std::endl;
            exit(1);
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
        unsigned int out_byte_offt[6];
        for (int i = 0; i < 6; i++) {
            out_byte_offt[i] = 0;
        }
        for (int i = 0; i < args_rec.size(); i++) {
            if (mode == FUNC_MODE) {
                if ((args_rec[i].idx >= PLIO_01_FROM_AIE) && (args_rec[i].idx <= PLIO_06_FROM_AIE)) {
                    memcpy(args_rec[i].data, from_aie_ptr[0] + out_byte_offt[0], args_rec[i].size_in_byte);
                    out_byte_offt[0] += args_rec[i].size_in_byte;
                } else if ((args_rec[i].idx >= PLIO_07_FROM_AIE) && (args_rec[i].idx <= PLIO_12_FROM_AIE)) {
                    memcpy(args_rec[i].data, from_aie_ptr[1] + out_byte_offt[1], args_rec[i].size_in_byte);
                    out_byte_offt[1] += args_rec[i].size_in_byte;
                } else if ((args_rec[i].idx >= PLIO_13_FROM_AIE) && (args_rec[i].idx <= PLIO_18_FROM_AIE)) {
                    memcpy(args_rec[i].data, from_aie_ptr[2] + out_byte_offt[2], args_rec[i].size_in_byte);
                    out_byte_offt[2] += args_rec[i].size_in_byte;
                } else if ((args_rec[i].idx >= PLIO_19_FROM_AIE) && (args_rec[i].idx <= PLIO_24_FROM_AIE)) {
                    memcpy(args_rec[i].data, from_aie_ptr[3] + out_byte_offt[3], args_rec[i].size_in_byte);
                    out_byte_offt[3] += args_rec[i].size_in_byte;
                } else if ((args_rec[i].idx >= PLIO_25_FROM_AIE) && (args_rec[i].idx <= PLIO_30_FROM_AIE)) {
                    memcpy(args_rec[i].data, from_aie_ptr[4] + out_byte_offt[4], args_rec[i].size_in_byte);
                    out_byte_offt[4] += args_rec[i].size_in_byte;
                } else if ((args_rec[i].idx >= PLIO_31_FROM_AIE) && (args_rec[i].idx <= PLIO_36_FROM_AIE)) {
                    memcpy(args_rec[i].data, from_aie_ptr[5] + out_byte_offt[5], args_rec[i].size_in_byte);
                    out_byte_offt[5] += args_rec[i].size_in_byte;
                }
            } else if (mode == REP_MODE) {
                if (args_rec[i].idx >= PLIO_01_FROM_AIE) {
                    int bias = (args_rec[i].idx - PLIO_01_FROM_AIE) * W * D;
                    memcpy(args_rec[i].data, from_aie_ptr[0] + bias, args_rec[i].size_in_byte);
                }
            } else { // mode == PERF_MODE
                if (args_rec[i].idx >= PLIO_01_FROM_AIE) {
                    int bias = (args_rec[i].idx - PLIO_01_FROM_AIE) * W * D;
                    if (args_rec[i].size_in_byte > W * D) {
                        memcpy(args_rec[i].data, from_aie_ptr[0] + bias, W * D);
                    } else {
                        memcpy(args_rec[i].data, from_aie_ptr[0] + bias, args_rec[i].size_in_byte);
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
                    std::cout << "[INFO]: PLIO_" << chn + 1 << "_TO_AIE starts from cycle[" << args_rec[i].delay
                              << "], ends at cycle[" << perf_ptr[chn] << "]." << std::endl;
                }
            }
            for (int i = 0; i < args_rec.size(); i++) {
                if (args_rec[i].idx >= PLIO_01_FROM_AIE) {
                    int chn = args_rec[i].idx - PLIO_01_FROM_AIE;
                    std::cout << "[INFO]: PLIO_" << chn + 1 << "_FROM_AIE starts from cycle[" << args_rec[i].delay
                              << "], ends at cycle[" << perf_ptr[chn + N] << "]." << std::endl;
                }
            }
        }
    }

    ~test_harness_mgr() {
        free(cfg_ptr);
        free(perf_ptr);
        free(to_aie_ptr[0]);
        free(from_aie_ptr[0]);
        if (mode == FUNC_MODE) {
            for(int i = 1; i < 6; i++) {
                free(to_aie_ptr[i]);
                free(from_aie_ptr[i]);
            }
        }
    }

    bool result_valid;

   private:
    bool graph_started;
    uint64_t* cfg_ptr;
    uint64_t* perf_ptr;
    char* to_aie_ptr[6];
    char* from_aie_ptr[6];
    uint64_t mode;
    std::string device;
    std::vector<test_harness_args> args_rec;

    // for REP_MODE + PERF_MODE
    bool check_frame_align(unsigned int sz) {
        std::cout << "N = " << N << ", W = " << W << ", D = " << D << std::endl;
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
