.. 
   MIT License

   Copyright (C) 2023 Advanced Micro Devices, Inc.

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

Environment Setup
=================

Before building the design with the AIE test harness, you need to source the ``setup.sh`` script included in this repository. You should also ensure that Vitis, XRT, Versal ``SDKTARGETSYSROOT``, and ``LD_LIBRARY_PATH`` are properly set.

.. code-block:: shell

    source <path to Versal common image>/environment-setup-cortexa72-cortexa53-xilinx-linux
    source <path to Vitis installation>/settings64.sh
    source <path to XRT installation>/setup.sh

    source <test harness repo root folder>/setup.sh
    export LD_LIBRARY_PATH=<your gcc installation>/lib64:$LD_LIBRARY_PATH

Instrumenting the AI Engine Graph
=================================

The following modifications are needed to connect an AIE graph to the test harness:

Test Harness Header File
------------------------

The test harness graph header must be included in the AIE graph sources:

.. code-block:: c++

    #include "test_harness_graph.hpp"

Mapping PLIOs
-------------

The names and width of the available PLIOs are predefined in the test harness. The original AIE graph must be mapped to these predefined PLIOs to make sure the PL data mover can be correctly connected to the user AIE graph.

The graph must be modified to ensure that all PLIOs are 128 bits wide and use one of the PLIOs predefined in the test harness.

The predefined PLIO names are listed in :url_to_repo:`include/test_harness_port_name.hpp`. The ``test_harness::in_names`` is the list of PLIO names which can be used to send data to AI Engine and ``test_harness::out_names`` is the list of PLIO names that can be used to receive data from AI Engine. These are the only valid PLIO names to be built with test harness. 

**Example**

.. code-block:: c++

    pl_in0 = input_plio::create("PLIO_01_TO_AIE"   , adf::plio_128_bits, "DataIn0.txt");
    pl_in1 = input_plio::create("PLIO_02_TO_AIE"   , adf::plio_128_bits, "DataIn1.txt");
    pl_out = output_plio::create("PLIO_01_FROM_AIE", adf::plio_128_bits, "DataOut0.txt");

Occupying unused PLIOs
-----------------------

All the PLIO ports defined in the test harness must be connected. In case the AIE graph does not need all the PLIOs defined in the test harness, an instance of the ``test_harness::occupyUnusedPLIO`` helper class must be added to the ``graph.cpp`` file. This class will help to occupy all the PLIOs which are not used by the user AIE graph. 

.. code-block:: c++

   template <int used_in_plio, int used_out_plio, int max_num_plio>
   class occupyUnusedPLIO;

   template <int used_in_plio, int used_out_plio, int max_num_plio>
   occupyUnusedPLIO::occupyUnusedPLIO(std::vector<std::string> used_in_plio_names,  std::vector<std::string> used_out_plio_names);


**Templates**

``used_in_plio``
  The number of input PLIOs used in the AIE graph 

``used_out_plio`` 
  The number of output PLIOs used in the AIE graph

``max_num_plio``
  Maximum number of PLIOs pre-defined in XSA (36 for VCK190, 16 for VEK280)


**Parameters**

``used_in_plio_names`` 
  Vector of strings containing the names of the input PLIOs used by the AIE graph. The length of the vector should match the value of the ``used_in_plio`` template

``used_out_plio_names`` 
  Vector of strings containing the names of the output PLIOs used by the AIE graph. The length of the vector should match the value of the ``used_out_plio`` template.


**Example**

.. code-block:: c++

    #include "test_harness_graph.hpp"

    static std::vector<std::string> cust_in = {"PLIO_01_TO_AIE", "PLIO_02_TO_AIE"};
    static std::vector<std::string> cust_out = {"PLIO_01_FROM_AIE"};
    test_harness::occupyUnusedPLIO<2, 1, 36> unusedPLIOs(cust_in, cust_out);


Creating the SW Application
===========================

A SW application running on the embedded ARM core (PS) of the Versal is necessary to run the test. This SW application must be developped using the :ref:`test harness software APIs <sw_apis>`.

The application usually ressembles the structure and contents of ``graph.cpp`` file used in x86sim and AIEsim. The main difference is that a different set of APIs is used to transfer data and interact with the AIE graph.

For additional details, refer to the :ref:`step by step example <ps_app>` section in this documentation, or the example provided in this repo, such as :url_to_repo:`examples/vck190/adder/ps/host.cpp`.

Testing on Hardware
===================

Once the AIE graph has been modified and the SW application has been created, the test can be built and run on the hardware board.

Building the test application is done in three simple steps:

1. Build the AIE graph
2. Build the SW application
3. Package the libadf.a, host_elf and other files to create a bootable SD card image


Building the AI Engine Graph
----------------------------

To build the libadf.a for use with the test harness, it must be compiled using the prebuilt XSA as the input platform, with the ``hw`` target, and setting the ``--event-trace`` and ``--event-trace-port`` options as shown below:

