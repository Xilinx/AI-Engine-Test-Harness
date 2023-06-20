.. 
   MIT License

   Copyright (C) 2023 Advanced Micro Devices, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.

.. _release_notes:

.. toctree::
   :hidden:

Release Notes
=============

Summary
-------

This is the first release of the AIE Test Harness. This release supports the following:

- AIE graphs with up to 16 input PLIOs and 16 output PLIOs
- PLIO throughput of 5GB/sec (32 bits @ 1.25GHz)
- Input and output datasets up to 128KB in size
- User-defined number of replay for each input and output dataset
- User-defined start delay for each input and output PLIO
- Built-in support for AIE event trace
- SW APIs to develop a test application to run the AIE graph and transfer data to and from the AIE graph
- Utility scripts to build the test for SW emulation and create a SD card image for VCK190


Limitations
-----------

- The precompiled .xsa only supports the VCK190 board
- AIE graphs with GMIOs are not supported
- Datasets larger than 128KB are not supported. For longer input / output sequences, the "replay" functionality should be used
- The size (in bytes) of input and output datasets must be a multiple of 16
