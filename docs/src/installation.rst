.. 
   MIT License

   Copyright (C) 2023 Advanced Micro Devices, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.

.. _installation:

.. highlight:: none

.. toctree::
   :hidden:

Installation
============

Prerequisites
-------------

To use the test harness, up-to-date installations of the following softwares and libraries are required:

- Vitis tools (https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vitis.html)

- Xilinx Runtime (https://www.xilinx.com/products/design-tools/vitis/xrt.html#gettingstarted)

- Versal common image for Vitis embedded platforms (https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-platforms.html)


Installation Instructions
-------------------------

1. Clone test harness repository::

    git clone https://github.com/Xilinx/AI-Engine-Test-Harness.git

2. Download the prebuilt test harness XSAs::

    cd AI-Engine-Test-Harness/bin
    source download.sh

.. CAUTION::
   The prebuilt XSAs can only be used with the 2023.2 version of the Vitis tool. 
   To build and test your AI Engine graph using other versions of Vitis, you must first rebuild the XSA with the corresponding version of the Vitis tool, as described in the section below.

Rebuilding from Source
----------------------

The prebuilt test harness XSA can optionally be rebuilt from source as follows::

    cd AI-Engine-Test-Harness/test_harness
    source <path to Vitis installation>/settings64.sh
    make clean
    # To build functional/performance testing mode XSA for VCK190
    make <vck190_func_xsa/vck190_perf_xsa> TARGET=hw DEVICE=vck190
    # Or to build performance testing mode only XSA for VEK280
    make vek280_xsa TARGET=hw DEVICE=vek280
