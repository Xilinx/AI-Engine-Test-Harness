export LD_LIBRARY_PATH=/mnt:/tmp:
export XILINX_VITIS=/mnt
export XILINX_XRT=/usr
if [ -f platform_desc.txt  ]; then
        cp platform_desc.txt /etc/xocl.txt
fi
./host_elf vck190_test_harness.xclbin
return_code=$?
if [ $return_code -ne 0 ]; then
        echo "ERROR: TEST FAILED, RC=$return_code"
else
        echo "INFO: TEST PASSED, RC=0"
fi
echo "INFO: Embedded host run completed."
exit $return_code
