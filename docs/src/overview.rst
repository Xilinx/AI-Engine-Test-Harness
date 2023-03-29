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

Overview
=========

This repository provides a test harness to help AI Engine developers easily build and test their application on board. With minor modifications and a few minutes to build, developers can turn their AIE simulation project into SD card image for hardware run. The test harness consists of three major components: 
 

* Test harness on PL, including 16 output channels to send data to AI Engine and 16 input channels, to load data from AI Engine. We provided a pre-built implementation of test harness (A .xsa file which needs to be downloaded from Xilinx) to save the trouble to build. 
 
* A set of software APIs to initialize device, setup configs to drive test harness channels and run tests. 

* Scripts to help package developer's application on AI Engine (libadf.a) and test application on PS with pre-built test harness and generate the boot image for hardware run. 

License
========

Licensed using the `Apache 2.0 license <https://www.apache.org/licenses/LICENSE-2.0>`_.

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

Trademark Notice
=================

    Xilinx, the Xilinx logo, Artix, ISE, Kintex, Spartan, Virtex, Zynq, and
    other designated brands included herein are trademarks of Xilinx in the
    United States and other countries.
    
    All other trademarks are the property of their respective owners.


