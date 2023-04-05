.. 
   MIT License

   Copyright (C) 2023 Advanced Micro Devices, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

.. _overview:

.. toctree::
   :hidden:

Build With Test Harness
========================

Environment setup
------------------

To build with test harness, developer needs to install:

* 1. Vitis tools (https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vitis.html)

* 2. Xilinx Runtime (https://www.xilinx.com/products/design-tools/vitis/xrt.html#gettingstarted)

* 3. Versal common image for Vitis embedded platforms (https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-platforms.html)

Before start building, you need to source ``setup.sh`` and export ``SDKTARGETSYSROOT`` according to your installation of Versal common image for Vitis embedded platforms.

Build AI Engine application (libadf.a)
---------------------------------------

To build with test harness, developer needs modify two parts:

* 1. Modify all the PLIO in user graph to ``plio_128_bits`` to match with test harness and use the names specify in ``include/vck190_test_harness_port_name.hpp``. The ``vck190_test_harness::in_names`` is the list of PLIO names that can be used to send data to AI Engine and The ``out_names`` is the list of PLIO names that can be used to receive data from AI Engine. These are the only valid PLIO names to build with test harness.

* 2. In case your design does not use all PLIO ports, you need to put in an instance ``vck190_test_harness::occupyUnusedPLIO`` aside your original design. This instance will help occupy all the rest PLIOs. ``vck190_test_harness::occupyUnusedPLIO`` is a template graph, template arugment ``used_in_plio`` is the number of used PLIO to send data to AI Engine and ``used_out_plio`` is the number of used PLIO to receive data from AI Engine. Also you need to put in all PLIO names into the constructor of ``vck190_test_harness::occupyUnusedPLIO``.

.. code-block::

   template <int used_in_plio, int used_out_plio>
   class occupyUnusedPLIO;

   template <int used_in_plio, int used_out_plio>
   occupyUnusedPLIO::occupyUnusedPLIO(std::vector<std::string> used_in_plio_names,  std::vector<std::string> used_out_plio_names);


Build PS application
---------------------

To build ps application, you need source Xilinx Runtime before compiling. The rest is regular c++ coding.

Package
--------

Test harness provide support packaging for software emulation and hardware. To be notice that software emulation needs AI Engine application to be complied under target ``x86sim``. Please take reference from examples cases for details. You need source Vitis and Xilinx Runtime setup.sh before packaging, and setup proper ``SDKTARGETSYSROOT`` according to build environment.

1. Software emulation: 

Package flow for software emulation will generate ``vck190_test_harness.xclbin`` to run software emulation and put all files needed to user specific directory.

.. code-block:: shell

   test_harness/package_sw_emu.sh <path for sw_emu package files> <your libadf.a> <your host exe> <other files needed for test>

2. Hardware:

Package flow for hardware is slightly different than software emulation. You need download the pre-compiled ``vck190_test_harness.xsa`` from Xilinx or go to ``test_harness`` and build it locally before packaging for hardware. Package flow for hardware will generate SD card image to run on board.

.. code-block:: shell

   test_harness/package_hw.sh <path for hw package files> <your libadf.a> <your host exe> <other files needed for test>
