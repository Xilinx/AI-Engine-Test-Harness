.. 
   MIT License

   Copyright (C) 2023 Advanced Micro Devices, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.

.. _using_the_harness:

.. toctree::
   :hidden:

**********************
Using the Test Harness
**********************

Environment Setup
=================

Before building the design with the AIE test harness, you need to source the ``setup.sh`` script included in this repository. You should also ensure that Vitis, XRT and Versal ``SDKTARGETSYSROOT`` are properly set up.

.. code-block:: shell

    source <path to Versal common image>/environment-setup-cortexa72-cortexa53-xilinx-linux
    source <path to Vitis installation>/settings64.sh
    source <path to XRT installation>/setup.sh

    source <test harness repo folder>/setup.sh

Instrumenting the AI Engine Graph
=================================

The following modifications are needed to use an AIE graph with the test harness:

Test Harness Header File
------------------------

The harness graph header must be included in the AIE graph sources:

.. code-block:: c++

    #include "vck190_test_harness_graph.hpp"

Mapping PLIOs
-------------

The names and width of the usable PLIOs are predefined in the test harness. The original AIE graph must be mapped to these predefined PLIOs. 

The graph must be modified to ensure that all PLIOs are 128 bits wide and use one of the PLIO names predefined in the harness.

The predefined PLIO names are listed in :url_to_repo:`include/vck190_test_harness_port_name.hpp`. The ``vck190_test_harness::in_names`` is the list of PLIO names which can be used to send data to AI Engine and ``out_names`` is the list of PLIO names that can be used to receive data from AI Engine. These are the only valid PLIO names to build with test harness. 

**Example**

.. code-block:: c++

    pl_in0 = input_plio::create("Column_12_TO_AIE"   , adf::plio_128_bits, "DataIn0.txt");
    pl_in1 = input_plio::create("Column_13_TO_AIE"   , adf::plio_128_bits, "DataIn1.txt");
    pl_out = output_plio::create("Column_28_FROM_AIE", adf::plio_128_bits, "DataOut0.txt");

Connecting unused PLIOs
-----------------------

All the PLIO ports defined in the harness must be connected. In case the AIE graph does not use all the PLIOs defined in the harness, an instance of the ``vck190_test_harness::occupyUnusedPLIO`` helper class must be added in the ``graph.cpp`` file. This class will connect all the PLIOs which are not used by the original AIE graph. 

.. code-block:: c++

   template <int used_in_plio, int used_out_plio>
   class occupyUnusedPLIO;

   template <int used_in_plio, int used_out_plio>
   occupyUnusedPLIO::occupyUnusedPLIO(std::vector<std::string> used_in_plio_names,  std::vector<std::string> used_out_plio_names);


**Templates**

``used_in_plio``
  The number of input PLIOs in the AIE graph 

``used_out_plio`` 
  The number of output PLIOs in the AIE graph


**Parameters**

``used_in_plio_names`` 
  Vector of strings containing the names of the input PLIOs used by the AIE graph. The length of the vector should match the value of the ``used_in_plio`` template

``used_out_plio_names`` 
  Vector of strings containing the names of the output PLIOs used by the AIE graph. The length of the vector should match the value of the ``used_out_plio`` template.


**Example**

.. code-block:: c++

    #include "vck190_test_harness_graph.hpp"

    static std::vector<std::string> cust_in = {"Column_12_TO_AIE", "Column_13_TO_AIE"};
    static std::vector<std::string> cust_out = {"Column_28_FROM_AIE"};
    vck190_test_harness::occupyUnusedPLIO<2, 1> unusedPLIOs(cust_in, cust_out);


Creating the SW Application
===========================

A SW application running on the embedded ARM core of the Versal is necessary to run the test. This SW application must be developped using the :ref:`test harness software APIs <sw_apis>`.

The application usually ressembles the structure and contents of graph.cpp file used in x86sim and AIEsim. The main difference is that a difference set of APIs is used to transfer data and interact with the AIE graph.

