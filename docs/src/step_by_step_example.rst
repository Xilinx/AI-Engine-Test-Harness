.. 
   MIT License

   Copyright (C) 2023 Advanced Micro Devices, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.

.. _step_by_step:

.. highlight:: none

.. toctree::
   :hidden:

********************
Step by Step Example
********************

This step by step example takes a reference design ``examples/vck190/adder_perf`` and shows the commands to build and launch. And the example output is also shown. 
The code specific to test harness features is explained. The commands and code can be easily extended to other examples and user own designs.

Launching the Server Session
==================

Burn and boot with the ``bin/sd_card.img``. In the board, get the IP address and launch the server by::

    <log in Linux with username / passwd: petalinux / petalinux>
    sudo su
    cd /run/media/mmcblk0p1
	ifconfig
    ./run_server.sh

Note that the IP address of the board will be used by client server.

Running an Example in Client Server
==================

Note: Setup the environment first before launching the commands. It can be referred to :ref:`Using the Test Harness <using_the_harness>`
The examples have Makefile ready for building and launching::

    cd examples/vck190/adder_perf
    make all
    make run

Example Output
==================

Here's the example output::

    Using XCLBIN file: vck190_test_harness.xclbin
    Running example ADDER
     - Number of graph iterations         :     1000
     - Number of values                   :  1024000 (4000KB)
     - Number of repetitions              :        1
     - Number of graph iterations (total) :     1000
     - Channel delay                      :        0 cycles
    [2025-**-** 15:16:21] [INFO] Initializing the test harness manager.
    [2025-**-** 15:16:21] [INFO] Connecting to the test harness manager server at 172.16.75.168:8080
    Testing mode: PERF_MODE
    [2025-**-** 15:16:30] [INFO] Running the AIE graph.
    [2025-**-** 15:16:31] [INFO] Running the test harness.
    [2025-**-** 15:16:31] [INFO] Waiting for the result.
    [2025-**-** 15:16:36] [INFO] PLIO_1_TO_AIE send the first data at cycle[0], ends at cycle[303949], and the throughputs is 4016.14 MBps.
    
    [2025-**-** 15:16:36] [INFO] PLIO_1_FROM_AIE received the first data at cycle[631], ends at cycle[304630], and the throughputs is 4015.48 MBps.
    
    [INFO]: Result checking is not valid if test size is beyond the capacity of URAM in each channel.
    TEST PASSED
    [2025-**-** 15:16:36] [INFO] Destroying the test harness manager.
    INFO: TEST PASSED, RC=0

Here, the throughput of every port is reported.

Understanding the Example
=========================

This section describes the source code specific to AIE test harness requirements.

For additional details on the steps described below, refer to the documentation about :ref:`Using the Test Harness <using_the_harness>` and about the :ref:`Software APIs <sw_apis>`.


Test Harness PLIOs
------------------

This example uses 2 input PLIO and 1 output PLIO. The width of each PLIO is set 128 bits, as required by the test harness. We chose PLIO ``PLIO_01_TO_AIE`` and ``PLIO_03_TO_AIE`` to send data to the AI-Engine and PLIO ``PLIO_02_FROM_AIE`` to receive data from the AI-Engine. As explained in the section about :ref:`Placement of PLIOs <plio_placement>`, the PLIO names indicate the generic name and direction of each PLIO. All valid PLIO names can be found in :url_to_repo:`include/test_harness_port_name.hpp`.

.. code-block:: c++

    test_graph() {
        pl_in0 = input_plio::create("PLIO_01_TO_AIE", adf::plio_128_bits, "data/DataIn0.txt");
        pl_in1 = input_plio::create("PLIO_03_TO_AIE", adf::plio_128_bits, "data/DataIn1.txt");
        pl_out = output_plio::create("PLIO_02_FROM_AIE", adf::plio_128_bits, "data/DataOut0.txt");
    }

.. _ps_app:

SW Application
--------------

A SW application running on the client server is necessary to run the test with AIE test harness. The source code for this application is provided in the :url_to_repo:`examples/vck190/adder_perf/ps/host.cpp` file. The application must be developed using the :ref:`test harness software APIs <sw_apis>`.

.. code-block:: c++

    test_harness_mgr_client mgr(xclbin_path, {"gr"}, "vck190");
    // configuration: channel index, size_in_bytes, repetition, delay, pointer to data
    std::vector<test_harness_args> args;
    args.push_back({channel_index(PLIO_01_TO_AIE), num_values * sizeof(int), num_repetitions, num_delay, (char*)a.data()});
    args.push_back({channel_index(PLIO_03_TO_AIE), num_values * sizeof(int), num_repetitions, num_delay, (char*)b.data()});
    args.push_back({channel_index(PLIO_02_FROM_AIE), num_values * sizeof(int), num_repetitions, num_delay, (char*)s.data()});
    std::vector<test_harness::TestMode> modes = {FUNC_MODE, PERF_MODE};
    for(auto mode : modes) {
        mgr.runAIEGraph(0, num_iterations * num_repetitions); //0=graph index in the xclbin, 
        // Start the DMA engine
        mgr.runTestHarness(mode, args);
       // Wait for all DMA transactions and for the AIE graph to finish.
       // The argument is an optional timeout (in millisecond) for the AIE graph.
        mgr.waitForRes(0); //0=Wait for completion
        mgr.printPerf();
        auto is_valid = mgr.isResultValid();
        ......
    }

The application needs one instance of the :cpp:class:`test_harness_mgr_client` class. The name of the PL kernel that is pre-compiled in the XSA. The ``xclbin_path`` specifies the XCLBIN file. The argument ``{"gr"}`` is a vector of string with the name of the graph instantiated in the ``graph.cpp`` file.  The argument ``"vck190"`` specifies the board to be used.

Then, a vector of :cpp:class:`test_harness_args` is created to configure the DMA channels associated with each PLIO used by the AIE graph.
As seen in step #1, the inputs of the graph are mapped to PLIO ``PLIO_01_TO_AIE`` and ``PLIO_03_TO_AIE``, and the output is mapped to ``PLIO_02_FROM_AIE``. We know how many bytes or samples that each iteration of graph ``gr`` consumes or produces. The :cpp:enum:`channel_index` member of the :cpp:class:`test_harness_args` descriptors must be set accordingly in the SW application.

The replay count of the :cpp:class:`test_harness_args` are set to ``num_repetitions``. This programs the test harness to issue the input data ``num_repetitions`` times and to expect ``num_repetitions`` times the output data. Note that only one repetition count is supported in functional mode.

After the test is configured, the graph is started with the :cpp:func:`test_harness_mgr_client::runAIEGraph` API. 

The test harness is started with the :cpp:func:`test_harness_mgr_client::runTestHarness` API. Starting the harness **AFTER** the graph is necessary to ensure accurate measurement of DMA channel latencies.

Then, use :cpp:func:`test_harness_mgr_client::waitForRes` API to wait for the finish of the data transfer, and get the validity of the result by getting the boolean value of the :cpp:func:`test_harness_mgr_client::result_valid`.
