.. 
   Copyright 2019 Xilinx, Inc.
  
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
  
       http://www.apache.org/licenses/LICENSE-2.0
  
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

.. _overview:

.. toctree::
   :hidden:

Hareware Features
==================

URAM buffer
------------

Test harness on PL consists of 16 output channels and 16 input channels. Each channel has its own URAM based buffer which is 128 bits wide and 8192 deep. For output channels, test harness will first load data from DDR to buffer then load data from buffer to AI Engine through AXI-Stream. For input channels, test harness will first store data from AI Engine to buffer then load data from buffer to DDR. With the help of buffer on PL, test harness can communicate with AI Engine without suffering from DDR accessing latency. 

Pre-set delays
---------------

Each test harness channel can wait for a pre-set number of cycles before it starts to send/receive data. It will help developers to compose more granular tests. 

Cycle counter
--------------

Each channel of test harness has its own built-in cycle counter which will start counting once the channel starts to send / receive data. It will stop counting once all sending / receiving are finished. The counter will not stop even its channel got stuck on sending / receiving. It can help profile real throughput of AI Engine application.

Replay
-------

The URAM buffer for each channel is limited. To support longer tests, each channel can repeat sending / receiving data for a given repetition count. By sending / receiving the same data frames multiple times, test harness can mimic a longer test input / output.
