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