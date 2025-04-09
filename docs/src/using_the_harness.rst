.. 
   MIT License

   Copyright (C) 2023-2025 Advanced Micro Devices, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.

.. _using_the_harness:

.. highlight:: none

.. toctree::
   :hidden:

**********************
Using the Test Harness
**********************

Delopying Pre-built SD Card on the Board
========================================

The test harness server has been built already for deploying on the VCK190 or VEK280 board. You can download the pre-built image by :

.. code-block:: shell

    cd bin
    source download.sh
    tar xzvf vck190_sd_card.img.zip
    <or>
    tar xzvf vek280_sd_card.img.zip

Burn and boot with the ``sd_card.img``. In the board, get the IP address and launch the server by:

.. code-block:: shell

    ifconfig
    cd /run/media/mmcblk0p1
    ./run_server.sh

Note that by default, if there's no data transaction between the server and client for 60 seconds, the server will kill the current connection and wait for new connections. If longer waiting time for the transaction is needed, you can edit ``run_server.sh`` and then launch it.

Environment Setup
=================

Before building the design with the AIE test harness, you need to source the ``setup.sh`` script included in this repository. 
You should also ensure that Vitis, XRT, Versal ``SDKTARGETSYSROOT``, and ``LD_LIBRARY_PATH`` are properly set.

.. code-block:: shell

    source <path to Versal common image>/environment-setup-cortexa72-cortexa53-xilinx-linux
    source <path to Vitis installation>/settings64.sh
    source <path to XRT installation>/setup.sh

    source <test harness repo root folder>/setup.sh
    export LD_LIBRARY_PATH=<your gcc installation>/lib64:$LD_LIBRARY_PATH
    Set up your ROOTFS and IMAGE to point to the rootfs.ext4 and Image files located in the /Common Images Dir/xilinx-versal-common-v2024.2 directory
    Set up your PLATFORM_REPO_PATHS environment variable to $XILINX_VITIS/base_platforms
	
Instrumenting the AI Engine Graph
=================================

The following modifications are required to connect the user AIE graph to the test harness:

Test Harness Header File
------------------------

The test harness graph header must be included in the AIE graph sources:

.. code-block:: c++

    #include "test_harness_graph.hpp"

Mapping PLIOs
-------------

The names and width of the available PLIOs are predefined in the test harness. 
The original AIE graph must be mapped to these predefined PLIOs to make sure the PL data mover can be correctly connected to the user AIE graph.

The user AIE graph must be modified to ensure that all PLIOs are 128 bits wide and use the PLIOs predefined in the test harness.

The predefined PLIO names are listed in :url_to_repo:`include/aie/test_harness_port_name.hpp`. 
The ``test_harness::in_names`` is the list of PLIO names which can be used to send data to AI Engine 
and ``test_harness::out_names`` is the list of PLIO names that can be used to receive data from AI Engine. 
These are the **ONLY** valid PLIOs to be built with the test harness. 

**Example**

.. code-block:: c++

    pl_in0 = input_plio::create("PLIO_01_TO_AIE"   , adf::plio_128_bits, "DataIn0.txt");
    pl_in1 = input_plio::create("PLIO_02_TO_AIE"   , adf::plio_128_bits, "DataIn1.txt");
    pl_out = output_plio::create("PLIO_01_FROM_AIE", adf::plio_128_bits, "DataOut0.txt");

Creating the SW Application
===========================

A SW application running on the client server (x86) or embedded ARM core (PS) of the Versal is necessary to run the test. 
This SW application must be developped using the :ref:`test harness software APIs <sw_apis>`.

For additional details, refer to the :ref:`step by step example <ps_app>` section in this documentation, 
or the example provided in this repo, such as :url_to_repo:`examples/vck190/adder_perf/ps/host.cpp`.

Testing on Hardware
===================

Once the AIE graph has been modified and the SW application has been created, the test can be built and run with connection to the server on the board.

Building the test application is done in three simple steps:

1. Building the AIE graph
2. Building the SW application
3. Packaging the libadf.a, host_elf and other files to create a run directory.


Building the AI Engine Graph
----------------------------

To build the libadf.a for use with the test harness, 
it must be compiled using the desired prebuilt XSA as the input platform, with the ``hw`` target.

The prebuilt XSAs are ``vck190_test_harness.xsa`` for VCK190 and ``vek280_test_harness.xsa`` for VEK280.

