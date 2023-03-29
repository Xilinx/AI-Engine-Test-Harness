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

Step by Step
=============

We provide 4 cases from Vitis-Tutorials to demonstrate how to use test harness to with AI Engine. We will take ``examples/super-sampling-rate-fir/SingleKernel`` as an example.

1. Adopt correct PLIO
----------------------

This case uses 1x input_plio and 1x output_plio. We pick "Column_12_TO_AIE" to send data to AI Engine and "Column_28_FROM_AIE" to receive data from AI Engine. "Column_XX" in PLIO names is actually the shim tile coordinate we used for certain PLIO. In case your design failed during mapping, please try other PLIO ports. All valid PLIO names can be found from ``include/vck190_test_harness_port_name.hpp``.

.. code-block:: c++

    TopGraph() {
        input_plio plin =
            input_plio::create(vck190_test_harness::in_names[0], plio_128_bits, "data/PhaseIn_0.txt", 250);
        output_plio plout =
            output_plio::create(vck190_test_harness::out_names[0], plio_128_bits, "data/Output_0.txt", 250);
        connect<>(plin.out[0], G1.in);
        connect<>(G1.out, plout.in[0]);
    }

2. Occupy unused PLIO
----------------------

Because this case does not used all PLIOs, we need to put a dummy graph to occupy all unused PLIOs.
``occupyUnusedPLIO`` is a template design. We need to provide number of used input_plio and output_plio, and names of used input_plio and output_plio.

.. code-block:: c++

    static std::vector<std::string> cust_in = {"Column_12_TO_AIE"};
    static std::vector<std::string> cust_out = {"Column_28_FROM_AIE"};
    TopGraph G;
    vck190_test_harness::occupyUnusedPLIO<1, 1> dummyGraph(cust_in, cust_out);

3. Create PS application
-------------------------

To drive test harness and AI Engine application from PS side, we need to instantiate one instance of ``vck190_test_harness::test_harness_mgr``.
We want to load ``in_sz`` bytes of data to channel ``Column_12_TO_AIE`` and receive ``out_sz`` bytes of data from channel ``Column_28_FROM_AIE``.
We know that each iteration of graph "G" can consume ``in_sz`` bytes of data and generate ``out_sz`` bytes of data.
To support multiple run of graph "G", we set the repetition count of ``test_harness_arg`` to 4, same as total iteration of graph run.
In order to measure total latency of each channels, we strongly recommend to call ``runGraph`` before ``runTestHarness``.

.. code-block:: c++

    test_harness_mgr mgr(0, argv[1], {"G"});
    std::vector<test_harness_args> args;
    args.push_back({channel_index(Column_12_TO_AIE), in_sz, 4, 0, (char*)in_data[0]});
    args.push_back({channel_index(Column_28_FROM_AIE), out_sz, 4, 0, (char*)out_data[0]});
    mgr.runGraph(0, 4);
    mgr.runTestHarness(args);
    mgr.waitForRes(10000);

4. Run software emulation
--------------------------

After we got libadf.a (x86sim targeted) and PS executable, we can run software emualation for quick verification.

.. code-block:: shell

    ${TEST_HARNESS_REPO_PATH}/test_harness/package_sw_emu.sh ${PKG_DIR} ${AIE_EXE} ${HOST_EXE}
    LD_LIBRARY_PATH=$(LIBRARY_PATH):$$LD_LIBRARY_PATH XCL_EMULATION_MODE=sw_emu $(PKG_DIR)/host_elf $(PKG_DIR)/vck190_test_harness.xclbin

5. Package SD card image
-------------------------

After we got libadf.a (hw targeted), PS executable and vck190_test_harness.xsa, we can pacakge SD card image for on-board running.

.. code-block:: shell

    ${TEST_HARNESS_REPO_PATH}/test_harness/package_hw.sh ${PKG_DIR} ${AIE_EXE} ${HOST_EXE}
