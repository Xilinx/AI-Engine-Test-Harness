.. 
   MIT License

   Copyright (C) 2023 Advanced Micro Devices, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.

.. _hw_features:

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

The precompiled .xsa implements a DMA engine with 72 channels. 36 Tx channels send data from DDR to AIE and 36 Rx send data from AIE to DDR. The DMA engine is designed to allow maximum throughput on the PLIO interfaces, ensuring that the AIE graph isn't artificially stalled by the DMA channels and thereby allowing accurate performance testing in hardware.

- Each channel is connected to a unique AIE PLIO port using 128-bits wide AXI-Stream clocked at 312.5Mhz. This delivers an effective throughput of 5GB/sec (32-bits @ 1.25GHz)
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

Each channel has its own built-in performance counter which will start counting cycles once the channel starts to send/receive data. It will stop counting once all sending/receiving are finished. The counter keeps counting even when the channel is stalled by the AIE graph. The performance counter helps profile the real throughput of the AI Engine application.


DMA Channel Operating Sequences
-------------------------------

Tx Channels sending data to the AI Engine work as follows:

1. Each channel loads a user-defined quantity of data from DDR and stores it in its own local URAM buffer.
2. After all data has been stored in the URAM buffer, each channel starts a countdown based on its 'delay' parameter. This allows Tx channels to start sending data to the AIE at different time points.
3. Once the countdown reaches 0, each channel loads data from the URAM buffer and sends it to AIE. Each channel can repeat this step based on an user-specified number of replays.
4. Each channel reports the latency (cycle count) between when it sents the first data and when it sents the last data to the AIE.

Rx Channels receiving data from the AI Engine work as follows:

1. Each channel starts a countdown based on its 'delay' parameter. This allows Rx channels to start receiving data from the AIE at different time points.
2. Once the countdown reaches 0, each channel starts receiving an user-defined quantity of data from the AIE and stores it in its local URAM buffer. Each channel can repeat this step based on an user-specified number of replays.
3. After all output channels have finished receiving data from the AIE, they move the output data from the URAM buffers to DDR.
4. Each channel reports the latency (cycle count) between when it receives the first data and when it receives the last data to the AIE.


.. _plio_placement:


Placement of AIE PLIOs
======================

Each PLIO defined in the precompiled .xsa is assigned to an unique AIE SHIM channel, the name reflects the index and the direction of the PLIO. The placement can be described as follows:

- ``PLIO_xx_TO_AIE``: these PLIOs are independent channels that could be used to send data from PL DDR to AIE.
- ``PLIO_xx_FROM_AIE``: these PLIOs are independent channels that could be used to receive data from AIE to PL DDR.

When declaring PLIOs in the AIE graph, the developer must use one these predefined PLIO names. This lets the developer control which AIE interface tiles are used and thereby, influence the results of the AIE mapper and router.


The predefined PLIO names are listed in :url_to_repo:`include/vck190_test_harness_port_name.hpp`::

    static std::vector<std::string> in_names = {
        "PLIO_01_TO_AIE", "PLIO_02_TO_AIE", "PLIO_03_TO_AIE", "PLIO_04_TO_AIE",
        "PLIO_05_TO_AIE", "PLIO_06_TO_AIE", "PLIO_07_TO_AIE", "PLIO_08_TO_AIE",
        "PLIO_09_TO_AIE", "PLIO_10_TO_AIE", "PLIO_11_TO_AIE", "PLIO_12_TO_AIE",
        "PLIO_13_TO_AIE", "PLIO_14_TO_AIE", "PLIO_15_TO_AIE", "PLIO_16_TO_AIE",
        "PLIO_17_TO_AIE", "PLIO_18_TO_AIE", "PLIO_19_TO_AIE", "PLIO_20_TO_AIE",
        "PLIO_21_TO_AIE", "PLIO_22_TO_AIE", "PLIO_23_TO_AIE", "PLIO_24_TO_AIE",
        "PLIO_25_TO_AIE", "PLIO_26_TO_AIE", "PLIO_27_TO_AIE", "PLIO_28_TO_AIE",
        "PLIO_29_TO_AIE", "PLIO_30_TO_AIE", "PLIO_31_TO_AIE", "PLIO_32_TO_AIE",
        "PLIO_33_TO_AIE", "PLIO_34_TO_AIE", "PLIO_35_TO_AIE", "PLIO_36_TO_AIE"};
    static std::vector<std::string> out_names = {
        "PLIO_01_FROM_AIE", "PLIO_02_FROM_AIE", "PLIO_03_FROM_AIE",
        "PLIO_04_FROM_AIE", "PLIO_05_FROM_AIE", "PLIO_06_FROM_AIE",
        "PLIO_07_FROM_AIE", "PLIO_08_FROM_AIE", "PLIO_09_FROM_AIE",
        "PLIO_10_FROM_AIE", "PLIO_11_FROM_AIE", "PLIO_12_FROM_AIE",
        "PLIO_13_FROM_AIE", "PLIO_14_FROM_AIE", "PLIO_15_FROM_AIE",
        "PLIO_16_FROM_AIE", "PLIO_17_FROM_AIE", "PLIO_18_FROM_AIE",
        "PLIO_19_FROM_AIE", "PLIO_20_FROM_AIE", "PLIO_21_FROM_AIE",
        "PLIO_22_FROM_AIE", "PLIO_23_FROM_AIE", "PLIO_24_FROM_AIE",
        "PLIO_25_FROM_AIE", "PLIO_26_FROM_AIE", "PLIO_27_FROM_AIE",
        "PLIO_28_FROM_AIE", "PLIO_29_FROM_AIE", "PLIO_30_FROM_AIE",
        "PLIO_31_FROM_AIE", "PLIO_32_FROM_AIE", "PLIO_33_FROM_AIE",
        "PLIO_34_FROM_AIE", "PLIO_35_FROM_AIE", "PLIO_36_FROM_AIE"};

