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

Before start building, you need to source ``setup.sh`` and export ``SDKTARGETSYSROOT`` according to your installation of Versal common image for Vitis embedded platforms.

Instrumenting the AI Engine Graph
=================================

The following modifications are needed to use an AIE graph with the test harness:

Test Harness Header File
------------------------

The harness graph header must be included in the AIE graph sources::

    #include "vck190_test_harness_graph.hpp"

Mapping PLIOs
-------------

The names and width of the usable PLIOs are predefined in the test harness. The original AIE graph must be mapped to these predefined PLIOs. 

The graph must be modified to ensure that all PLIOs are 128 bits wide and use one of the PLIO names predefined in the harness::

    pl_in0 = input_plio::create("Column_12_TO_AIE"   , adf::plio_128_bits, "DataIn0.txt");
    pl_in1 = input_plio::create("Column_13_TO_AIE"   , adf::plio_128_bits, "DataIn1.txt");
    pl_out = output_plio::create("Column_28_FROM_AIE", adf::plio_128_bits, "DataOut0.txt");

The predefined PLIO names are listed in ``include/vck190_test_harness_port_name.hpp``. The ``vck190_test_harness::in_names`` is the list of PLIO names which can be used to send data to AI Engine and ``out_names`` is the list of PLIO names that can be used to receive data from AI Engine. These are the only valid PLIO names to build with test harness. 

Connecting PLIOs
----------------

All the PLIO ports defined in the harness must be connected. In case the AIE graph does not use all the PLIOs defined in the harness, an instance of the ``vck190_test_harness::occupyUnusedPLIO`` helper class must be added in the ``graph.cpp`` file. This class will connect all the PLIOs which are not used by the original AIE graph. 

The ``vck190_test_harness::occupyUnusedPLIO`` class takes two template arguments: ``used_in_plio`` is the number of input PLIOs in the AIE graph and ``used_out_plio`` of output PLIOs in the AIE graph. 

The ``vck190_test_harness::occupyUnusedPLIO`` class constructor takes two arguments: ``used_in_plio_names`` which is a vector of strings containing the names of the input PLIOs used by the AIE graph, and ``used_out_plio_names`` which is a vector of strings containing the names of the output PLIOs used by the AIE graph.

.. code-block::

   template <int used_in_plio, int used_out_plio>
   class occupyUnusedPLIO;

   template <int used_in_plio, int used_out_plio>
   occupyUnusedPLIO::occupyUnusedPLIO(std::vector<std::string> used_in_plio_names,  std::vector<std::string> used_out_plio_names);

NOTE: the length of the ``used_in_plio_names`` vector should match the value of the ``used_in_plio`` template, and the length of the ``used_out_plio_names`` vector should match the value of the ``used_out_plio`` template.


Building the AI Engine Graph
============================

To build the libadf.a for use with the test harness, it must be compiled using the prebuilt XSA as the input platform, and setting the --event-trace and --event-trace-port options as shown below::

    aiecompiler --platform=<path to test harness repo>/bin/vck190_test_harness.xsa --event-trace=runtime --event-trace-port=gmio [other user options]


Building the PS Application
===========================

To build ps application, you need source Xilinx Runtime before compiling. The rest is regular c++ coding.

Packaging the Test
==================

Test harness provide support packaging for software emulation and hardware. To be notice that software emulation needs AI Engine application to be complied under target ``x86sim``. Please take reference from examples cases for details. You need source Vitis and Xilinx Runtime setup.sh before packaging, and setup proper ``SDKTARGETSYSROOT`` according to build environment.

1. Software emulation: 

Package flow for software emulation will generate ``vck190_test_harness.xclbin`` to run software emulation and put all files needed to user specific directory.

.. code-block:: shell

   test_harness/package_sw_emu.sh <path for sw_emu package files> <your libadf.a> <your host exe> <other files needed for test>

2. Hardware:

Package flow for hardware is slightly different than software emulation. You need download the pre-compiled ``vck190_test_harness.xsa`` from Xilinx or go to ``test_harness`` and build it locally before packaging for hardware. Package flow for hardware will generate SD card image to run on board.

.. code-block:: shell

   test_harness/package_hw.sh <path for hw package files> <your libadf.a> <your host exe> <other files needed for test>
