# AIE Test Harness

The main purpose of this repository is to provide PL test harness that help AIE application designers to do quick build and on-board tests.

Vitis allows compiling an AIE graph using a precompiled .xsa as an input platform to skip v++ link stage and directly go to v++ package stage.
This will save the most time consuming part of compiling and generate the final .xclbin file for on-board tests.

This repository test harness design on PL to feed data to and fetch data from AIE, including:

1. Source file of test harness, which helps to do sw emulation and hw emulation.
2. Precompiled .xsa of test harness, which helps to do fast packaging.
3. Software APIs, which helps to drive test harness on PL and user application on AIE.

## Hardware Features

## Sotware Features

## How to Use

### Emulation flow

### On-board flow
