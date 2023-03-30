.. 
   Copyright 2019 Xilinx, Inc.
  
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
  
       http://www.apache.org/licenses/LICENSE-2.0
  
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

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

Before start building, you need to source ``setup.sh`` and export ``EDGE_COMMON_SW`` according to your installation of Versal common image for Vitis embedded platforms.

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

Test harness provide support packaging for software emulation and hardware. To be notice that software emulation needs AI Engine application to be complied under target ``x86sim``. Please take reference from examples cases for details. You need source Vitis and Xilinx Runtime setup.sh before packaging, and setup proper ``SYSROOT`` and ``EDGE_COMMON_SW`` according to build environment.

1. Software emulation: 

Package flow for software emulation will generate ``vck190_test_harness.xclbin`` to run software emulation and put all files needed to user specific directory.

.. code-block:: shell

   test_harness/package_sw_emu.sh <path for sw_emu package files> <your libadf.a> <your host exe> <other files needed for test>

2. Hardware:

Package flow for hardware is slightly different than software emulation. You need download the pre-compiled ``vck190_test_harness.xsa`` from Xilinx or go to ``test_harness`` and build it locally before packaging for hardware. Package flow for hardware will generate SD card image to run on board.

.. code-block:: shell

   test_harness/package_hw.sh <path for hw package files> <your libadf.a> <your host exe> <other files needed for test>
