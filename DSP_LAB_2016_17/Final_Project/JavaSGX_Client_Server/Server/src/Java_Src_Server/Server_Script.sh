#!/bin/bash
echo 'Setting Environment variables'
export LD_LIBRARY_PATH='/opt/intel/sgxsdk/sdk_libs/'
echo '1. Rebuilding Libraries'

echo 'Clean Build SGX_Arith_Core'

make clean -C '../JNI_Bindings_C_Code/Server_SGXArith_Core/sgx'
make -C '../JNI_Bindings_C_Code/Server_SGXArith_Core/sgx'

make clean -C '../JNI_Bindings_C_Code/Remote_Attestation/'
make SGX_MODE=SIM -C '../JNI_Bindings_C_Code/Remote_Attestation/'


echo '2. Removing old Library Files in current directory'
rm 'libisv_app.so'
rm 'libEnc_Eval.so'
rm 'Arith_Core.signed.so'
rm 'isv_enclave.signed.so'


echo '2. Adding Fresh Libraries'

cp '../JNI_Bindings_C_Code/Server_SGXArith_Core/sgx/enclave_Arith_Core/libEnc_Eval.so' .
cp '../JNI_Bindings_C_Code/Server_SGXArith_Core/sgx/enclave_Arith_Core/Arith_Core.signed.so' .
cp '../JNI_Bindings_C_Code/Remote_Attestation/libisv_app.so' .
cp '../JNI_Bindings_C_Code/Remote_Attestation/isv_enclave.signed.so' .

echo '3. Refershing Java Files'
rm 'Server.class'
rm 'Java_Main.class'
rm 'JavaSGX.class'
rm 'EncryptionUtil.class'


javac Server.java
java -Djava.library.path=. Server

