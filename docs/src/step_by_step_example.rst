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

This repository includes `5 examples <https://github.com/Xilinx/AI-Engine-Test-Harness/tree/main/examples/vck190>`_ from the `Vitis Tutorials <https://github.com/Xilinx/Vitis-Tutorials>`_ and 4 individual user examples to demonstrate how to use the test harness on VCK190 and `1 example <https::/github.com/Xilinx/AI-Engine-Test-Harness/tree/main/examples/vek280>` from the `Vitis Accelerated Libraries <https://github.com/Xilinx/Vitis_Libraries>` to use the test harness on VEK280 to test the AIE graph on a hardware board. 

All these examples include a standard Makefile which supports the following actions:

- Build the example for SW emulation (VCK190 only) and HW, respectively::

    make package TARGET=sw_emu
    make package TARGET=hw

- Simulate the example in x86sim, AIEsim and SW emulation (VCK190 only), respectively::

    make run TARGET=x86sim
    make run TARGET=aiesim
    make run TARGET=sw_emu

- Clean all files::

    make cleanall


Running an Example
==================

This section describes how to run the `examples/vck190/super-sampling-rate-fir/SingleKernel <https://github.com/Xilinx/AI-Engine-Test-Harness/tree/main/examples/vck190/super-sampling-rate-fir/SingleKernel>`_ example. The same steps apply to all other examples included in the repository.

#. If not already done, install the AIE Test Harness::

    git clone https://github.com/Xilinx/AI-Engine-Test-Harness.git

#. Set up your environment to use the 2023.1 versions Vitis, XRT and the prebuilt Embedded SW Image for Versal. The :envvar:`XILINX_VITIS`, :envvar:`XILINX_XRT` and :envvar:`SDKTARGETSYSROOT` environment variables must be properly defined.

#. Set up your environment to use the test harness, and navigate to the desired example folder::

    cd AI-Engine-Test-Harness
    source setup.sh
    cd examples/vck190/super-sampling-rate-fir/SingleKernel

#. Run the example in SW emulation mode::

    make cleanall run TARGET=sw_emu

#. Package the example to run on the VCK190 board::

    make cleanall package TARGET=hw

#. Flash the :file:`sd_card.img` file located in the :file:`package_hw` folder on a SD card. On Windows, use the `Balena Etcher <https://etcher.balena.io/#download-etcher>`_ tool to flash the card. On Linux, use the ``dd`` command.

#. Using a client such as `PuTTY <https://www.putty.org/>`_, connect to the VCK190 board using the correct USB Serial Port. Use speed 115200.

#. Insert the SD card in card reader on the VCK190 board, and turn on the power switch. NOTE: Make sure that the SW1 DIP switch on the VCK190 board is set to [1110] to boot from the SD card.

#. After the boot sequence is completed, login using username: petalinux and password: petalinux

#. Run the application as shown below, using petalinux as the root password. The test application transfers data to the AIE graph and reports the number of clock cycles needed to send or receive data for each PLIO. Upon successfull completion of the test, the run script will finish with a “TEST PASSED” message::

    sudo su
    cd /run/media/mmcblk0p1/
    source ./run_script.sh


Understanding the Example
=========================

This section describes the source code changes made to run the :url_to_repo:`examples/vck190/super-sampling-rate-fir/SingleKernel` example with the AIE Test Harness.

For additional details on the steps described below, refer to the documentation about :ref:`Using the Test Harness <using_the_harness>` and about the :ref:`Software APIs <sw_apis>`.


Test Harness PLIOs
------------------

This example uses 1 input PLIO and 1 output PLIO. The width of each PLIO is set 128 bits, as required by the test harness. We chose PLIO ``PLIO_01_TO_AIE`` to send data to the AI-Engine and PLIO ``PLIO_02_FROM_AIE`` to receive data from the AI-Engine. As explained in the section about :ref:`Placement of PLIOs <plio_placement>`, the PLIO names indicate the generic name and direction of each PLIO. All valid PLIO names can be found in :url_to_repo:`include/test_harness_port_name.hpp`.

