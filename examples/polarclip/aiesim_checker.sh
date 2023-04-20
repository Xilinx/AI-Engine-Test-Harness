#!/bin/bash
# MIT License
#
# Copyright (C) 2023 Advanced Micro Devices, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 
# Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.
#

set -e

serialize_and_diff()
{
	tmp1a=$(mktemp -u --suffix=.in1.txt)
	tmp1b=$(mktemp -u --suffix=.in1.txt)
	tmp2a=$(mktemp -u --suffix=.in2.txt)	
	tmp2b=$(mktemp -u --suffix=.in2.txt)
	# Remove all lines starting with T
	sed '/T/d' $1 > $tmp1a
	sed '/T/d' $2 > $tmp2a
	# Joins all lines of the specified file to form an out with a single, long line
	sed ':a; N; $!ba; s/\n//g' $tmp1a > $tmp1b
	sed ':a; N; $!ba; s/\n//g' $tmp2a > $tmp2b
	# Compare the two files ignoring any white spaces
	diff $tmp1b $tmp2b -wZqs	
}

echo ""
serialize_and_diff ./aiesimulator_output/data/output.txt ./data/golden_4iters.txt
echo ""
