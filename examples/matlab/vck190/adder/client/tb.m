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

function host(xclbin_path, num_iterations)
    arguments
        xclbin_path (1, 1) string
        num_iterations (1, 1) {mustBeInteger, mustBePositive} = 1
    end

    var = getenv("TEST_HARNESS_REPO_PATH");
    if isempty(var)
        error('Please set the environment variable TEST_HARNESS_REPO_PATH to the root directory of the test harness repository');
    end
    addpath(append(var, '/lib/matlab/'));
    addpath(append(var, '/include/matlab/'));

    % Call member functions
    num_values = 65536 * num_iterations;
    num_repetitions = 1;
    num_delay = 0;

    num_err = 0;

    a = cast(linspace(-65536, 65536, num_values), 'int32');
    b = ones(num_values, 1, 'int32');
    c = zeros(num_values, 1, 'int32');

    mgr_client = test_harness(xclbin_path, {'gr'}, 'vck190');

    args = {};
    args = [args; test_harness_args(channel_index.PLIO_01_TO_AIE, num_values * 4, num_repetitions, num_delay, a)];
    args = [args; test_harness_args(channel_index.PLIO_03_TO_AIE, num_values * 4, num_repetitions, num_delay, b)];
    args = [args; test_harness_args(channel_index.PLIO_02_FROM_AIE, num_values * 4, num_repetitions, num_delay, c)];

    % Call function mode
    mgr_client.runAIEGraph(0, num_iterations);
    mgr_client.runTestHarness(test_mode.FUNC_MODE, args);  
    mgr_client.waitForRes();  
    mgr_client.printPerf();
    delay = mgr_client.getDelayBetween(channel_index.PLIO_01_TO_AIE, channel_index.PLIO_02_FROM_AIE);
    disp("The delay between PLIO_01_TO_AIE and PLIO_02_FROM_AIE is "+delay+" clock cycles.")

    c = args(3).data;

    for i = 1:num_values
        if c(i) ~= b(i) + a(i)
            num_err = num_err + 1;
            fprintf('c[%d] = %d, b[%d] = %d, a[%d] = %d\n', i, c(i), i, b(i), i, a(i));
        end
    end

    if num_err > 0
        fprintf('There are %d errors\n', num_err);
        error('Test failed');
    end
    disp('Test passed');

    % Call performance mode
    new_args = {};
    new_args = [new_args; test_harness_args(channel_index.PLIO_01_TO_AIE, num_values * 4, num_repetitions, num_delay, a)];
    new_args = [new_args; test_harness_args(channel_index.PLIO_03_TO_AIE, num_values * 4, num_repetitions, num_delay, b)];
    new_args = [new_args; test_harness_args(channel_index.PLIO_02_FROM_AIE, num_values * 4, num_repetitions, num_delay, c)];

    mgr_client.runAIEGraph(0, num_iterations);
    mgr_client.runTestHarness(test_mode.PERF_MODE, new_args);  
    mgr_client.waitForRes();  
    mgr_client.printPerf();
    delay = mgr_client.getDelayBetween(channel_index.PLIO_01_TO_AIE, channel_index.PLIO_02_FROM_AIE);
    disp("The delay between PLIO_01_TO_AIE and PLIO_02_FROM_AIE is "+delay+" clock cycles.")

    c = new_args(3).data;

    if mgr_client.isResultValid()
        for i = 1:num_values
            if c(i) ~= b(i) + a(i)
                num_err = num_err + 1;
                fprintf('c[%d] = %d, b[%d] = %d, a[%d] = %d\n', i, c(i), i, b(i), i, a(i));
            end
        end
    else
        fprintf('Result is invalid\n');
    end

    if num_err > 0
        fprintf('There are %d errors\n', num_err);
        error('Test failed');
    end

    disp('Test passed');
end