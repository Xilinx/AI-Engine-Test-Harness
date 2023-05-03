.. 
   MIT License

   Copyright (C) 2023 Advanced Micro Devices, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.

AIE Test Harness
=================

This repository provides a test harness to help AIE developers easily build and test their AIE graph on VCK190 boards. This test harness is designed to provide a very simple and natural transition from the AIE simulation environment to testing on hardware. With the test harness, AIE graphs running in simulation can be taken to hardware in just a few minutes and with only a few minor modifications. 

The test harness consists of three main elements:

1. A precompiled .xsa implementing a DMA engine to transfer data between DDR and a set of predefined AI Engine PLIOs.
2. A set of software APIs to facilitate the development of the application used to initialize the device and run the tests.
3. A script to easily package the user's libadf.a and test application, and generate the boot image for VCK190.


.. image:: /images/sys_diagram.png
   :alt: stream sync Structure
   :width: 80%
   :align: center


The test harness leverages a precompiled .xsa file which is used as an input platform when compiling the AIE graph. This allows skipping the v++ link step after compiling the libadf.a and directly go to the v++ package step to generate the hardware boot image. This saves the most time-consuming part of the build process for on-board tests and allows for fast and predictable iterations.

The precompiled .xsa implements a DMA engine with 32 channels. 16 channels send data from DDR to AIE and 16 send data from AIE to DDR. The DMA engine is designed to allow maximum throughput on the PLIO interfaces, ensuring that the AIE graph isn't artificially stalled by the DMA channels and thereby allowing accurate performance testing in hardware.

The software APIs enable to easily build SW applications to test an AIE graph using the test harness. The APIs are designed to structure test applications following these 5 simple steps:

1. Initialize the device and load the xclbin
2. Configure how each DMA channel with transfer data to or from the AIE graph
3. Run the AIE graph
4. Wait for all the data to be received
5. Check correctness of results and performance for each channel


.. toctree::
   :caption: Introduction
   :maxdepth: 1
   :hidden:

   Release Notes <release_notes.rst>
   Installation <installation.rst>

.. toctree::
   :caption: User Guide
   :maxdepth: 1
   :hidden:

   Hardware Features <hw_features.rst>
   Software APIs <sw_apis.rst>
   Using the Test Harness <using_the_harness.rst>
   Step By Step Example <step_by_step_example.rst>

.. toctree::
   :caption: Additional Topics
   :maxdepth: 1
   :hidden:

   License <license.rst>
   Trademark <trademark.rst>
