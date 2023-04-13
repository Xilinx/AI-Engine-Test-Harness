.. 
   MIT License

   Copyright (C) 2023 Advanced Micro Devices, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.

.. _overview:

.. toctree::
   :hidden:

Hardware Features
==================


URAM buffer
------------

Test harness on PL consists of 16 output channels and 16 input channels. Each channel has its own URAM based buffer which is 128 bits wide and 8192 deep. For output channels, test harness will first load data from DDR to buffer then load data from buffer to AI Engine through AXI-Stream. For input channels, test harness will first store data from AI Engine to buffer then load data from buffer to DDR. With the help of buffer on PL, test harness can communicate with AI Engine without suffering from DDR accessing latency. 

Fix connection with AIE interface tile
---------------------------------------

All 16 output channels and 16 input channels of pre-buildt ``vck190_test_harness.xsa`` are connected to fixed AIE interface tile. In order to utilize these channel, AIE application need to specify its input_plio and output_plio with the valid names specified in ``include/vck190_test_harness_port_name.hpp``. Take ``Column_12_TO_AIE`` as example, ``Column_12`` means it's name for AIE interface tile at column 12 and ``TO_AIE`` means that it's used for input_plio to feed data to AIE. Current implementation of test harness utilize column 12 to column 27 to feed input to AIE and column 28 to column 43 to receive data from AIE.

.. image:: /images/connection.png
   :alt: stream sync Structure
   :width: 80%
   :align: center

Pre-set delays
---------------

Each test harness channel can wait for a pre-set number of cycles before it starts to send/receive data. It will help developers to compose more granular tests. 

Cycle counter
--------------

Each channel of test harness has its own built-in cycle counter which will start counting once the channel starts to send / receive data. It will stop counting once all sending / receiving are finished. The counter will not stop even its channel got stuck on sending / receiving. It can help profile real throughput of AI Engine application.

Replay
-------

The URAM buffer for each channel is limited. To support longer tests, each channel can repeat sending / receiving data for a given repetition count. By sending / receiving the same data frames multiple times, test harness can mimic a longer test input / output.