.. code-block:: shell

    v++ -c --mode aie --platform=${TEST_HARNESS_REPO_PATH}/bin/<vck190_test_harness_func.xsa/vck190_test_harness_perf.xsa/vek280_test_harness.xsa>
                      --target=hw 
                      --aie.event-trace runtime 
                      --aie.event-trace-port gmio 
                      --I ${TEST_HARNESS_REPO_PATH}/include/
                      [other user options]


Building the SW Application
---------------------------

The SW application must be compiled with the ARM cross-compiler and using the Xilinx Runtime (XRT) libraries.

.. code-block:: shell

    source <path to Versal common image>/environment-setup-cortexa72-cortexa53-xilinx-linux

    ${CXX} test.cpp -c -I${XILINX_XRT}/include -I${TEST_HARNESS_REPO_PATH}/include -o test.o
    ${CXX} test.o -lxrt_coreutil -L${XILINX_XRT}/lib -o host_elf


Packaging the Test
------------------

The AIE test harness includes a utility script which can be used to package the test files and generate a bootable SD card image to run the test on the hardware board.

.. code-block:: shell

   test_harness/package_<vck190/vek280>_hw.sh <output dir>
                                              <pre-built XSAs>
                                              <host_elf> 
                                              <other files needed by the test>


**Parameters**

``<output dir>``
  The folder in which the output of the packaging script and the bootable SD card image should be generated.

``<pre-built XSA(s)>``
  The pre-compiled XSA. vck190_test_harness_func.xsa & vck190_test_harness_perf.xsa for VCK190, or vek280_test_harness.xsa for VEK280.

``<libadf.a>``
  The libadf.a resulting from the compilation of the AIE graph.

``<host_elf>``
  The executable resulting from building the SW application.

``<other files needed by the test>``
  A list of other files needed by the test and to be packaged in the SD card image. This can be for instance input data files read by the test application.


Software Emulation (VCK190 only)
================================

The AIE test harness also provides support for packaging and running the test in the software emulation mode for VCK190.

In the software emulation mode, the AIE graph is compiled for x86sim and the SW application is compiled using the native compiler of the host system. 

This mode allows testing the SW application and making sure that it works correctly before running the test on hardware.

.. rubric:: Building the AI Engine Graph

Building the AIE graph for software emulation is similar to building it for HW, except that the ``x86sim`` target should be used:

.. code-block:: shell

    v++ -c --mode aie --platform=${TEST_HARNESS_REPO_PATH}/bin/<vck190_test_harness_func.xsa/vck190_test_harness_perf.xsa>
                      --target=x86sim 
                      --aie.event-trace runtime 
                      --aie.event-trace-port gmio 
                      --I ${TEST_HARNESS_REPO_PATH}/include/                
                      [other user options]


.. rubric:: Building the SW Application

Building the SW application for software emulation is similar to building it for HW, except that the native g++ compiler should be used:

.. code-block:: shell

    g++ test.cpp -c -I${XILINX_XRT}/include -I${TEST_HARNESS_REPO_PATH}/include -o test.o
    g++ test.o -lxrt_coreutil -L${XILINX_XRT}/lib -o host_elf


.. rubric:: Packaging the Test

Packaging the test for software emulation is similar to packaging it for HW, except that the package_sw_emu.sh script should be used:

.. code-block:: shell

   test_harness/package_sw_emu.sh <output dir> 
                                  <pre-built XSAs>
                                  <host_elf> 
                                  <other files needed by the test>


.. rubric:: Running SW emulation

To run the test in software emulation mode, first set the ``XCL_EMULATION_MODE`` variable to ``sw_emu`` before running the natively compiled test application:

.. code-block:: shell

   cd <sw emu output dir>
   export XCL_EMULATION_MODE=sw_emu 
   ./host_elf <optional args>


Troubleshooting
===============

AIE Compilation
---------------

**Issue:** The following error message is seen when compiling the AIE graph with the test harness XSA: ``ERROR: [aiecompiler 77-4252] For application port with annotation 'PLIO_01_TO_AIE' the buswidth is 32-bits, which is different than the buswidth of 128-bits as specified in incoming logical architecture``

- The width of the PLIOs in the prebuilt XSA is set to 128 bits. The PLIO widths in the AIE graph must align with the XSA. Set all PLIO width in the graph to ``adf::plio_128_bits``.


**Issue:** The following error message is seen when compiling the AIE graph with the test harness XSA: ``ERROR: [aiecompiler 77-295] Cannot find port instance tf0_pi0 corresponding to Logical Arch Port M00_AXI``

- The ``--aie.event-trace runtime --aie.event-trace-port gmio`` options are missing for the ``v++ -c --mode aie`` command


AIE Simulation
--------------

**Issue:** The following error message is seen when running x86sim or AIEsim after modifying the graph to work with the test harness: ``Error: Could not open input file : ./data/dummy.txt``

- The unused PLIOs are expecting an input data file called ``./data/dummy.txt``. Create an empty file with this name and in this folder, then rerun x86sim or AIEsim.


HW Testing
----------

**Issue:** When running on HW, the performance numbers reported by the test harness change a lot from run to run.

- Making sure to start the AIE graph before starting the DMA engine. The performance counters start at the same time as the DMA engine. If the graph is not already started and ready to send / receive data, the performance counters will be incremented by an arbitrary number of cycles.

