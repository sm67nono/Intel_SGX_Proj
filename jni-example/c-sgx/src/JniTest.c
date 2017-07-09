/**
 *
 * Functions accessed via jni from java program
 *
 * author: Savvas Savvides <savvas@purdue.edu>
 * date: Nov/05/2016
 *
 */

#include <stdio.h>

// the automatically generated header via the javah command
#include "edu_purdue_main_JniTest.h"

JNIEXPORT void JNICALL Java_edu_purdue_main_JniTest_hello(JNIEnv *env, jobject obj) {
	printf("Hello from JNI!\n");
}

JNIEXPORT void JNICALL Java_edu_purdue_main_JniTest_helloArguments(JNIEnv *env, jobject obj, jstring jname, jint times) {

	// convert from jstring to char*
	const char *name = (*env).GetStringUTFChars(jname, 0);

	for (jint i = 0; i < times; i++) {
		printf("Hello %s\n", name);

	}
}

JNIEXPORT jint JNICALL Java_edu_purdue_main_JniTest_helloArgumentsReturn(JNIEnv *env, jobject obj,
		jint x, jint y) {
	return x + y;
}
