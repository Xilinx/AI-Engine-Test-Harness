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

Client APIs
+++++++++++

The AIE test harness includes software APIs for user to easily build SW applications to test an AIE graph on hardware board. 
The SW application can be executed on the both the x86 and the embedded ARM processor of the Versal device. 
The APIs are designed to structure the test applications following these simple steps:

- Create socket and connect to the server with the given IP address and port.
- Initialize the device and load the xclbin.
- Run the AIE graph with the given iterations.
- Configure the DMA channels and start the DMA for the data transfers between the PL and AIE graph.
- Wait for all transactions done and receive all results from the server.
- Report throughputs of each channel, delays between two given channels and and optionally check correctness of results on functional testing mode or performance testing mode with limited data size
- Automatically synchronize the AIE event traces from the server to the local working directory when required.

The test harness client APIs are included in :url_to_repo:`include/ps/test_harness_mgr_client.hpp`. 


Initialization
--------------

.. cpp:class:: test_harness_mgr_client
   
   The test harness manager class. This class encapsulates the necessary runtime objects to interact with the hardware DMA channels and the AIE graph. All test harness APIs belong to this class.

.. cpp:function:: test_harness_mgr_client::test_harness_mgr_client(std::string xclbin_file_path, const std::vector<std::string>& graph_name, const std::string& device_name, const uint64_t timeout_miliseconds = 0, unsigned int device_index = 0)

   The test harness manager class constructor. Loads the xclbin to the device and initializes the various test harness runtime objects.


**Parameters**

``std::string xclbin_path``
  The path and name of the xclbin file to be loaded and used for testing. The xclbin file must contain the AI Engine graph(s) to be tested.
``std::vector<std::string> graph_name``
  The vector of graph names in the libadf.a packaged in the xclbin file
``std::string device_name``
   The device name for the testing board. The device name is used to identify the target device in the xclbin file, acceptable values are ``vck190`` and ``vek280``.
``const uint64_t timeout_miliseconds``
   The timeout limit in milliseconds for connecting to the server. This parameter is optional, and the default value is zero, which means no timeout limit.
``unsigned int device_index``
  The device ID of the testing board, by default it is zero


Running the AIE Graph
---------------------

.. cpp:function:: void test_harness_mgr_client::runAIEGraph(unsigned int g_idx, unsigned int iters)

   Run the specified AI Engine graph for a specific number of iterations. To ensure accurate performance results, this function should be called **before** :cpp:func:`test_harness_mgr_client::runTestHarness` when user wants to profile a valid performance after the HW board-run.  

**Parameters**

``unsigned int g_idx``
  The index of the graph in the vector of graph names passed to the class constructor. For designs with a single graph, this argument is zero.
``unsigned int iters``
  The number of graph iterations to run


Running the Test Harness
------------------------
.. cpp:enum:: TestMode

   The test harness can run in either functional testing mode ``FUNC_MODE`` or performance testing mode ``PERF_MODE``. The functional testing mode is used to verify the functionalities of the AIE design, while the performance testing mode is used to measure the performance e.g., throughput and latency.

.. cpp:enum:: channel_index

   Enumerated type listing the index of each DMA channel. The enumerated values corresponding to the names of the predefined PLIOs: :cpp:enum:`PLIO_01_TO_AIE`, :cpp:enum:`PLIO_02_TO_AIE`, etc...

.. cpp:class:: test_harness_args

   This class encapsulates the necessary information to configure the DMA channels for data transfers between the PL and AIE.

**Members**

``idx``
  The index of the DMA channel to be used, specified using the :cpp:enum:`channel_index` enum. The index corresponds to the name of the targeted AIE PLIO, and implicitly specifies the direction of the data transfer. 
``size_in_byte``
  The size (in bytes) of one data frame to be transferred to or from the AIE
``replay``
  Number of replays used for test harness to perform the data transfer between PL and AIE multiple times, only valid in the performance testing mode.
``delay``
  The start delay (in PL clock cycles) for this channel
``data``
  Pointer to the data that you want read or receive


