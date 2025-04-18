if [ -z "$SERVER_IP_PORT"  ]; then
    export SERVER_IP_PORT=127.0.0.1:8080
fi
python host.py vck190_test_harness.xclbin --iterations 4 --function --performance
