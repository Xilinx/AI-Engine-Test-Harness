.. 
   MIT License

   Copyright (C) 2023-2025 Advanced Micro Devices, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.

AIE Test Harness
=================

AIE (AI Engine) Test Harness is designed to assist AIE application designers in verifying and benchmarking their designs 
on the **Versal** platforms such as the VCK190 and VEK280, with the ease of a software simulator. 
Most importantly, AIE Test Harness allows the developers to conduct testing across diverse host systems, including x86 and ARM, 
from various development environments like Matlab and Python.

This test harness is designed to provide a very simple and intuitive transition from the AIE simulation environment to 
testing user AIE graph on hardware. With the test harness, AIE graphs running in simulation can be taken to hardware in just a few minutes 
and with only a few minor modifications required.



AIE Test Harness Virtualization
+++++++++++++++++++++++++++++++

The virtualization feature of the test harness offers AIE designers a seamless and efficient testing experience by 
abstracting away the complexities of network communications and hardware specifics, such as device controls. 
This feature allows designers to focus entirely on their AIE designs without needing to interact directly with the underlying hardware. 
By leveraging the client APIs available in C++, Python, and MATLAB, designers can create testbenches that operate entirely on local hosts. 
This setup ensures that all interactions with the test harness server are handled virtually, eliminating the need for manual data exchanges or server modifications during testing. As a result, the virtualization feature streamlines the testing process, making it more accessible and less time-consuming for developers.

.. image:: /images/test_harness_setup.png
   :alt: Test Harness System Architecture
   :width: 80%
   :align: center

Test Harness Server
--------------------
The server application, included with the repository, runs continuously on the Versal board and accepts connections from multiple clients 
on a first-come, first-served basis. Once a connection is established, 
the server processes client requests to perform a series of actions for testing. 
These actions include initializing the device with AIE designs provided by the clients, running the AIE graphs 
and DMA with client-supplied data, and waiting for transactions to complete before sending the results back to the clients. 
If required, the server can also dump AIE event traces and transmit them to the clients. After completing these tasks, 
the server cleans all user data and resets the board, ensuring a fresh start for subsequent testing sessions.

Test Harness Client APIs
------------------------
With the test harness, AIE designers can focus solely on their designs without worrying about network communications or hardware details, 
such as device controls. The testbench is created using the test harness client APIs, which are available in C++, Python, or MATLAB. 
These APIs empower designers to efficiently manage the testing process by allowing them to connect to the server, load the AIE design, 
and run tests in transactions. Designers can also wait for transactions to complete or set a timeout, measure performance, 
verify functionalities, and request AIE event traces. 
Additionally, the APIs provide the capability to disconnect from and reset the board, 
ensuring a streamlined and user-friendly testing experience.

Testing Mode
+++++++++++++
There are two useful testing modes named functional testing mode and performance testing mode.

Functional Testing Mode
-----------------------
In the functional testing mode, it aims to verify functional correctness of the implementation on hardware with a large datasets provided by the users. In this mode, all user-provided data are transferred between the DDR and the AIE with a proper buffering. In the this mode, stalls may be introduced due to the bandwidth limitation when accessing DDR. Hence, the measured performance is not representative of the maximum potential throughput of the graph.

Performance Testing Mode
------------------------
In the performance testing mode, the DMA engine for performance testing is designed to allow a maximum throughput on the PLIO interfaces, ensuring that the data transfer between AIE and PL isn’t artificially stalled by the DMA channels and thereby allowing accurate performance testing on the hardware.

Test Harness Pre-built XSA
++++++++++++++++++++++++++

For each supported platform e.g., VCK190 and VEK280, pre-built xsa is provided along with the release of this repository, 
and is used as a platform (with necessary hardware information) on which the designers compile the AIE graph. 
This allows skipping the v++ link step after compiling the libadf.a and directly go to the v++ package step to generate the hardware boot image. 
This saves the most time-consuming part of the build process for on-board tests and allows to fast and predictable iterations.
The major component of the XSA is the DMA designed on the PL, 
which feeds data to AIE application or fetch data from AIE application via AXI-streams. 
It allows the users to skip the time-consuming v++ link stage and directly go to v++ package stage to 
generate the final .xclbin file for on-board tests.

On VCK190, the precompiled .xsa implements a PL DMA engine with 72 independent AXI stream channels. 
36 channels send data from DDR to AIE and 36 channels receive data from AIE to DDR. 
On VEK280, it has 32 independent AXI stream channels that 16 channels can send data from DDR to AIE and 16 channels can receive data from AIE to DDR. 
For both of the DMA engines, they are designed to allow a maximum throughput on the PLIO interfaces (128-bit @ 312.5MHz), 
ensuring that the AIE graph isn't artificially stalled by the DMA channels and thereby allowing accurate performance testing on the hardware.


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
