Running a sample enclave from Java
==================================

A) set "LIBENCLAVE_PATH" env variable pointing to your jni library e.g. export LIBENCLAVE_PATH=/home/savvas/javaSGX/sgx/enclave_calc/libsgx.so

B) run build-java-sgx.sh

C) run java JavaSGX as a regular java application


In particular consider what the method "testAddEnc()" in JavaSGX.java does. It sends two encrypted values to the enclave (via JNI) and gets back the encrypted sum. 
This should work as a good baseline. It demonstrates how to send and receive "stuff" to/from an enclave and how encryption works.

For any issues contact me at: savvas@purdue.edu

