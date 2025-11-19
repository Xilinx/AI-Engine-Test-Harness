# MIT License
#
# Copyright (C) 2023-2024 Advanced Micro Devices, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 
# Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.
#
# Parse and validate board type argument
if [ $# -lt 1 ]; then
        echo "Usage: $0 <vek385|vrk160>"
        exit 1
fi

board_type="$1"
if [ "$board_type" != "vek385" ] && [ "$board_type" != "vrk160" ]; then
        echo "Invalid board type: $board_type. Must be 'vek385' or 'vrk160'."
        exit 1
fi

# Get the directory of the currently running script
script_dir=$(dirname "$(readlink -f "$0")")
export SERVER_ROOT=$script_dir

# check if the environment variable XCL_EMULATION_MODE is defined
if [ -z "$XCL_EMULATION_MODE" ]; then
        export LD_LIBRARY_PATH=/mnt:$script_dir:/tmp:$LD_LIBRARY_PATH
        export XILINX_VITIS=/mnt
        export XILINX_XRT=/usr
        
# else, emulation, check if XILINX_XRT is defined, if not print error message
elif [ -z "$XILINX_XRT" ]; then
        echo "XILINX_XRT is not defined. Please set the XILINX_XRT environment variable to the path of the XRT installation."
        exit 1
fi

if [ -f platform_desc.txt  ]; then
        cp platform_desc.txt /etc/xocl.txt
fi

echo "Removing overlays"
fpgautil -R

echo "Programming PDI and DTBO overlays"
fpgautil -b ${board_type}_test_harness.pdi -o ${board_type}_test_harness.dtbo

pwd=$PWD

exec_path=/home/amd-edf/exec_server/
port=8080
while true
do
        rm -rf $exec_path
        mkdir -p $exec_path
        cd $exec_path
        $pwd/test_harness_server $port 60 1 0 2>&1 | tee -a $pwd/server_log.txt
        echo "Server crashed, restarting in 30 seconds"
        sleep 30
done
