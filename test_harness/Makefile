TARGET ?= hw
ifneq ($(filter aiesim hw_emu hw, $(TARGET)),)
AIETARGET := hw
else
AIETARGET := x86sim
endif
HOST_ARCH ?= aarch64

BUILD_DIR := ./tmp
REL_DIR := ./bin
TEST_HARNESS_PLATFORM := ${XILINX_VITIS}/base_platforms/xilinx_vck190_base_dfx_202220_1/xilinx_vck190_base_dfx_202220_1.xpfm

.PHONY: xsa clean

xsa:
	mkdir -p ${BUILD_DIR}
	mkdir -p ${REL_DIR}
	aiecompiler --target ${AIETARGET} -I ./ -I ../include -I ../src/aie -I ${XILINX_VITIS}/aietools/include --constraints=./aie_constraints.json --event-trace=runtime --event-trace-port=gmio --workdir=./Work ./aie_graph.cpp
	mv libadf.a ${BUILD_DIR}/
	v++ -c -t ${TARGET} --platform ${TEST_HARNESS_PLATFORM} -I ./ -I ../include -k vck190_test_harness --hls.clock 250000000:vck190_test_harness ../src/pl/test_harness.cpp -o ${BUILD_DIR}/vck190_test_harness.xo
	v++ -l -t ${TARGET} --platform ${TEST_HARNESS_PLATFORM} --config ./system.cfg ${BUILD_DIR}/vck190_test_harness.xo ${BUILD_DIR}/libadf.a -o ${BUILD_DIR}/vck190_test_harness.xsa
	cp ${BUILD_DIR}/vck190_test_harness.xsa ${REL_DIR}

clean:
	rm -rf ${BUILD_DIR}
	rm -rf ${REL_DIR}
	rm -rf AIECompiler.log Map_Report.csv ./Work _x sol.db v++*log xcd.log _x .Xil temp