For additional details, refer to the :ref:`step by step example <ps_app>` section of this documentation, or to one of the examples included in this repo, such as :url_to_repo:`examples/super-sampling-rate-fir/SingleKernel/ps/host.cpp`.

Testing on Hardware
===================

Once the AIE graph has been modified and the SW application has been created, the test can be built and run on the hardware board.

Building the test is done in three simple steps:

1. Build the AIE graph
2. Build the SW application
3. Package the libadf.a, test.exe and other files to create a bootable SD card image


Building the AI Engine Graph
----------------------------

To build the libadf.a for use with the test harness, it must be compiled using the prebuilt XSA as the input platform, with the ``hw`` target, and setting the --event-trace and --event-trace-port options as shown below:

.. code-block:: shell

    aiecompiler --platform=${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness.xsa
                --target=hw 
                --event-trace=runtime 
                --event-trace-port=gmio 
                -include=${TEST_HARNESS_REPO_PATH}/include/
                [other user options]


Building the SW Application
---------------------------

The SW application must be compiled with the ARM cross-compiler and using the Xilinx Runtime (XRT) libraries.

.. code-block:: shell

    source <path to Versal common image>/environment-setup-cortexa72-cortexa53-xilinx-linux

    ${CXX} test.cpp -c -I${XILINX_XRT}/include -I${TEST_HARNESS_REPO_PATH}/include -o test.o
    ${CXX} test.o -lxrt_coreutil -L${XILINX_XRT}/lib -o test.exe


Packaging the Test
------------------

The AIE test harness includes a utility script which can be used to package the test files and generate a bootable SD card image to run the test on the hardware board.

.. code-block:: shell

   test_harness/package_hw.sh <output dir> 
                              <libadf.a>
                              <text.exe> 
                              <other files needed by the test>


**Parameters**

``<output dir>``
  The folder in which the output of the packaging script and the bootable SD card image should be generated

``<libadf.a>``
  The libadf.a resulting from the compilation of the AIE graph

``<test.exe>``
  The executable resulting from building the SW application

``<other files needed by the test>``
  A list of other files needed by the test and to be packaged in the SD card image. This can be for instance input data files read by the test application


Software Emulation
==================

The AIE test harness also provides support for packaging and running the test in the software emulation mode. 

In the software emulation mode, the AIE graph is compiled for x86sim and the SW application is compiled using the native compiler of the host system. 

This mode allows testing the SW application and making sure that it works correctly before running the test on hardware.

.. rubric:: Building the AI Engine Graph

Building the AIE graph for software emulation is similar to building fit or HW, except that the ``x86sim`` target should be used:

.. code-block:: shell

    aiecompiler --platform=${TEST_HARNESS_REPO_PATH}/bin/vck190_test_harness.xsa
                --target=x86sim 
                --event-trace=runtime 
                --event-trace-port=gmio 
                -include=${TEST_HARNESS_REPO_PATH}/include/                
                [other user options]


.. rubric:: Building the SW Application

Building the SW application for software emulation is similar to building it for HW, except that the native g++ compiler should be used:

.. code-block:: shell

    g++ test.cpp -c -I${XILINX_XRT}/include -I${TEST_HARNESS_REPO_PATH}/include -o test.o
    g++ test.o -lxrt_coreutil -L${XILINX_XRT}/lib -o test.exe


.. rubric:: Packaging the Test

Packaging the test for software emulation is similar to packaging it for HW, except that the package_sw_emu.sh script should be used:

.. code-block:: shell

   test_harness/package_sw_emu.sh <output dir> 
                                  <libadf.a>
                                  <text.exe> 
                                  <other files needed by the test>


.. rubric:: Running SW emulation

To run the test in software emulation mode, first set the ``XCL_EMULATION_MODE`` variable before running the natively compiled test application:

.. code-block:: shell

   cd <sw emu output dir>
   export XCL_EMULATION_MODE=sw_emu 
   ./test.exe <optional args>

