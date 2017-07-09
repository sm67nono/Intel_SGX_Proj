#!/bin/bash

echo '1. Refershing Java Files'
rm 'Client.class'
rm 'EncryptionUtil.class'


javac Client.java
java Client

