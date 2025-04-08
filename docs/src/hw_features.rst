.. 
   MIT License

   Copyright (C) 2023-2025 Advanced Micro Devices, Inc.

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

The test harness leverages a precompiled .xsa file which is used as an input platform when compiling the AIE graph. 
This allows skipping the v++ link step after compiling the libadf.a and directly go to the v++ package step to generate the `XCLBIN` for testing. 
This saves the most time-consuming part of the build process for on-board tests and allows to fast and predictable iterations.


PL DMA Engine
=============

On VCK190, the precompiled .xsa implements a PL DMA engine with 72 independent channels. 
36 Tx channels send data from DDR to AIE and 36 Rx channels receive data from AIE to DDR. 

On VEK280, the precompiled .xsa implements a PL DMA engine with 32 independent channels. 
16 Tx channels send data from DDR to AIE and 16 Rx channels receive data from AIE to DDR. 

Each channel is connected to an unique AIE PLIO port using 128-bit wide AXI-Stream clocked at 312.5MHz. 
This delivers an effective throughput of 5GB/sec (32-bits @ 1.25GHz from AIE view)

The channels are parameterized, allowing the user to control at runtime how data is transferred to or from the AI Engine. 
The software APIs are used to configure these parameters (size, delay, replay if needed).

Testing Modes
-------------

The AIE Test Harness supports two testing modes: Performance testing and Functional testing.

**Performance Testing Mode**

On both VCK190 and VEK280, the DMA engine for performance testing is designed to allow a maximum throughput on the PLIO interfaces 
(128-bit @ 312.5MHz from PL view), 
ensuring that the data transfer between AIE and PL isn't artificially stalled by the DMA channels 
and thereby allowing accurate performance testing in hardware.

Each channel contains its own URAM memory (64kB for VCK190 / 128kB for VEK280) used to buffer data. 
For datasets exceeding the size of the local URAM memory, the DMA engine automatically generates pseudo-random data 
and appends it to the data sequence sent to the AIE.

This allows transferring data to or from the AIE without unwanted external performance side-effects and deliver maximum throughput.

**Functional Testing Mode**

On both VCK190 and VEK280, the DMA engine for functional testing is designed to allow testing the graph with very large datasets of 
user-provided data. 
In this mode, the user-provided data is directly transferred between the DDR and the AIE without being buffered in the local URAM memory. 

In functional testing mode, stalls will be introduced when accessing DDR. 
Performance will not be representative of the maximum potential throughput of the graph.


Data Size
---------

The data size parameter specifies the size in bytes of the dataset to be transferred (sent or received). 
The size must be a multiple of 16 bytes.

In performance testing mode, the amount of user-provided data is limited by the size of the local buffer (64kB on VCK190 and 128kB on VEK280). 
If the data size specified by the user exceeds the size of the local buffer, 
the DMA engine will insert pseudo-random data after transfering the user-provided data to ensure that specified data size gets transfered.

Examples:
- If the user specifies a data size of 32kB on VCK190, the DMA engine will transfer 32kB of user-provided data to the AIE
- If the user specified a data size of 72kB on VCK190, the DMA engine will transfer 64kB of user-provided data, followed by 8kB of pseudo-random data.


In functional testing mode, the amount of user-provided data is not limited, allowing the user to provide very large datasets. 
The entire set of user-provided data is transfered to the AIE. This mode doesn't insert pseudo-random data in the test sequence. 


Start Delay
-----------

The start delay parameter specifies the number of cycles that each channel should be waiting before it starts to send/receive data. 
This can be used to compose more granular tests and to model more realistic performance scenarios. By default, the start delay is 0.

Replay
------

The replay parameter specifies how many times each dataset should be replayed (sent or received). 
The replay parameter is useful to generate longer test sequences by sending or receiving a dataset multiple times. 
It is valid only in the performance testing mode. By default, each dataset is transfered only once.

Performance Counter
-------------------

Each channel has its own built-in performance counter which starts counting cycles once the channel starts to send/receive data. 
It will stop counting until all sending/receiving are finished. 
The counter keeps counting even when the channel is stalled by the AIE graph. 
The performance counter helps to profile the real throughput of the AI Engine application.
Users could also obtain the delays in number of clock cycles from the first data presented in one PLIO 
to the first data presented in another PLIO by using the given API.


DMA Channel Operating Sequences
-------------------------------

**Performance Testing Mode**

Tx Channels sending data to the AI Engine work as follows:

1. Each channel loads a user-defined quantity of data from DDR and stores it in its own local URAM buffer. 
2. After all data has been stored in the URAM buffer, each channel starts a countdown based on its 'delay' parameter. This allows Tx channels to start sending data to the AIE at different starting time points.
3. Once the countdown reaches 0, each channel loads data from the URAM buffer and sends it to AIE. Each channel can repeat this step based on an user-defined number of replays to ensure both result correctness and performance, or in another way which is only supported on VCK190, random sequence can be concatenated to the data that is preloaded to the URAM to supplement the data sequence to the length that is required by the user.
4. Each channel reports the latency (cycle count) between the time it starts to send the first data and it starts to send the last data to the AIE. 

Rx Channels receiving data from the AI Engine work as follows:

