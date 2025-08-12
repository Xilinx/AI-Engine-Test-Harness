# Usage of the Test Harness

## Build Essential Libraries
In order to drive the test harness from Matlab and Python, users need to build the libraries on the local machines with the following command lines. 

### Build the Python library
```
# setup the python
pip install -r requirements.txt
make python
```


### Build the Matlab library
```
# setup the matlab environment
make matlab
```


## Build the Test Harness XSAs and Images

The xsa and the server sd card images on each supported platform are provided by AMD. Those pre-built files satisty most use scenarios. Please follow this documents if you need to build the xsa and/or the sd card images from the scratch to meet your customized requirements. 

### Download the XSA and the Test Harness Images
```
cd ${TEST_HARNESS_REPO}/bin
./download.sh
```

### Build the Test Harness XSA
The following command lines build the xsa on the given device.
```
# setup the vitis environment
cd ${TEST_HARNESS_REPO}/test_harness
make xsa DEVICE=vek280 TARGET=hw  # build xsa on vek280
```

### Build the SD card images for the test harness server
The following command lines build the server SD card image on the given device.
```
# setup the vitis and the xrt
cd ${TEST_HARNESS_REPO}/test_harness
make sd_card DEVICE=vek280 TARGET=hw  # build the sd card images for vek280
```

### Supported Devices

| Device   | XSA Build Command                      | SD Card Image Command                     |
|----------|----------------------------------------|-------------------------------------------|
| vek280   | `make xsa DEVICE=vek280 TARGET=hw`     | `make sd_card DEVICE=vek280 TARGET=hw`    |
| vck190   | `make xsa DEVICE=vck190 TARGET=hw`     | `make sd_card DEVICE=vck190 TARGET=hw`    |
| vrk160   | `make xsa DEVICE=vrk160 TARGET=hw`     | `make sd_card DEVICE=vrk160 TARGET=hw`    |
| vek385   | `make xsa DEVICE=vek385 TARGET=hw`     | `make sd_card DEVICE=vek385 TARGET=hw`    |
