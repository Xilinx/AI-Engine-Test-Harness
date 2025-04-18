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

classdef test_harness_args
    properties
        channel_id,
        size_in_bytes,
        repetitions,
        delay,
        data
    end
    methods
        function obj = test_harness_args(channel_id, size_in_bytes, repetitions, delay, data)
            arguments
                channel_id (1, 1) channel_index
                size_in_bytes (1, 1) uint64
                repetitions (1, 1) uint64
                delay (1, 1) uint64
                data (1, :)
            end
            obj.channel_id = uint16(channel_id);
            obj.delay = delay;
            obj.repetitions = repetitions;
            obj.size_in_bytes = size_in_bytes;
            obj.data = data;
        end
    end
end