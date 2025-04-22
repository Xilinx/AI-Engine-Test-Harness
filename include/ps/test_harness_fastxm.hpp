/*
 * MIT License
 *
 * Copyright (C) 2023-2025 Advanced Micro Devices, Inc.
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

#ifndef _TEST_HARNESS_FASTXM_HPP_
#define _TEST_HARNESS_FASTXM_HPP_

/*
 * xrt native api based manager
 */

#include "xrt.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_graph.h"
#include "xrt/xrt_bo.h"
#include "experimental/xrt_ip.h"
#include <filesystem>
#include <cstdint>
#include <cstring>
#include "test_harness_logging.hpp"

namespace fs = std::filesystem;

namespace test_harness {

    struct plKernelArg {
        unsigned int arg_idx;
        bool isBO;
        size_t size_in_byte;
        char* data;
        unsigned int scalar;
    };

    class fastXM {
    private:
        xrt::device d_hdl;
        std::vector<xrt::graph> g_hdl;
        std::vector<xrt::kernel> pl_k_hdl;
        std::vector<std::vector<xrt::bo> > bo_hdl;
        std::vector<std::vector<char*> > bo_src_ptr;
        std::vector<std::vector<xrt::run> > pl_k_r_hdl;
        std::vector<xrt::ip> ip_hdl;

    public:
        fastXM(unsigned int device_index,
            const fs::path& pl_xclbin_file_path,
            const fs::path& aie_xclbin_file_path,
            const std::vector<std::string>& pl_kernel_name,
            const std::vector<std::string>& graph_name) {
            d_hdl = xrt::device(device_index);
            test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, 
                                    "Loading PL xclbin: " + pl_xclbin_file_path.string());
            xrt::uuid pl_id = d_hdl.load_xclbin(pl_xclbin_file_path.string());
            test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING,
                                    "PL xclbin loaded with uuid " + std::to_string(pl_id));