1. Each channel starts a countdown based on its 'delay' parameter. This allows Rx channels to start receiving data from the AIE at different time points.
2. Once the countdown reaches 0, each channel starts receiving an user-defined quantity of data from the AIE through PLIOs and stores it in its local URAM buffer. Each channel can repeat this step based on an user-specified number of replays, or in another way which is only supported on VCK190, a pre-defined length of data will be received from the AIE and dropped by the PL data mover to ensure the whole system won't be stalled.
3. After all output channels have finished receiving data from the AIE, they move the output data out from the URAM buffers to DDR.
4. Each channel reports the latency (cycle count) between the time it receives the first data and the time it receives the last data from the AIE. 

**Functional Testing Mode**

1. The data are not preloaded to the URAM for each channel as in the performance mode, but all stored in the DDR. A small buffer is designed for each channel to maximize the DDR bandwdith utilization.
2. The Tx channels transfer data directly from the DDR to the AIE PLIOs, and RX channels transfer data directly from AIE PLIOs to the DDR.
3. Since the bandwidth requirements on the AIE are likely to exceed the maxmimum bandwidth of the DDR controller, the DMA channels will not operate at maximum throughput. The performance numbers are gathered when running in this mode, but it doesn't reflect the best performance the AIE design can achieve.
4. Replay is not supported in this mode. The start delay is still supported.

.. _plio_placement:


Placement of AIE PLIOs
======================

Each PLIO defined in the precompiled .xsa is assigned to an unique AIE SHIM channel, the name reflects the index and the direction of the PLIO. 
The placement can be described as follows:

- ``PLIO_xx_TO_AIE``: these PLIOs are independent channels that could be used to send data from PL DDR to AIE.
- ``PLIO_xx_FROM_AIE``: these PLIOs are independent channels that could be used to receive data from AIE to PL DDR.

When declaring PLIOs in the AIE graph, the developer must use one of these predefined PLIO names. 
This lets the developer control which AIE interface tiles are used and thereby, influence the results of the AIE mapper and router.
To be noticed that the VEK280 only has 16 input PLIOs and 16 output PLIOs respectively, 
so the first 16 ``in_names`` and ``out_names`` of the VCK190 PLIO names are valid for VEK280.


The predefined PLIO names are listed in :url_to_repo:`include/aie/test_harness_port_name.hpp`::

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

The placement for VCK190 of the PLIOs both ``TO_AIE`` and ``FROM_AIE`` can be seen in :url_to_repo:`cfg/vck190_aie_constraints.json`::

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

The placement for VEK280 of the PLIOs both ``TO_AIE`` and ``FROM_AIE`` can be seen in :url_to_repo:`cfg/vek280_aie_constraints.json`::

    "PLIO_01_TO_AIE":     {"shim": {"column": 12, "channel": 0 } },
    "PLIO_02_TO_AIE":     {"shim": {"column": 13, "channel": 0 } },
    "PLIO_03_TO_AIE":     {"shim": {"column": 14, "channel": 0 } },
    "PLIO_04_TO_AIE":     {"shim": {"column": 15, "channel": 0 } },
    "PLIO_05_TO_AIE":     {"shim": {"column": 16, "channel": 0 } },
    "PLIO_06_TO_AIE":     {"shim": {"column": 17, "channel": 0 } },
    "PLIO_07_TO_AIE":     {"shim": {"column": 18, "channel": 0 } },
    "PLIO_08_TO_AIE":     {"shim": {"column": 19, "channel": 0 } },
    "PLIO_09_TO_AIE":     {"shim": {"column": 20, "channel": 0 } },
    "PLIO_10_TO_AIE":     {"shim": {"column": 21, "channel": 0 } },
    "PLIO_11_TO_AIE":     {"shim": {"column": 22, "channel": 0 } },
    "PLIO_12_TO_AIE":     {"shim": {"column": 23, "channel": 0 } },
    "PLIO_13_TO_AIE":     {"shim": {"column": 24, "channel": 0 } },
    "PLIO_14_TO_AIE":     {"shim": {"column": 25, "channel": 0 } },
    "PLIO_15_TO_AIE":     {"shim": {"column": 26, "channel": 0 } },
    "PLIO_16_TO_AIE":     {"shim": {"column": 27, "channel": 0 } },

    "PLIO_01_FROM_AIE":   {"shim": {"column": 12, "channel": 0 } },
    "PLIO_02_FROM_AIE":   {"shim": {"column": 13, "channel": 0 } },
    "PLIO_03_FROM_AIE":   {"shim": {"column": 14, "channel": 0 } },
    "PLIO_04_FROM_AIE":   {"shim": {"column": 15, "channel": 0 } },
    "PLIO_05_FROM_AIE":   {"shim": {"column": 16, "channel": 0 } },
    "PLIO_06_FROM_AIE":   {"shim": {"column": 17, "channel": 0 } },
    "PLIO_07_FROM_AIE":   {"shim": {"column": 18, "channel": 0 } },
    "PLIO_08_FROM_AIE":   {"shim": {"column": 19, "channel": 0 } },
    "PLIO_09_FROM_AIE":   {"shim": {"column": 20, "channel": 0 } },
    "PLIO_10_FROM_AIE":   {"shim": {"column": 21, "channel": 0 } },
    "PLIO_11_FROM_AIE":   {"shim": {"column": 22, "channel": 0 } },
    "PLIO_12_FROM_AIE":   {"shim": {"column": 23, "channel": 0 } },
    "PLIO_13_FROM_AIE":   {"shim": {"column": 24, "channel": 0 } },
    "PLIO_14_FROM_AIE":   {"shim": {"column": 25, "channel": 0 } },
    "PLIO_15_FROM_AIE":   {"shim": {"column": 26, "channel": 0 } },
    "PLIO_16_FROM_AIE":   {"shim": {"column": 27, "channel": 0 } }
