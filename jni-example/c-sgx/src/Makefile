##
## author: savvas@purdue.edu
## date: Nov/05/2016
## 
## Makefile to compile the libary containing the native functions
## called by java.
##
## Supports Linux and OSX
##

UNAME := $(shell uname)

# name of the library to create
ifeq ($(UNAME), Linux)
	LIB_NAME=libsgx.so
else
	LIB_NAME=libsgx.jnilib
endif

# location of java class that includes native calls
JAVA_BIN=../../java-sgx/bin

# java class that includes native methods
CLASS_NAME=edu.purdue.main.JniTest

CPP=g++

ifeq ($(UNAME), Linux)
	CFLAGS=-shared
else
	CFLAGS=-dynamiclib
endif

# Location of jni.h
ifeq ($(UNAME), Linux)
	JVM_JNI_PATH=-I/usr/lib/jvm/java-8-oracle/include -I/usr/lib/jvm/java-8-oracle/include/linux
else
	JVM_JNI_PATH=-I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers/
endif

## ===========================

all : $(LIB_NAME)

$(LIB_NAME) : JniTest.o
	$(CPP) $(CFLAGS) -o $@ JniTest.o
   	
JniTest.o : JniTest.c jniHeader
	$(CPP) $(JVM_JNI_PATH) -fPIC -c JniTest.c
	
# create jni header
jniHeader :
	javah -classpath $(JAVA_BIN) $(CLASS_NAME)
	
clean :
	rm -f *.o edu_purdue_main_JniTest.h $(LIB_NAME)