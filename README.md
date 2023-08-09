# AIE Test Harness

The main purpose of this repository is to provide PL test harness that help AIE application designers to do quick build and on-board tests with VCK190.

Vitis allows compiling an AIE graph using a precompiled .xsa as an input platform to skip v++ link stage and directly go to v++ package stage.
This will save the most time consuming part of compiling and generate the final .xclbin file for on-board tests.

The test harness consists of three parts:

1.	An HLS based test harness design on PL. It can feed data to AIE application or fetch data from AIE application via AXI-stream.
2.	An ADF dummy graph design on AIE. If user AIE application did not utilize all PLIOs provided by test harness, it would occupy the unused PLIOs to help pass v++ package stage.
3.	A suite of software APIs on host side to help initialize device and run tests.

## Features

### Test Harness on PL

Test harness on PL consist of 72 channels, 36 of them feed data to AIE and the rest 36 fetch data from AIE.
Each channel contains its own URAM buffer which is 128bits x 4096 and its own AXI-stream port for input or output.

The input channel works in steps below:

1. All input channels will load input data for AIE from DDR and store them in URAM buffer.
2. After all data has been stored in URAM buffer, each channel will start to do its own count down. This allows input channels start to send data to AIE at different cycles.
3. Each channel load from URAM buffer and send desired amount of data to AIE. Each of them can repeat the sending with specified rounds.
4. Each channel can record the latency between the cycle in which it starts to send data to AIE and the cycle in which it finishes all sendings.

The output channels works in steps described as below:

1. Each channel will start to do its own count down. This allows output channels start to fetch data from AIE at different cycles.
2. Each channel fetch desired amout of data from AIE and store them in URAM buffer. Each of them can repeat the fetching with specified rounds.
3. After all output channels finished all fetchings, they will write output data on URAM to DDR.
4. Each channel can record the latency between the cycle in which it starts to fetch data from AIE and the cycle in which it finishes all fetchings.

The data sizes to feed to/fetch from AIE, count-down cycles and repetition number of each channel are all set by config buffer on DDR which is set by software API.
The latency cycles of each channels will be stored to DDR and fetch back and displayed by software API.

### Dummy Graph on AIE

Test harness on PL provides 36 PLIO for input and 36 PLIO for output. If user graph does not utilize all of them, it will lead to package error.
Dummy graph can be used to occupy all dangling PLIOs. The dummy graph won't do anything but help packaging.
To help dummy graph identify all dangling PLIOs, user graph needs to register all used PLIOs.
For details, please take reference from "Step by Step Example" section in the documentation.

### Software APIs

We provide software APIs to make 3-steps on board run:

1. Initialize device and load xclbin.
2. Set data which will be fed to AIE and data which will be fetch from AIE.
3. Run user graph.


## License

MIT License

Copyright (C) 2023 Advanced Micro Devices, Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.