            test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING,
                                    "Loading AIE xclbin: " + aie_xclbin_file_path.string());
            xrt::uuid aie_id = d_hdl.load_xclbin(aie_xclbin_file_path.string());
            test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING,
                                    "AIE xclbin loaded with uuid " + std::to_string(aie_id));

            for (int i = 0; i < pl_kernel_name.size(); i++) {
                pl_k_hdl.push_back(xrt::kernel(d_hdl, pl_id, pl_kernel_name[i]));
                ip_hdl.push_back(xrt::ip(d_hdl, pl_id, pl_kernel_name[i]));
            }

            for (int i = 0; i < graph_name.size(); i++) {
                auto h_graph = xrt::graph(d_hdl, aie_id, graph_name[i]);
                g_hdl.push_back(h_graph);
            }
            bo_hdl.resize(pl_k_hdl.size());
            bo_src_ptr.resize(pl_k_hdl.size());
            pl_k_r_hdl.resize(pl_k_hdl.size());
            resetGraphs();
            resetTestHarness();
        }

        fastXM(unsigned int device_index,
            const fs::path& xclbin_file_path,
            const std::vector<std::string>& pl_kernel_name,
            const std::vector<std::string>& graph_name) {
            d_hdl = xrt::device(device_index);
            test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, 
                                    "Loading xclbin: " + xclbin_file_path.string());
            xrt::uuid uuid = d_hdl.load_xclbin(xclbin_file_path.string());
            test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING,
                                    "Xclbin loaded with uuid " + std::to_string(uuid));

            for (int i = 0; i < pl_kernel_name.size(); i++) {
                pl_k_hdl.push_back(xrt::kernel(d_hdl, uuid, pl_kernel_name[i]));
                ip_hdl.push_back(xrt::ip(d_hdl, uuid, pl_kernel_name[i]));
            }

            for (int i = 0; i < graph_name.size(); i++) {
                auto h_graph = xrt::graph(d_hdl, uuid, graph_name[i]);
                g_hdl.push_back(h_graph);
            }
            bo_hdl.resize(pl_k_hdl.size());
            bo_src_ptr.resize(pl_k_hdl.size());
            pl_k_r_hdl.resize(pl_k_hdl.size());
            resetGraphs();
            resetTestHarness();
        }

        void runPL(unsigned int kernel_index, const std::vector<plKernelArg>& argument) {
            const int& kk = kernel_index;
            auto tmp_pl_k_r_hdl = xrt::run(pl_k_hdl[kk]);

            for (int i = 0; i < argument.size(); i++) {
                if (argument[i].isBO) {
                    if (argument[i].size_in_byte) {
                        auto tmp_bo_hdl = xrt::bo(d_hdl, argument[i].size_in_byte, 0, pl_k_hdl[kk].group_id(argument[i].arg_idx));
                        tmp_bo_hdl.write(argument[i].data);
                        tmp_bo_hdl.sync(XCL_BO_SYNC_BO_TO_DEVICE);
                        tmp_pl_k_r_hdl.set_arg(argument[i].arg_idx, tmp_bo_hdl);
                        bo_hdl[kk].push_back(tmp_bo_hdl);
                        bo_src_ptr[kk].push_back(argument[i].data);
                    }
                } else {
                    tmp_pl_k_r_hdl.set_arg(argument[i].arg_idx, argument[i].scalar);
                }
            }
            pl_k_r_hdl[kk].push_back(tmp_pl_k_r_hdl);
            tmp_pl_k_r_hdl.start();
        }

        void runGraph(unsigned int g_idx, unsigned int iters) { g_hdl[g_idx].run(iters); }
        /**
        * update() - Update graph Run Time Parameters.
        *
        * @param port_name
        *  Hierarchical name of RTP port.
        * @param arg
        *  The argument to set.
        */
        template<typename ArgType>
        void writeGraphPort(unsigned int g_idx, const std::string& port_name, ArgType&& arg) {
            g_hdl[g_idx].update(port_name, arg);
        }

        /**
        * read() - Read graph Run Time Parameters value.
        *
        * @param port_name
        *  Hierarchical name of RTP port.
        * @param arg
        *  The RTP value is written to.
        */
        template<typename ArgType>
        void readGraphPort(unsigned int g_idx, const std::string& port_name, ArgType& arg) {
            g_hdl[g_idx].read(port_name, arg);
        }

        void waitDone(const std::chrono::milliseconds&  timeout_millisec) {
            auto res = waitForTestHarness(timeout_millisec);
            if(res)
                waitForGraph(timeout_millisec);
            else {
                resetGraphs();
                resetTestHarness();
            }
        }

        void waitForGraph(const std::chrono::milliseconds& timeout_millisec) {
            for (int i = 0; i < g_hdl.size(); i++) {
                waitForGraph(i, timeout_millisec);
            }
        }

        bool waitForGraph(int g_idx, const std::chrono::milliseconds& timeout_millisec) noexcept {
            try {
                g_hdl[g_idx].wait(timeout_millisec);
            } catch (...) {
                g_hdl[g_idx].reset();
                return false;
            }
            return true;
        }

        bool waitForTestHarness(const std::chrono::milliseconds& timeout_millisec) {
            for (int i = 0; i < pl_k_r_hdl.size(); i++) {
                for (int j = 0; j < pl_k_r_hdl[i].size(); j++) {
                    auto state = pl_k_r_hdl[i][j].wait2((timeout_millisec));
                    if(state == std::cv_status::timeout) {
                        return false;
                    }
                }
            }
            return true;
        }

        void fetchRes() {
            for (int i = 0; i < bo_hdl.size(); i++) {
                for (int j = 0; j < bo_hdl[i].size(); j++) {
                    bo_hdl[i][j].sync(XCL_BO_SYNC_BO_FROM_DEVICE);
                    bo_hdl[i][j].read(bo_src_ptr[i][j]);
                }
                bo_hdl[i].clear();
                bo_src_ptr[i].clear();
            }
        }

        void resetTestHarness() {
            for (int i = 0; i < pl_k_r_hdl.size(); i++) {
                pl_k_r_hdl[i].clear();
            }
            for (int i = 0; i < bo_hdl.size(); i++) {
                bo_hdl[i].clear();
                bo_src_ptr[i].clear();
            }
            for(int i = 0; i < ip_hdl.size(); i++) {
                test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, 
                                    "Resetting IP: " + std::to_string(i));
                unsigned int ctrl = 1 << 8;
                ip_hdl[i].write_register(0, ctrl);
                do {
                    ctrl = ip_hdl[i].read_register(0);
                } while (ctrl & (1 << 8));
                test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, 
                                    "IP: " + std::to_string(i) + " resetted.");
            }
        }

        void resetGraphs() {
            for (int i = 0; i < g_hdl.size(); i++) {
                test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, 
                                    "Aborting graph: " + std::to_string(i));
                g_hdl[i].reset();
                test_harness_logger::getLogger().log(test_harness_logger::level::DEBUGGING, 
                                    "Graph: " + std::to_string(i) + " aborted.");
            }
        }

        virtual ~fastXM() {
            resetGraphs();
            resetTestHarness();
            resetGraphs();
            pl_k_hdl.clear();
            g_hdl.clear();
        }
    };
}


#endif