.. cpp:function:: void test_harness_mgr_client::runTestHarness(TestMode mode, const std::vector<test_harness_args>& args, uint32_t timeout = 0)

   Create a test harness transaction to run the test harness in the given mode with the specified DMA channel arguments. In performance testing mode, to ensure accurate performance results, this function should be called after :cpp:func:`test_harness_mgr_client::runAIEGraph`.  

**Parameters**

``TestMode mode``
   The testing mode to be used e.g., ``FUNC_MODE`` or ``PERF_MODE``
``std::vector<test_harness_args> args``
  A vector of DMA channel arguments
``uint32_t timeout``
   Timeout limit in milliseconds for the test harness to finish. This parameter is optional, and the default value is zero, which means no timeout limit.


Waiting for Completion
----------------------

.. cpp:function:: void test_harness_mgr_client::waitForRes(uint32_t timeout_millisec, uint32_t num_trans = 0)

   Wait for ``num_trans`` test harness transactions done and get back the results to the client application. 

**Parameters**

``int graph_timeout_millisec``
  Timeout limit in milliseconds for test harness transactions to finish. This function can be blocked permanently if the DMA channels are configured to send or receive more data than what the AIE graph will actually consume or produce.
``uint32_t num_trans``
   The number of transactions to wait for. If the value is zero, the function will wait for all unfinished transactions and all graph iteration to finish.

Reporting Performance
---------------------

.. cpp:function:: void test_harness_mgr_client::printPerf(int32_t t_idx = -1)

   Print the total number of cycles elapsed between the start and the end of the data transfer, and the throughputs for each DMA channel of given transaction. Must be called once the :cpp:func:`test_harness_mgr_client::waitForRes` is returned.

**Parameters**

``int32_t t_idx``
  The index of the transaction to be printed. If the value is -1, the performance of the last finished transaction is printed.

.. cpp:function:: bool test_harness_mgr_client::isResultValid(int32_t t_idx = -1)

   Check if the results of the given transaction are valid. Must be called once the :cpp:func:`test_harness_mgr_client::waitForRes` is returned. The results could be invalid due to timeout transactions or large amount of data are used in the performance testing mode.

**Parameters**
   
   ``int32_t t_idx``
   The index of the transaction to be checked. If the value is -1, the results of the last finished transaction are checked.

.. cpp:function:: int test_harness_mgr_client::getDelayBetween(channel_index, channel_index, int32_t t_idx = -1)
   
      Get the delay between two given channels in the given transaction. Must be called once the :cpp:func:`test_harness_mgr_client::waitForRes` is returned.

**Parameters**
   
   ``channel_index``
   The index of the first channel
   ``channel_index``
   The index of the second channel
   ``int32_t t_idx``
   The index of the transaction to be checked. If the value is -1, the results of the last finished transaction are checked.


Server Application
++++++++++++++++++

The AIE test harness server application is fixed and keeps running on the Arm processor of target Versal board.  
The server application is responsible for the following tasks:

- Create socket and wait for the client connection.
- Create a test harness session and handshake with the client to synchronize the test harness settings with the client e.g., the number of available PLIOs
- Initialize the device and load the xclbin from the client when requested.
- Run the AIE graph with the given iterations when requested.
- Run the test harness DMA with the data from the client when requested.
- Wait for the DMA transactions to finish and send the results back to the client.
- Wait for the graph iterations to finish.
- Synchronize the AIE event traces to the client when required.
- Clean up the resources and user data when the client disconnects.

Usage of server application
---------------------------
A script named ``run_server.sh`` is provided to run the server application on the target board, refer to the :ref:`step by step example <step_by_step>`.
The script is located in the repo :url_to_repo:`test_harness/scripts/run_server.sh`.
The pre-built application named ``test_harness_server`` is included in the SD card image. 
The source code is located in the repo :url_to_repo:`src/ps/test_harness_main.cpp`::

   test_harness_server <port> <timeout_in_seconds > <reset_flag> <max_num_sessions>

   - <port> is the port number to be used for the socket connection.
   - <timeout_in_seconds> is the timeout limit in seconds for the server watchdog to terminate a session if no valid activity detected.
   - <reset_flag> is the flag to reset the whole device for each session.
   - <max_num_sessions> is the maximum number of sessions to be supported by the server. 0 means no limit.

