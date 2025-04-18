% MIT License
%
% Copyright (C) 2024-2025 Advanced Micro Devices, Inc.
%
% Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
%
% The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
%
% Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.
%

classdef test_harness < handle
    properties (Hidden)
        c_ptr
    end
    methods (Access=public)
        function obj = test_harness(xclbin_path, graph_names, device_name, timeout, device_index)
            arguments
                xclbin_path (1, 1) string;
                graph_names (1, :) string;
                device_name (1, 1) string  = 'vck190';
                timeout (1, 1) uint32 = 0;
                device_index (1, 1) uint32 = 0;
            end
            obj.c_ptr = uint64(0);
            exists = exist(xclbin_path, 'file');
            if exists == 0
                error('xclbin file not found');
            end
            obj.c_ptr = m_test_harness(xclbin_path, graph_names, device_name, timeout, device_index);
        end

        function runAIEGraph(obj, g_id, num_iter)
            arguments
                obj (1, 1) test_harness;
                g_id (1, 1) uint32;
                num_iter (1, 1) uint32;
            end
            m_runAIEGraph(obj.c_ptr, g_id, num_iter);
        end

        function runTestHarness(obj, mode, args, timeout)
            arguments
                obj (1, 1) test_harness;
                mode (1, 1) test_mode;
                args (:, 1) test_harness_args;
                timeout (1, 1) uint32 = 0;
            end
            m_runTestHarness(obj.c_ptr, uint8(mode), args, timeout);
        end

        function waitForRes(obj, milliseconds, all)
            arguments
                obj (1, 1) test_harness;
                milliseconds (1, 1) uint32 = 0;
                all (1, 1) uint32 = 0;
            end
            m_waitForRes(obj.c_ptr, milliseconds, all);
        end

        function res = isResultValid(obj, idx)
            arguments
                obj (1, 1) test_harness;
                idx (1, 1) int32 = -1;
            end
            res = m_isResultValid(obj.c_ptr, idx);
        end

        function printPerf(obj, idx)
            arguments
                obj (1, 1) test_harness;
                idx (1, 1) int32 = -1;
            end
            m_printPerf(obj.c_ptr, idx);
        end

        function res = getDelayBetween(obj, idx0, idx1, t_id)
            arguments
                obj (1, 1) test_harness;
                idx0 (1, 1) channel_index;
                idx1 (1, 1) channel_index;
                t_id (1, 1) int32 = -1;
            end
            res = m_getDelayBetween(obj.c_ptr, idx0, idx1, t_id);
        end
    end
    methods (Access = private)
        function delete(obj)
            m_del_test_harness(obj.c_ptr);
            c_ptr = 0;
        end

    end
end