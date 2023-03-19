/*
 * Copyright 2022 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _OCL_HPP_
#define _OCL_HPP_

#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_bo.h"

/*
 * xrt native api based manager
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include "xrt.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_graph.h"

namespace vck190_test_harness {

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
        if (d_hdl == nullptr) throw std::runtime_error("No valid device handle found.");
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
            g_hdl.push_back(xrtGraphOpen(d_hdl, uuid, graph_name[i].c_str()));
        }
    }

    void runPL(unsigned int kernel_index, std::vector<plKernelArg> argument) {
        const int& kk = kernel_index;
        auto tmp_pl_k_r_hdl = xrtRunOpen(pl_k_hdl[kk]);

        for (int i = 0; i < argument.size(); i++) {
            if (argument[i].isBO) {
                auto tmp_bo_hdl = xrtBOAlloc(d_hdl, argument[i].size_in_byte, 0,
                                             xrtKernelArgGroupId(pl_k_hdl[kk], argument[i].arg_idx));
                char* tmp_bo_ptr = reinterpret_cast<char*>(xrtBOMap(tmp_bo_hdl));
                memcpy(tmp_bo_ptr, argument[i].data, argument[i].size_in_byte);
                xrtBOSync(tmp_bo_hdl, XCL_BO_SYNC_BO_TO_DEVICE, argument[i].size_in_byte, 0);
                xrtRunSetArg(tmp_pl_k_r_hdl, argument[i].arg_idx, tmp_bo_hdl);
                bo_hdl[kk].push_back(tmp_bo_hdl);
                bo_src_ptr[kk].push_back(argument[i].data);
                bo_src_size[kk].push_back(argument[i].size_in_byte);
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
    Column_12_TO_AIE,
    Column_13_TO_AIE,
    Column_14_TO_AIE,
    Column_15_TO_AIE,
    Column_16_TO_AIE,
    Column_17_TO_AIE,
    Column_18_TO_AIE,
    Column_19_TO_AIE,
    Column_20_TO_AIE,
    Column_21_TO_AIE,
    Column_22_TO_AIE,
    Column_23_TO_AIE,
    Column_24_TO_AIE,
    Column_25_TO_AIE,
    Column_26_TO_AIE,
    Column_27_TO_AIE,
    Column_28_FROM_AIE,
    Column_29_FROM_AIE,
    Column_30_FROM_AIE,
    Column_31_FROM_AIE,
    Column_32_FROM_AIE,
    Column_33_FROM_AIE,
    Column_34_FROM_AIE,
    Column_35_FROM_AIE,
    Column_36_FROM_AIE,
    Column_37_FROM_AIE,
    Column_38_FROM_AIE,
    Column_39_FROM_AIE,
    Column_40_FROM_AIE,
    Column_41_FROM_AIE,
    Column_42_FROM_AIE,
    Column_43_FROM_AIE
};

struct test_harness_args {
    channel_index idx;
    unsigned int size_in_byte;
    unsigned int repetition;
    uint64_t delay;
    char* data;
};

class test_harness_mgr : public fastXM {
   public:
    test_harness_mgr(unsigned int device_index, std::string xclbin_file_path, std::vector<std::string> graph_name)
        : fastXM(device_index, xclbin_file_path, {"vck190_test_harness"}, graph_name) {
        N = 16;
        W = 16;
        D = 8192;

        cfg_ptr = (uint64_t*)malloc(N * 2 * 3 * sizeof(uint64_t));
        to_aie_ptr = (char*)malloc(N * W * D);
        from_aie_ptr = (char*)malloc(N * W * D);
        perf_ptr = (uint64_t*)malloc(N * 2 * sizeof(uint64_t));

        reset_cfg();
    }

    void reset_cfg() {
        for (int i = 0; i < N * 2 * 3; i++) {
            cfg_ptr[i] = 0;
        }
        for (int i = 0; i < N * 2; i++) {
            perf_ptr[i] = 0;
        }
        args_rec.clear();
    }

    void runTestHarness(std::vector<test_harness_args> args) {
        reset_cfg();

        for (int i = 0; i < args.size(); i++) {
            args_rec.push_back(args[i]);
            {
                int bias = 0;
                int chn = 0;
                if (args[i].idx <= Column_27_TO_AIE) {
                    bias = 0;
                    chn = args[i].idx - Column_12_TO_AIE;
                } else {
                    bias = 3;
                    chn = args[i].idx - Column_28_FROM_AIE;
                }

                cfg_ptr[N * bias + 0 * N + chn] = args[i].delay;
                cfg_ptr[N * bias + 1 * N + chn] = args[i].size_in_byte / W;
                cfg_ptr[N * bias + 2 * N + chn] = args[i].repetition;
            }

            {
                if (args[i].idx <= Column_27_TO_AIE) {
                    int bias = (args[i].idx - Column_12_TO_AIE) * W * D;
                    memcpy(to_aie_ptr + bias, args[i].data, args[i].size_in_byte);
                }
            }
        }

        this->runPL(0, {{0, true, N * 2 * 3 * sizeof(uint64_t), (char*)cfg_ptr, 0},
                        {1, true, N * 2 * sizeof(uint64_t), (char*)perf_ptr, 0},
                        {2, true, N * W * D, to_aie_ptr, 0},
                        {3, true, N * W * D, from_aie_ptr, 0}});
    }

    void runAIEGraph(unsigned int g_idx, unsigned int iters) { runGraph(g_idx, iters); }

    void waitForRes(int graph_timeout_millisec) {
        this->waitDone(graph_timeout_millisec);
        this->fetchRes();
        for (int i = 0; i < args_rec.size(); i++) {
            if (args_rec[i].idx >= Column_28_FROM_AIE) {
                int bias = (args_rec[i].idx - Column_28_FROM_AIE) * W * D;
                memcpy(args_rec[i].data, from_aie_ptr + bias, args_rec[i].size_in_byte);
            }
        }
        printPerf();
    }

    void printPerf() {
        for (int i = 0; i < args_rec.size(); i++) {
            if (args_rec[i].idx <= Column_27_TO_AIE) {
                int chn = args_rec[i].idx - Column_12_TO_AIE;
                std::cout << "Column_" << chn + 12 << "_TO_AIE starts from cycle[" << args_rec[i].delay
                          << "], ends at cycle[" << perf_ptr[chn] << "]." << std::endl;
            }
        }
        for (int i = 0; i < args_rec.size(); i++) {
            if (args_rec[i].idx >= Column_27_TO_AIE) {
                int chn = args_rec[i].idx - Column_28_FROM_AIE;
                std::cout << "Column_" << chn + 28 << "_FROM_AIE starts from cycle[" << args_rec[i].delay
                          << "], ends at cycle[" << perf_ptr[chn + N] << "]." << std::endl;
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
    unsigned int N;
    unsigned int W;
    unsigned int D;
    uint64_t* cfg_ptr;
    uint64_t* perf_ptr;
    char* to_aie_ptr;
    char* from_aie_ptr;
    std::vector<test_harness_args> args_rec;
};
}
#endif
