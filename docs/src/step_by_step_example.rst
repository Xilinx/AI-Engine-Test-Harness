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

Step by Step Example
====================

This repository includes 4 examples from the `Vitis Tutorials <https://github.com/Xilinx/Vitis-Tutorials>`_ and 4 independent examples to demonstrate how to use the test harness to test the AIE graph on a hardware board. This page describes the :url_to_repo:`examples/super-sampling-rate-fir/SingleKernel` as an example.

For additional details on the steps described below, refer to the documentation about :ref:`Using the Test Harness <using_the_harness>` and about the :ref:`Software APIs <sw_apis>`.


1. Use Test Harness PLIOs
-------------------------

This example uses 1 input PLIO and 1 output PLIO. The width of each PLIO is set 128 bits, as required by the test harness. We chose PLIO ``PLIO_01_TO_AIE`` to send data to the AI-Engine and PLIO ``PLIO_02_FROM_AIE`` to receive data from the AI-Engine. As explained in the section about :ref:`Placement of PLIOs <plio_placement>`, the PLIO names indicate the placement and direction of each PLIO. All valid PLIO names can be found in :url_to_repo:`include/vck190_test_harness_port_name.hpp`.

.. code-block:: c++

    TopGraph() {
        input_plio plin = input_plio::create("PLIO_01_TO_AIE", plio_128_bits, "data/PhaseIn_0.txt", 250);
        output_plio plout = output_plio::create("PLIO_02_FROM_AIE", plio_128_bits, "data/Output_0.txt", 250);
        connect<>(plin.out[0], G1.in);
        connect<>(G1.out, plout.in[0]);
    }

2. Connect Unused PLIOs
-----------------------

Because this example does not use all the PLIOs implemented in the test harness, we need to put an additional graph to occupy all of the unused PLIOs. For this purpose, the ``occupyUnusedPLIO`` helper class is instantiated in the :url_to_repo:`examples/super-sampling-rate-fir/SingleKernel/aie/graph.cpp` file. The template parameters indicate number of used input and output PLIOs, and the constructor parameters indicate the names of used input and output PLIOs.

.. code-block:: c++

    static std::vector<std::string> cust_in = {"PLIO_01_TO_AIE"};
    static std::vector<std::string> cust_out = {"PLIO_02_FROM_AIE"};
    TopGraph G;
    vck190_test_harness::occupyUnusedPLIO<1, 1> dummyGraph(cust_in, cust_out);

.. _ps_app:

3. Create SW Application
------------------------

A SW application running on the embedded ARM core of the Versal is necessary to run the test. The source code for this application is provided in the :url_to_repo:`examples/super-sampling-rate-fir/SingleKernel/ps/host.cpp` file. The application must be developed using the :ref:`test harness software APIs <sw_apis>`.

.. code-block:: c++

    test_harness_mgr mgr(0, argv[1], {"G"});
    std::vector<test_harness_args> args;
    args.push_back({channel_index(PLIO_01_TO_AIE), in_sz, 4, 0, (char*)in_data[0]});
    args.push_back({channel_index(PLIO_02_FROM_AIE), out_sz, 4, 0, (char*)out_data[0]});
    mgr.runAIEGraph(0, 4);
    mgr.runTestHarness(args);
    mgr.waitForRes(10000);

The application needs one instance of the :cpp:class:`test_harness_mgr` class. The last argument ``{"G"}`` is a string with the name of the graph instantiated in the graph.cpp file. If an incorrect graph name is provided, the application will report an error at runtime.

Then, a vector or :cpp:struct:`test_harness_args` is created to configure the DMA channels associated with each PLIO used by the AIE graph.
As seen in step #1, the input of the graph is mapped to PLIO ``PLIO_01_TO_AIE``, and the output is mapped to ``PLIO_02_FROM_AIE``. The :cpp:enum:`channel_index` member of the :cpp:struct:`test_harness_args` descriptors must be set accordingly in the SW application.
The replay count of the :cpp:struct:`test_harness_args` are set to 4. This programs the test harness to issue the input data 4 times and to expect 4x the output data.

After the test is configured, the graph is started with the :cpp:func:`test_harness_mgr::runAIEGraph` API. We know that each iteration of graph ``G`` consumes ``in_sz`` bytes of data and generates ``out_sz`` bytes of data. Since we programmed the test harness to replay the inputs and outputs 4 times, the graph is run for 4 iterations to ensure that the data sizes match between the test harness and the graph.

The test harness is started with the :cpp:func:`test_harness_mgr::runAIEGraph` API. Starting the harness after the graph is necessary to ensure accurate measurement of DMA channel latencies.


4. Run Software Emulation
--------------------------

The ``package_sw_emu.sh`` utility is used to generate the files needed to run software emulation. The AIE graph must be compiled for the x86sim target, and the SW application must be compiled with the native g++ compiler.

.. code-block:: shell

    ${TEST_HARNESS_REPO_PATH}/test_harness/package_sw_emu.sh ${PKG_DIR} ${AIE_EXE} ${HOST_EXE}
    LD_LIBRARY_PATH=$(LIBRARY_PATH):$$LD_LIBRARY_PATH XCL_EMULATION_MODE=sw_emu $(PKG_DIR)/host_elf $(PKG_DIR)/vck190_test_harness.xclbin

5. Package SD card image
-------------------------

The ``package_hw.sh`` utility is used to generate the bootable SD card image. The AIE graph must be compiled for the hw target, and the SW application must be compiled with the ARM cross-compiler.

.. code-block:: shell

    ${TEST_HARNESS_REPO_PATH}/test_harness/package_hw.sh ${PKG_DIR} ${AIE_EXE} ${HOST_EXE}
