.. 
   MIT License

   Copyright (C) 2023-2025 Advanced Micro Devices, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.

.. _release_notes:

.. toctree::
   :hidden:

Release Notes
=============



2025.1, 2024.2 and 2024.1
-------------------------

**Summary**

- Support the client-server mode to use the AIE Test Harness
- Client APIS are available in C++, Python and Matlab to drive the test harness
- Pre-built SD card images are available for VCK190 and VEK280 boards for a simple one-time setup
- Server applications are released within the SD card image
- Watchdog timer is implemented in the server application to ensure that the server is running correctly

- Pre-built XSAs are provided for VCK190 and VEK280 boards
- On VCK190, support for graphs with up to 36 input PLIOs and 36 output PLIOs
- On VEK280, support for graphs with up to 16 input PLIOs and 16 output PLIOs
- Functional testing mode is supported on both VCK190 and VEK280 boards
- In the functional testing mode, users could manage the test vectors into multiple transactions, the data size for each transaction is limited to maximum 512MB
- PLIO throughput for each channel is 5GB/sec (32 bits @ 1.25GHz)
- User-defined number of replays for upscaling the size of datasets in performance testing mode on each input and output channel
- User-defined start delay for each input and output PLIO
- Built-in support for AIE event trace

**Limitations**

- On VCK190, the performance testing mode supports up to 64kB of user-provided data per channel 
- On VEK280, the performance testing mode supports up to 128kB of user-provided data per channel 
- The size (in bytes) of input and output datasets must be a multiple of 16
- AIE graphs with GMIOs are not supported

2023.2
------

**Summary**

- Support for testing graphs on VEK280 boards
- On VCK190, support for graphs with up to 36 input PLIOs and 36 output PLIOs
- On VEK280, support for graphs with up to 16 input PLIOs and 16 output PLIOs
- Performance testing mode has been enhanced to allow testing the performance of the graph with a sequence of user-provided data followed by a user-specified number of randomly generated values
- New functional testing mode allows testing the functionality of the graph without any limit on the size of the dataset. 
- PLIO throughput for each channel is 5GB/sec (32 bits @ 1.25GHz)
- User-defined number of replays for upscaling the size of datasets in performance testing mode on each input and output channel
- User-defined start delay for each input and output PLIO
- Built-in support for AIE event trace
- SW APIs to develop a test application to run the AIE graph and transfer data to and from the AIE
- Utility scripts to build the application and create a SD card image for board run on VCK190 and VEK280


**Limitations**

- On VCK190, the performance testing mode supports up to 64kB of user-provided data per channel 
- On VEK280, the performance testing mode supports up to 128kB of user-provided data per channel 
- The functional testing mode is not supported on VEK280
- SW emulation flow is not supported on VEK280
- AIE graphs with GMIOs are not supported
- The size (in bytes) of input and output datasets must be a multiple of 16


2023.1
------

**Summary**

This is the first release of the AIE Test Harness. This release supports the following:

- AIE graphs with up to 16 input PLIOs and 16 output PLIOs
- PLIO throughput for each channel is 5GB/sec (32 bits @ 1.25GHz)
- Input and output datasets up to 128KB in size
- User-defined number of replay for each input and output dataset
- User-defined start delay for each input and output PLIO
- Built-in support for AIE event trace
- SW APIs to develop a test application to run the AIE graph and transfer data to and from the AIE graph
- Utility scripts to build the test for SW emulation and create a SD card image for VCK190 board run


**Limitations**

- The precompiled .xsa only supports the VCK190 board
- AIE graphs with GMIOs are not supported
- Datasets larger than 128KB are not supported. For longer input / output sequences, the "replay" functionality should be used
- The size (in bytes) of input and output datasets must be a multiple of 16
