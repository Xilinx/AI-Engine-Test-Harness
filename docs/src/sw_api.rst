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

Software API
=============

To run test harness with developer's AI Engine applications on board, we need software APIs to help PS programs to initialize device and setup parameters to run PL and AIE. You can find them in ``include/vck190_test_harness_mgr.hpp``. Bascially you can instantiate an instance of ``test_harness_mgr`` and call its member functions.

Constructor
------------

The constructor of class ``test_harness_mgr`` will help initialize the device. Argument ``device_index`` is the device id of your testing board, typically it will be zero. Argument ``xclbin_file_path`` is the path to your xclbin file to be tested. Argument ``graph_name`` is the vector of graph names which packaged inside the xclbin file.

.. code-block:: c++

   test_harness_mgr(unsigned int device_index, std::string xclbin_file_path, std::vector<std::string> graph_name);

Run AIE Graph
--------------

Function ``test_harness_mgr::runAIEGraph`` is to run the specified AI Engine graph for a specified number of iterations. Argument ``g_idx`` is the graph's reference id in list of graph names. Argument ``iters`` is the number of iterations that you'd like to run.

.. code-block:: c++

   void runAIEGraph(unsigned int g_idx, unsigned int iters);

Run Test Harness
-----------------

Function ``test_harness_mgr::runTestHarness`` is to run the test harness. Its only input is a vector of ``test_harness_args``. ``test_harness_args::idx`` is the channel you want to use; ``test_harness_args::size_in_byte`` is the size of one data frame that you want to send or receive; ``test_harness_args::delay`` is the delays cycles for this channel; ``test_harness_args::data`` is a pointer to the data that you want read or receive.

.. code-block:: c++

   void runTestHarness(std::vector<test_harness_args> args);

   struct test_harness_args {
    channel_index idx;
    unsigned int size_in_byte;
    uint64_t delay;
    char* data;
   }

Wait for Result
----------------

Function ``test_harness_mgr::waitForRes`` is to wait for AI Engine application to finish and get back the result. The only arugment ``graph_timeout_millisec`` is the time out limit for AI Engine application to finish in milliseconds.

.. code-block:: c++

   void waitForRes(int graph_timeout_millisec);

Print Performance
------------------

Function ``test_harness_mgr::printPerf()`` is to print total working cycles of each channel. It does not require any arguments but can only be called after ``test_harness_mgr::waitForRes`` returns.

.. code-block:: c++

   void printPerf();
