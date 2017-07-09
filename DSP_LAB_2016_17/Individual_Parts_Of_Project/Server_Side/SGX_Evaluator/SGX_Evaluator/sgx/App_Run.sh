#!/bin/bash

echo '1.Rebuilding the libraries'
make clean

make
echo 'Rebuild success.'

cp enclave_Arith_Core/libEnc_Eval.so .

echo '2.Compiling Java Application'

javac JavaApp.java

echo 'Compiled Successfully'

echo '3.Starting Java Server(for Testing)...'
java -Djava.library.path=. JavaApp