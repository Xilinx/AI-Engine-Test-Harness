.. 
   MIT License

   Copyright (C) 2023-2025 Advanced Micro Devices, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.

.. _sw_apis:

.. toctree::
   :hidden:

Software APIs
=============

The AIE test harness includes software APIs for user to easily build SW applications to test an AIE graph on hardware board. 
The SW application is executed on the embedded ARM processor of the Versal device. 
The APIs are designed to structure the test applications following these simple steps:

1. Initialize the device, load the xclbin, and inferring the testing mode
2. Run the AIE graph as well as the PL data mover
3. Configure the DMA channels and start the data transfers between the PL and AIE graph
4. Wait for all the data to be sent/received
5. Report performance for each channel on performance testing mode, and optionally check correctness of results on functional testing mode or performance testing mode with limited data size

The test harness APIs are included in :url_to_repo:`include/test_harness_mgr_client.hpp`. 

.. toctree::
   :maxdepth: 0
   :caption: Test Harness Client APIs

   rst/class_test_harness_test_harness_mgr_client.rst