if [ -z "$SERVER_IP_PORT"  ]; then
    export SERVER_IP_PORT=127.0.0.1:8080
fi
matlab -nodisplay -nosplash -nodesktop -r "host vck190_test_harness.xclbin"