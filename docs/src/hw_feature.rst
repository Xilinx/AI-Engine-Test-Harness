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

*****************
Hardware Features
*****************


Precompiled XSA
===============

The test harness leverages a precompiled .xsa file which is used as an input platform when compiling the AIE graph. This allows skipping the v++ link step after compiling the libadf.a and directly go to the v++ package step to generate the hardware boot image. This saves the most time-consuming part of the build process for on-board tests and allows for fast and predictable iterations.


PL DMA Engine
=============

The precompiled .xsa implements a DMA engine with 32 channels. 16 Tx channels send data from DDR to AIE and 16 Rx send data from AIE to DDR. The DMA engine is designed to allow maximum throughput on the PLIO interfaces, ensuring that the AIE graph isn't artificially stalled by the DMA channels and thereby allowing accurate performance testing in hardware.

- Each channel is connected to a unique AIE PLIO port using 128-bits wide AXI-Stream clocked at 250Mhz. This delivers an effective throughput of 4GB/sec (32-bits @ 1GHz)
- Each channel contains its own 128KB URAM memory used to buffer data. This allows transferring data to or from the AIE without unwanted external performance size-effects and deliver maximum throughput.
- The channels are parametrized, allowing the user to control at runtime how data is transferred to or from the AI Engine. The software APIs are used configure these parameters (size, delay and replay).


Data Size
---------

The data size parameter specifies the size in bytes of the dataset to be transferred (sent or received). The data size cannot be larger than 128KB and must be a multiple of 16 bytes.

Start Delay
-----------

The start delay parameter specifies the number of cycles that each channel should wait before it starts to send/receive data. This can be used to compose more granular tests and to model more realistic performance scenarios. By default, the start delay is 0.

Replay
------

The replay parameter specifies how many times each dataset should be replayed (sent or received). The maximum size of a dataset is 128KB. The replay parameter is useful to generate longer test sequences by sending or receiving a dataset multiple times. By default, each dataset is transfered once.

Performance Counter
-------------------

Each channel has its own built-in performance counter which will start counting cycles once the channel starts to send / receive data. It will stop counting once all sending / receiving are finished. The counter keeps counting even when the channel is stalled by the AIE graph. The performance counter helps profile the real throughput of the AI Engine application.


DMA Channel Operating Sequences
-------------------------------

Tx Channels sending data to the AI Engine work as follows:

1. Each channel loads a user-defined quantity of data from DDR and stores it in its local URAM buffer.
2. After all data has been stored in the URAM buffer, each channel starts a countdown based on its 'delay' parameter. This allows Tx channels to start sending data to the AIE at different times.
3. Once the countdown reaches 0, each channel loads data from the URAM buffer and sends it to AIE. Each channel can repeat this step for a user-specified number of rounds.
4. Each channel reports the latency (cycle count) between when it sent the first data and when it sent the last data to the AIE.

Rx Channels receiving data from the AI Engine work as follows:

1. Each channel starts a countdown based on its 'delay' parameter. This allows Rx channels to start receiving data from the AIE at different times.
2. Once the countdown reaches 0, each channel starts receiving a user-defined quantity of data from the AIE and stores it in its local URAM buffer. Each channel can repeat this step for a user-specified number of rounds.
3. After all output channels have finished receiving data from the AIE, they write the output data from the URAM buffers to DDR.
4. Each channel reports the latency (cycle count) between when it received the first data and when it received the last data to the AIE.


.. _plio_placement:


Placement of AIE PLIOs
======================

Each PLIO defined in the precompiled .xsa is assigned to a specific AIE interface file and is identified by a unique name. The name reflects the placement and the direction of the PLIO. 

- PL to AIE PLIOs 1 to 16 are mapped to the AIE interface tiles on columns 12 to 27, respectively.
- AIE to PL PLIOs 1 to 16 are mapped to the AIE interface tiles on columns 28 to 43, respectively.


.. image:: /images/connection.png
   :alt: stream sync Structure
   :width: 80%
   :align: center

When declaring PLIOs in the AIE graph, the developer must use one these predefined PLIO names. This lets the developer control which AIE interface tiles are used and thereby, influence the results of the AIE mapper and router.


The predefined PLIO names are listed in :url_to_repo:`include/vck190_test_harness_port_name.hpp`::

   static std::vector<std::string> in_names = {
       "Column_12_TO_AIE", "Column_13_TO_AIE", "Column_14_TO_AIE", "Column_15_TO_AIE",
       "Column_16_TO_AIE", "Column_17_TO_AIE", "Column_18_TO_AIE", "Column_19_TO_AIE",
       "Column_20_TO_AIE", "Column_21_TO_AIE", "Column_22_TO_AIE", "Column_23_TO_AIE",
       "Column_24_TO_AIE", "Column_25_TO_AIE", "Column_26_TO_AIE", "Column_27_TO_AIE"};
   static std::vector<std::string> out_names = {
       "Column_28_FROM_AIE", "Column_29_FROM_AIE", "Column_30_FROM_AIE", "Column_31_FROM_AIE",
       "Column_32_FROM_AIE", "Column_33_FROM_AIE", "Column_34_FROM_AIE", "Column_35_FROM_AIE",
       "Column_36_FROM_AIE", "Column_37_FROM_AIE", "Column_38_FROM_AIE", "Column_39_FROM_AIE",
       "Column_40_FROM_AIE", "Column_41_FROM_AIE", "Column_42_FROM_AIE", "Column_43_FROM_AIE"};
   } 

Example: the PLIO named ``Column_12_TO_AIE`` is mapped to the interface tile on column 12 and is used for to transfer data to the AIE. 