.. code-block:: c++

    TopGraph() {
        input_plio plin = input_plio::create("PLIO_01_TO_AIE", plio_128_bits, "data/PhaseIn_0.txt", 250);
        output_plio plout = output_plio::create("PLIO_02_FROM_AIE", plio_128_bits, "data/Output_0.txt", 250);
        connect<>(plin.out[0], G1.in);
        connect<>(G1.out, plout.in[0]);
    }

Unused PLIOs
------------

Because this example does not use all the PLIOs implemented in the test harness, we need to put an additional graph to occupy all unused PLIOs. For this purpose, the ``occupyUnusedPLIO`` helper class is instantiated in the :url_to_repo:`examples/vck190/super-sampling-rate-fir/SingleKernel/aie/graph.cpp` file. The template parameters indicate number of used input and output PLIOs, and the constructor parameters indicate the names of used input and output PLIOs.

.. code-block:: c++

    static std::vector<std::string> cust_in = {"PLIO_01_TO_AIE"};
    static std::vector<std::string> cust_out = {"PLIO_02_FROM_AIE"};
    TopGraph G;
    vck190_test_harness::occupyUnusedPLIO<1, 1, 36> dummyGraph(cust_in, cust_out);

.. _ps_app:

SW Application
--------------

A SW application running on the embedded ARM core of the Versal is necessary to run the test. The source code for this application is provided in the :url_to_repo:`examples/vck190/super-sampling-rate-fir/SingleKernel/ps/host.cpp` file. The application must be developed using the :ref:`test harness software APIs <sw_apis>`.

.. code-block:: c++

    test_harness_mgr<36, 16, 4096> mgr(0, argv[1], {"vck190_test_harness_perf"}, {"G"}, REP_MODE, "vck190");
    std::vector<test_harness_args> args;
    args.push_back({channel_index(PLIO_01_TO_AIE), in_sz, 4, 0, 0, 0, (char*)in_data[0]});
    args.push_back({channel_index(PLIO_02_FROM_AIE), out_sz, 4, 0, 0, 0, (char*)out_data[0]});
    mgr.runAIEGraph(0, 4);
    mgr.runTestHarness(args);
    mgr.waitForRes(10000);

The application needs one instance of the :cpp:class:`test_harness_mgr` class. The argument ``{"vck190_test_harness_perf"}`` is the PL kernel pre-compiled in the XSA, the argument ``{"G"}`` is a string with the name of the graph instantiated in the ``graph.cpp`` file, ``REP_MODE`` is the testing mode that the user is requiring, and ``vck190`` is the device that the applicatoin targeting. If any incorrect argument is provided, the application will report an error during runtime.

Then, a vector or :cpp:struct:`test_harness_args` is created to configure the DMA channels associated with each PLIO used by the AIE graph.
As seen in step #1, the input of the graph is mapped to PLIO ``PLIO_01_TO_AIE``, and the output is mapped to ``PLIO_02_FROM_AIE``. The :cpp:enum:`channel_index` member of the :cpp:struct:`test_harness_args` descriptors must be set accordingly in the SW application.
The replay count of the :cpp:struct:`test_harness_args` are set to 4. This programs the test harness to issue the input data 4 times and to expect 4x the output data.

After the test is configured, the graph is started with the :cpp:func:`test_harness_mgr::runAIEGraph` API. We know that each iteration of graph ``G`` consumes ``in_sz`` bytes of data and generates ``out_sz`` bytes of data. Since we programmed the test harness to replay the inputs and outputs 4 times, the graph is run for 4 iterations to ensure that the data sizes match between the test harness and the graph.

The test harness is started with the :cpp:func:`test_harness_mgr::runAIEGraph` API. Starting the harness after the graph is necessary to ensure accurate measurement of DMA channel latencies.
