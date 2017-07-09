#!/bin/bash

##
## Savvas Savvides <savvas@purdue.edu>
## Jan/2017
##

# Build java program and generate jni header file
javac JavaSGX.java
javah JavaSGX
mv JavaSGX.h sgx/enclave_calc/untrusted/

make -f sgx/Makefile