The placement of the PLIOs both ``TO_AIE`` and ``FROM_AIE`` can be seen in :url_to_repo:`cfg/aie_constraints.json`::

    "PLIO_01_TO_AIE":     {"shim": {"column":  6, "channel": 0 } },
    "PLIO_02_TO_AIE":     {"shim": {"column":  6, "channel": 4 } },
    "PLIO_03_TO_AIE":     {"shim": {"column":  7, "channel": 0 } },
    "PLIO_04_TO_AIE":     {"shim": {"column":  7, "channel": 4 } },
    "PLIO_05_TO_AIE":     {"shim": {"column":  8, "channel": 0 } },
    "PLIO_06_TO_AIE":     {"shim": {"column":  8, "channel": 4 } },
    "PLIO_07_TO_AIE":     {"shim": {"column":  9, "channel": 0 } },
    "PLIO_08_TO_AIE":     {"shim": {"column":  9, "channel": 4 } },
    "PLIO_09_TO_AIE":     {"shim": {"column": 10, "channel": 0 } },
    "PLIO_10_TO_AIE":     {"shim": {"column": 10, "channel": 4 } },
    "PLIO_11_TO_AIE":     {"shim": {"column": 11, "channel": 0 } },
    "PLIO_12_TO_AIE":     {"shim": {"column": 11, "channel": 4 } },
    "PLIO_13_TO_AIE":     {"shim": {"column": 12, "channel": 0 } },
    "PLIO_14_TO_AIE":     {"shim": {"column": 12, "channel": 4 } },
    "PLIO_15_TO_AIE":     {"shim": {"column": 13, "channel": 0 } },
    "PLIO_16_TO_AIE":     {"shim": {"column": 13, "channel": 4 } },
    "PLIO_17_TO_AIE":     {"shim": {"column": 14, "channel": 0 } },
    "PLIO_18_TO_AIE":     {"shim": {"column": 14, "channel": 4 } },
    "PLIO_19_TO_AIE":     {"shim": {"column": 15, "channel": 0 } },
    "PLIO_20_TO_AIE":     {"shim": {"column": 15, "channel": 4 } },
    "PLIO_21_TO_AIE":     {"shim": {"column": 16, "channel": 0 } },
    "PLIO_22_TO_AIE":     {"shim": {"column": 16, "channel": 4 } },
    "PLIO_23_TO_AIE":     {"shim": {"column": 17, "channel": 0 } },
    "PLIO_24_TO_AIE":     {"shim": {"column": 17, "channel": 4 } },
    "PLIO_25_TO_AIE":     {"shim": {"column": 18, "channel": 0 } },
    "PLIO_26_TO_AIE":     {"shim": {"column": 18, "channel": 4 } },
    "PLIO_27_TO_AIE":     {"shim": {"column": 19, "channel": 0 } },
    "PLIO_28_TO_AIE":     {"shim": {"column": 19, "channel": 4 } },
    "PLIO_29_TO_AIE":     {"shim": {"column": 20, "channel": 0 } },
    "PLIO_30_TO_AIE":     {"shim": {"column": 20, "channel": 4 } },
    "PLIO_31_TO_AIE":     {"shim": {"column": 21, "channel": 0 } },
    "PLIO_32_TO_AIE":     {"shim": {"column": 21, "channel": 4 } },
    "PLIO_33_TO_AIE":     {"shim": {"column": 22, "channel": 0 } },
    "PLIO_34_TO_AIE":     {"shim": {"column": 22, "channel": 4 } },
    "PLIO_35_TO_AIE":     {"shim": {"column": 23, "channel": 0 } },
    "PLIO_36_TO_AIE":     {"shim": {"column": 23, "channel": 4 } },

    "PLIO_01_FROM_AIE":   {"shim": {"column":  6, "channel": 0 } },
    "PLIO_02_FROM_AIE":   {"shim": {"column":  6, "channel": 4 } },
    "PLIO_03_FROM_AIE":   {"shim": {"column":  7, "channel": 0 } },
    "PLIO_04_FROM_AIE":   {"shim": {"column":  7, "channel": 4 } },
    "PLIO_05_FROM_AIE":   {"shim": {"column":  8, "channel": 0 } },
    "PLIO_06_FROM_AIE":   {"shim": {"column":  8, "channel": 4 } },
    "PLIO_07_FROM_AIE":   {"shim": {"column":  9, "channel": 0 } },
    "PLIO_08_FROM_AIE":   {"shim": {"column":  9, "channel": 4 } },
    "PLIO_09_FROM_AIE":   {"shim": {"column": 10, "channel": 0 } },
    "PLIO_10_FROM_AIE":   {"shim": {"column": 10, "channel": 4 } },
    "PLIO_11_FROM_AIE":   {"shim": {"column": 11, "channel": 0 } },
    "PLIO_12_FROM_AIE":   {"shim": {"column": 11, "channel": 4 } },
    "PLIO_13_FROM_AIE":   {"shim": {"column": 12, "channel": 0 } },
    "PLIO_14_FROM_AIE":   {"shim": {"column": 12, "channel": 4 } },
    "PLIO_15_FROM_AIE":   {"shim": {"column": 13, "channel": 0 } },
    "PLIO_16_FROM_AIE":   {"shim": {"column": 13, "channel": 4 } },
    "PLIO_17_FROM_AIE":   {"shim": {"column": 14, "channel": 0 } },
    "PLIO_18_FROM_AIE":   {"shim": {"column": 14, "channel": 4 } },
    "PLIO_19_FROM_AIE":   {"shim": {"column": 15, "channel": 0 } },
    "PLIO_20_FROM_AIE":   {"shim": {"column": 15, "channel": 4 } },
    "PLIO_21_FROM_AIE":   {"shim": {"column": 16, "channel": 0 } },
    "PLIO_22_FROM_AIE":   {"shim": {"column": 16, "channel": 4 } },
    "PLIO_23_FROM_AIE":   {"shim": {"column": 17, "channel": 0 } },
    "PLIO_24_FROM_AIE":   {"shim": {"column": 17, "channel": 4 } },
    "PLIO_25_FROM_AIE":   {"shim": {"column": 18, "channel": 0 } },
    "PLIO_26_FROM_AIE":   {"shim": {"column": 18, "channel": 4 } },
    "PLIO_27_FROM_AIE":   {"shim": {"column": 19, "channel": 0 } },
    "PLIO_28_FROM_AIE":   {"shim": {"column": 19, "channel": 4 } },
    "PLIO_29_FROM_AIE":   {"shim": {"column": 20, "channel": 0 } },
    "PLIO_30_FROM_AIE":   {"shim": {"column": 20, "channel": 4 } },
    "PLIO_31_FROM_AIE":   {"shim": {"column": 21, "channel": 0 } },
    "PLIO_32_FROM_AIE":   {"shim": {"column": 21, "channel": 4 } },
    "PLIO_33_FROM_AIE":   {"shim": {"column": 22, "channel": 0 } },
    "PLIO_34_FROM_AIE":   {"shim": {"column": 22, "channel": 4 } },
    "PLIO_35_FROM_AIE":   {"shim": {"column": 23, "channel": 0 } },
    "PLIO_36_FROM_AIE":   {"shim": {"column": 23, "channel": 4 } }