.. code-block:: shell

    v++ -c --mode aie --platform=${TEST_HARNESS_REPO_PATH}/bin/<vck190_test_harness.xsa/vek280_test_harness.xsa>
                      --target=hw 
                      --aie.event-trace runtime 
                      --aie.event-trace-port gmio 
                      --I ${TEST_HARNESS_REPO_PATH}/include/aie
                      [other options]


Building the SW Application
---------------------------

The SW application by default is compiled for x86 server. Here, ``CXX`` points to ``g++``:

.. code-block:: shell

    ${CXX} -o $(TEMP_DIR)/host_elf ps/host.cpp ${HOST_CONFIG_FLAGS} -I${TEST_HARNESS_REPO_PATH}/include/ps

It can also be compiled with the ARM cross-compiler and using the similar command as above. Here, ``CXX`` points to the arm cross compiler ``aarch64-linux-gnu-g++``.


Packaging the Test
------------------

The AIE test harness includes utility scripts which can be used to package the test files and generate a bootable SD card image to run the test on the hardware board on either VCK190 or VEK280.


**VCK190**

.. code-block:: shell

   v++ -p -t ${TARGET} -f ${TEST_HARNESS_PLATFORM} -o ${XCLBIN} $(TEST_HARNESS_XSA) ${AIE_ADF} \
        --package.defer_aie_run \
        --advanced.param package.enableAiePartitionDrc=0 \
        --package.out_dir ${TEMP_DIR}

**VEK280**

.. code-block:: shell

   v++ -p -t ${TARGET} -f ${TEST_HARNESS_PLATFORM} -o ${XCLBIN} $(TEST_HARNESS_XSA) ${AIE_ADF} \
        --config ${TEST_HARNESS_REPO_PATH}/cfg/package_aie_only.cfg \
        --package.defer_aie_run \
        --advanced.param package.enableAiePartitionDrc=0 \
        --package.out_dir ${TEMP_DIR}


Launching Client Application and Getting Performance Result
===========================================================

After completing the client packaging, it's ready to connect to the server on the board and get the functional and performance results. First it needs to set the server IP port and then launch the application. Then the execution result will be transferred back to the client server and the throughputs of the ports are reported. The commands to be used:

.. code-block:: shell

   export SERVER_IP_PORT=<IP Address of the board>:8080
   ./host_elf <XCLBIN Name> <Other Options>

Building the Matlab APIs
========================

The Matlab APIs can be built as needed by:

.. code-block:: shell

   cd test_harness
   make matlab

Building the Python APIs
========================

The python APIs can built as needed by:

.. code-block:: shell

   cd test_harness
   pip install -r requirements.txt
   make python

Debuggability
===============

Event Trace
---------------

Event trace can help analyze the performance bottleneck in HW. It can be enabled by adding the ``xrt.ini`` file in the client server, and then launch the application. The event trace results will be transferred back to the client after it's running on the board. An example ``xrt.ini`` file:

.. code-block:: c++

   [Debug]
   aie_trace=true
   [AIE_trace_settings]
   buffer_size=100M
   graph_based_aie_tile_metrics=all:all:all_stalls

AIE Status Report
------------------

When design hangs, the AIE status report can be used to get an visual overview of the AIE status in Vitis Analysis View. This helps analyze where is the cause of the hang. To enable the status report, it needs to set a timeout value in host code (for example: ``ps/host.cpp``) to ensure the application exits normally:

.. code-block:: c++

    mgr.waitForRes(1000); //Here, set the timeout to 1000us
   
And then add ``xrt.ini`` in the application directory before lauching. The example ``xrt.ini``:

.. code-block:: c++

   [Debug]
   aie_status=true
   aie_status_interval_us=1000 
	
Launch the application. After it finishes, open the summary in Vitis:

.. code-block:: shell

   vitis -a xrt.run_summary
   

Troubleshooting
===============

AIE Compilation
---------------

**Issue:** The following error message is seen when compiling the AIE graph with the test harness XSA: ``ERROR: [aiecompiler 77-4252] For application port with annotation 'PLIO_01_TO_AIE' the buswidth is 32-bits, which is different than the buswidth of 128-bits as specified in incoming logical architecture``

- The width of the PLIOs in the prebuilt XSA is set to 128 bits. The PLIO widths in the AIE graph must match with the XSA. Set all PLIO width in the graph to ``adf::plio_128_bits``.

HW Testing
----------

**Issue:** When running on HW, the performance numbers reported by the test harness vary a lot from run to run.

- Making sure to start the AIE graph before starting the PL DMA engine. The performance counters start at the same time as the PL DMA engine starts. If the graph is not already started and ready to transfer data, the performance counters will be incremented by an arbitrary number of cycles before the application actually starts.

