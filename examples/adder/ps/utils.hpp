/*
 * MIT License
 *
 * Copyright (C) 2023 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the “Software”), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or
 * otherwise to promote the sale, use or other dealings in this Software without prior written authorization from
 * Advanced Micro Devices, Inc.
 */

#pragma once

#include <vector>
#include <complex>


template<typename T>
size_t num_bytes(const std::vector<T>& vec)
{
    return sizeof(T) * vec.size();
}

template<typename T>
void check_size(std::string name, const std::vector<T>& vec)
{
    if (num_bytes(vec) / 1024 > 128 ) {
        printf("Error: size of %s (%dKB) exceeds the capacity of test harness buffers (128KB).\n", name.c_str(), num_bytes(vec) / 1024);
        exit(-1);
    }
}

template<typename T>
void read_data_from_file(const std::string file_name, std::vector<T> &vec)
{
    std::ifstream input_file(file_name,std::ifstream::in);
    if (!input_file.is_open()) {
        printf("Failed to open data file %s for reading\n", file_name.c_str());
        throw std::runtime_error("Failed to open data file.\n");
    }

    T val;
    while (input_file >> val) {
    	vec.push_back(val);
    }

    input_file.close();    
}

template<typename T>
void read_data_from_file(const std::string file_name, std::vector<std::complex<T>> &vec)
{
    std::ifstream input_file(file_name,std::ifstream::in);
    if (!input_file.is_open()) {
        printf("Failed to open data file %s for reading\n", file_name.c_str());
        throw std::runtime_error("Failed to open data file.\n");
    }

    T val;
    std::complex<T> cpx;
    while (input_file >> val) {
    	cpx.real(val);
	    if (input_file >> val) {
	    	cpx.imag(val);
    	} else {
	    	cpx.imag(0);
    	}
   		vec.push_back(cpx);
    }

    input_file.close();    
}
