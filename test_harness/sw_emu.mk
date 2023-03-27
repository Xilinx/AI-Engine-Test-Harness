ifneq ($(findstring 2023.1, $(XILINX_VITIS)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202310_1/xilinx_vck190_base_dfx_202310_1.xpfm
endif

ifneq ($(findstring 2022.2, $(XILINX_VITIS)), )
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202220_1/xilinx_vck190_base_dfx_202220_1.xpfm
endif

xsa:
	v++ -c -t sw_emu --platform ${TEST_HARNESS_PLATFORM} -I ./ -I ${TEST_HARNESS_REPO_PATH}/include -k vck190_test_harness --hls.clock 250000000:vck190_test_harness ${TEST_HARNESS_REPO_PATH}/src/pl/test_harness.cpp -o ${BUILD_DIR}/vck190_test_harness.xo
	v++ -l -t sw_emu --platform ${TEST_HARNESS_PLATFORM} --config ${TEST_HARNESS_REPO_PATH}/cfg/system.cfg ${BUILD_DIR}/vck190_test_harness.xo ${AIE_EXE} -o ${BUILD_DIR}/vck190_test_harness.xsa
	v++ -p -t sw_emu -o ${BUILD_DIR}/vck190_test_harness.xclbin ${BUILD_DIR}/vck190_test_harness.xsa --package.defer_aie_run --platform ${TEST_HARNESS_PLATFORM} --package.out_dir ${BUILD_DIR} ${AIE_EXE}
	emconfigutil --platform ${TEST_HARNESS_PLATFORM} --od ${BUILD_DIR}
