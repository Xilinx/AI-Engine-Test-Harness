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

To use the test harness, up-to-date installations of the following softwares are needed:

- Vitis tools (https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vitis.html)

- Xilinx Runtime (https://www.xilinx.com/products/design-tools/vitis/xrt.html#gettingstarted)

- Versal common image for Vitis embedded platforms (https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-platforms.html)


Installation Instructions
-------------------------

1. Clone this repository::

    git clone https://github.com/Xilinx/AI-Engine-Test-Harness.git

2. Download the prebuilt test harness XSA::

    cd Test_Harness/bin
    source download.sh

.. CAUTION::
   The prebuilt test harness XSA can only work 2023.1 Vitis, including building AI Engine Application and linking/packaging with prebuilt test harness.
   If you need to compile AI Engine Application using 2022.2 Vitis, you must rebuild test harness XSA from source with 2022.2 Vitis.

Rebuilding from Source
----------------------

The prebuilt test harness XSA can optionally be rebuilt from source as follows::

    cd Test_Harness/test_harness
    source <path to Vitis installation>/settings64.sh
    make xsa
