#include <string.h>
#include <assert.h>

#include <time.h>

#include <unistd.h>
#include <pwd.h>
#include <libgen.h>
#include <stdlib.h>

# define MAX_PATH FILENAME_MAX

#include <sgx_urts.h>
#include "sample.h"

#include "calc_u.h"

#include "JavaSGX.h"

#define MACLEN 16

#define BILLION 1000000000L

/*
 double timeMillis() {
 struct timeval tv;
 gettimeofday(&tv, NULL);
 return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
 }
 */

// call this function to start a nanosecond-resolution timer
struct timespec timer_start() {
	struct timespec start_time;
	clock_gettime(CLOCK_MONOTONIC, &start_time);
	return start_time;
}

// call this function to end a timer, returning nanoseconds elapsed as a long
long timer_end(struct timespec start_time) {
	struct timespec end_time;
	clock_gettime(CLOCK_MONOTONIC, &end_time);
	long diffInNanos = BILLION * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec
			- start_time.tv_nsec;
	return diffInNanos;
}

void INIT_ENCLAVE();
void INIT_ENCLAVE(char*);
void DESTROY_ENCLAVE();

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
	sgx_status_t err;
	const char *msg;
	const char *sug; /* Suggestion */
} sgx_errlist_t;

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] =
		{ { SGX_ERROR_UNEXPECTED, "Unexpected error occurred.", NULL }, {
				SGX_ERROR_INVALID_PARAMETER, "Invalid parameter.", NULL }, {
				SGX_ERROR_OUT_OF_MEMORY, "Out of memory.", NULL }, { SGX_ERROR_ENCLAVE_LOST,
				"Power transition occurred.",
				"Please refer to the sample \"PowerTransition\" for details." }, {
				SGX_ERROR_INVALID_ENCLAVE, "Invalid enclave image.", NULL }, {
				SGX_ERROR_INVALID_ENCLAVE_ID, "Invalid enclave identification.", NULL }, {
				SGX_ERROR_INVALID_SIGNATURE, "Invalid enclave signature.", NULL }, {
				SGX_ERROR_OUT_OF_EPC, "Out of EPC memory.", NULL },
				{ SGX_ERROR_NO_DEVICE, "Invalid SGX device.",
						"Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards." },
				{ SGX_ERROR_MEMORY_MAP_CONFLICT, "Memory map conflicted.", NULL }, {
						SGX_ERROR_INVALID_METADATA, "Invalid enclave metadata.", NULL }, {
						SGX_ERROR_DEVICE_BUSY, "SGX device was busy.", NULL }, {
						SGX_ERROR_INVALID_VERSION, "Enclave version was invalid.", NULL }, {
						SGX_ERROR_INVALID_ATTRIBUTE, "Enclave was not authorized.", NULL }, {
						SGX_ERROR_ENCLAVE_FILE_ACCESS, "Can't open enclave file.", NULL }, };

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret) {
	size_t idx = 0;
	size_t ttl = sizeof sgx_errlist / sizeof sgx_errlist[0];

	for (idx = 0; idx < ttl; idx++) {
		if (ret == sgx_errlist[idx].err) {
			if (NULL != sgx_errlist[idx].sug)
				printf("Info: %s\n", sgx_errlist[idx].sug);
			printf("Error: %s\n", sgx_errlist[idx].msg);
			break;
		}
	}

	if (idx == ttl)
		printf("Error: Unexpected error occurred.\n");
}

/* Initialize the enclave:
 *   Step 1: retrieve the launch token saved by last transaction
 *   Step 2: call sgx_create_enclave to initialize an enclave instance
 *   Step 3: save the launch token if it is updated
 */
int initialize_enclave(void) {
	char token_path[MAX_PATH] = { '\0' };
	sgx_launch_token_t token = { 0 };
	sgx_status_t ret = SGX_ERROR_UNEXPECTED;
	int updated = 0;
	/* Step 1: retrieve the launch token saved by last transaction */

	/* try to get the token saved in $HOME */
	const char *home_dir = getpwuid(getuid())->pw_dir;
	if (home_dir
			!= NULL&& (strlen(home_dir) + strlen("/") + sizeof(TOKEN_FILENAME) + 1) <= MAX_PATH) {
		/* compose the token path */
		strncpy(token_path, home_dir, strlen(home_dir));
		strncat(token_path, "/", strlen("/"));
		strncat(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME) + 1);
	} else {
		/* if token path is too long or $HOME is NULL */
		strncpy(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME));
	}

	FILE *fp = fopen(token_path, "rb");
	if (fp == NULL && (fp = fopen(token_path, "wb")) == NULL) {
		printf("Warning: Failed to create/open the launch token file \"%s\".\n", token_path);
	}
	printf("token_path: %s\n", token_path);
	if (fp != NULL) {
		/* read the token from saved file */
		size_t read_num = fread(token, 1, sizeof(sgx_launch_token_t), fp);
		if (read_num != 0 && read_num != sizeof(sgx_launch_token_t)) {
			/* if token is invalid, clear the buffer */
			memset(&token, 0x0, sizeof(sgx_launch_token_t));
			printf("Warning: Invalid launch token read from \"%s\".\n", token_path);
		}
	}

	/* Step 2: call sgx_create_enclave to initialize an enclave instance */
	/* Debug Support: set 2nd parameter to 1 */

	ret = sgx_create_enclave(CALC_FILENAME, SGX_DEBUG_FLAG, &token, &updated, &global_eid, NULL);

	if (ret != SGX_SUCCESS) {
		print_error_message(ret);

		printf("RETURN: %d\".\n", ret);

		if (fp != NULL)
			fclose(fp);

		return -1;
	}

	/* Step 3: save the launch token if it is updated */

	if (updated == FALSE || fp == NULL) {
		/* if the token is not updated, or file handler is invalid, do not perform saving */
		if (fp != NULL)
			fclose(fp);
		return 0;
	}

	/* reopen the file with write capablity */
	fp = freopen(token_path, "wb", fp);
	if (fp == NULL)
		return 0;
	size_t write_num = fwrite(token, 1, sizeof(sgx_launch_token_t), fp);
	if (write_num != sizeof(sgx_launch_token_t))
		printf("Warning: Failed to save launch token to \"%s\".\n", token_path);
	fclose(fp);

	return 0;
}

/* OCall functions */
void ocall_calc_sample(const char *str) {
	/* Proxy/Bridge will check the length and null-terminate
	 * the input string to prevent buffer overflow.
	 */
	printf("%s", str);
}

JNIEXPORT void JNICALL Java_JavaSGX_sgx_1init(JNIEnv * env, jobject obj) {
	// SS: FIXME: hardcoded. This is to simulate arg[0] (see main method)
	//char *path = (char*) malloc(1024);
	// strcpy(path, "/home/savvas/programming/github/Hydra/SGX/javaSGX/sgx/enclave_calc/sample");
	// strcpy(path, "/home/ubuntu/programming/github/Hydra/SGX/javaSGX/sgx/enclave_calc/sample");

	INIT_ENCLAVE();
}

JNIEXPORT jbyteArray JNICALL Java_JavaSGX_sgx_1enc(JNIEnv *env, jobject obj, jstring ptxt) {

	sgx_status_t ret = SGX_ERROR_UNEXPECTED;

	// convert java string to char*
	char* ptxt_char = const_cast<char*>((*env).GetStringUTFChars(ptxt, 0));
	size_t ptxt_size = (size_t)((*env).GetStringLength(ptxt));

	int ctxt_size = ptxt_size + MACLEN;
	char ctxt[ctxt_size];
	ret = ecall_encrypt(global_eid, ptxt_char, ptxt_size, ctxt, ctxt_size);

	if (ret != SGX_SUCCESS)
		abort();

	// convert to java byte array
	jbyteArray r = (*env).NewByteArray(ctxt_size);
	(*env).SetByteArrayRegion(r, 0, ctxt_size, (jbyte*) ctxt);

	return r;
}

JNIEXPORT jstring JNICALL Java_JavaSGX_sgx_1dec(JNIEnv *env, jobject obj, jbyteArray ctxt) {

	sgx_status_t ret = SGX_ERROR_UNEXPECTED;

	jbyte* ctxt_char = (*env).GetByteArrayElements(ctxt, NULL);
	jsize ctxt_size = (*env).GetArrayLength(ctxt);

	int ptxt_size = ctxt_size - MACLEN;
	char ptxt[ptxt_size];

	// NOTE: requires ptxt_size + 1 to allow for \0
	ret = ecall_decrypt(global_eid, (char*) ctxt_char, ctxt_size, ptxt, ptxt_size + 1);

	if (ret != SGX_SUCCESS)
		abort();

	// NOTE: this requires ptxt_size + 1 but much faster (~150x) than method below.
	return (*env).NewStringUTF(ptxt);

	/*
	 // This is much slower than  NewStringUTF

	 jbyteArray r = (*env).NewByteArray(ptxt_size);
	 (*env).SetByteArrayRegion(r, 0, ptxt_size, (jbyte*) ptxt);

	 jclass strClass = (*env).FindClass("java/lang/String");
	 jmethodID ctorID = (*env).GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
	 jstring encoding = (*env).NewStringUTF("GBK");

	 jstring str = (jstring)(*env).NewObject(strClass, ctorID, r, encoding);
	 */
}

JNIEXPORT jint JNICALL Java_JavaSGX_sgx_1add(JNIEnv *env, jobject obj, jint x, jint y) {

	sgx_status_t ret = SGX_ERROR_UNEXPECTED;

	int r;
	ret = ecall_add(global_eid, &r, x, y);

	return r;
}

JNIEXPORT jbyteArray JNICALL Java_JavaSGX_sgx_1add_1enc(JNIEnv *env, jobject obj, jbyteArray xAr,
		jbyteArray yAr) {

	// convert byte arrays to jbyte* and get their lengths
	jbyte* x = (*env).GetByteArrayElements(xAr, NULL);
	jsize xSize = (*env).GetArrayLength(xAr);
	jbyte* y = (*env).GetByteArrayElements(yAr, NULL);
	jsize ySize = (*env).GetArrayLength(yAr);

	sgx_status_t ret = SGX_ERROR_UNEXPECTED;

	char buffer[xSize - MACLEN];

	// FIXME: assumes output size same as input size
	ret = ecall_add_enc(global_eid, (char*) x, xSize, (char*) y, ySize, buffer, xSize);

	// convert to byte array
	// FIXME: assumes output size same as input size
	jbyteArray r = (*env).NewByteArray(xSize);
	(*env).SetByteArrayRegion(r, 0, xSize, (jbyte*) buffer);

	return r;
}

JNIEXPORT void JNICALL Java_JavaSGX_sgx_1destroy(JNIEnv * env, jobject obj) {
	DESTROY_ENCLAVE();
}

/*
 * SS: INIT_ENCLAVE
 * For when the method is called indirectly i.e via jni
 */
void INIT_ENCLAVE() {
	char* path = getenv("LIBENCLAVE_PATH");

	if (!path) {
		printf("LIBENCLAVE_PATH environment variable not set.\n");
		abort();
	}

	INIT_ENCLAVE(path);
}

/*
 * SS: INIT_ENCLAVE
 *
 */
void INIT_ENCLAVE(char* path) {

	// dirname changes the input path so use a copy instead
	char* pathcopy = strdup(path);

	// Changing dir to where the executable is.
	char absolutePath[MAX_PATH];
	char *ptr = NULL;

	ptr = realpath(dirname(pathcopy), absolutePath);

	if (chdir(absolutePath) != 0)
		abort();

	// Initialize the enclave
	if (initialize_enclave() < 0) {
		printf("initialize_enclave() FAILED\n");
		abort();
	}
}

/* SS: DESTROY_ENCLAVE
 *
 */
void DESTROY_ENCLAVE() {
	sgx_destroy_enclave(global_eid);
}

/**
 * Measures the time to perform an integer addition inside and outside an enclave with no
 * encryption involved.
 *
 */
void test_add() {
	struct timespec start;
	long time_elapsed_nanos;

	sgx_status_t ret = SGX_ERROR_UNEXPECTED;

	// addition outside enclave
	int x = 4;
	int y = 3;
	start = timer_start();
	int r = x + y;
	time_elapsed_nanos = timer_end(start);
	printf("Plaintext Time (nanoseconds): %ld\n", time_elapsed_nanos);

	// addition inside enclave
	start = timer_start();
	int sum;
	ret = ecall_add(global_eid, &sum, x, y);
	time_elapsed_nanos = timer_end(start);
	printf("Enclave Time (nanoseconds): %ld\n", time_elapsed_nanos);
	printf("Result: %d\n", sum);
}

/**
 * Measures the time to perform an integer addition inside and outside an enclave with no
 * encryption involved.
 *
 */
void test_enc_dec() {
	struct timespec start;
	long time_elapsed_nanos;

	int ptxt_size = 4;
	int ctxt_size = ptxt_size + MACLEN;
	char ptxt[ptxt_size];
	strcpy(ptxt, "222");

	// ENCRYPT
	printf("Encrypting %s\n", ptxt);
	char ctxt[ctxt_size];
	start = timer_start();
	sgx_status_t ret = ecall_encrypt(global_eid, ptxt, ptxt_size, ctxt, ctxt_size);
	time_elapsed_nanos = timer_end(start);

	printf("CTXT: %s\n", ctxt);
	printf("Encr Time (nanoseconds): %ld\n", time_elapsed_nanos);

	// DECRYPT
	char *decr = (char*) malloc(ptxt_size);
	start = timer_start();
	ret = ecall_decrypt(global_eid, ctxt, ctxt_size, decr, ptxt_size);
	time_elapsed_nanos = timer_end(start);

	printf("DECR: %s\n", decr);
	printf("Decr Time (nanoseconds): %ld\n", time_elapsed_nanos);
}

/**
 * Measures the time to perform an integer addition inside and outside an enclave with no
 * encryption involved.
 *
 */
void test_enc_add() {
	struct timespec start;
	long time_elapsed_nanos;

	sgx_status_t ret;

	int ptxt_size = 4;
	int ctxt_size = ptxt_size + MACLEN;

	char ptxt1[ptxt_size];
	strcpy(ptxt1, "123");

	char ptxt2[ptxt_size];
	strcpy(ptxt2, "222");

	// ENCRYPT

	printf("Encrypting: %s\n", ptxt1);
	char ctxt1[ctxt_size];
	ret = ecall_encrypt(global_eid, ptxt1, ptxt_size, ctxt1, ctxt_size);

	printf("Encrypting: %s\n", ptxt2);
	char ctxt2[ctxt_size];
	ret = ecall_encrypt(global_eid, ptxt2, ptxt_size, ctxt2, ctxt_size);
	printf("CTXT1: %s\n", ctxt1);
	printf("CTXT2: %s\n", ctxt2);

	char res[ctxt_size];

	printf("Adding two values in enclave...\n");
	start = timer_start();
	ret = ecall_add_enc(global_eid, ctxt1, ctxt_size, ctxt2, ctxt_size, res, ctxt_size);
	time_elapsed_nanos = timer_end(start);

	printf("RES: %s\n", res);
	printf("Add Time (nanoseconds): %ld\n", time_elapsed_nanos);

	// DECRYPT
	char *decr = (char*) malloc(ptxt_size);
	ret = ecall_decrypt(global_eid, res, ctxt_size, decr, ptxt_size);

	printf("Decrypted result: %s\n", decr);
}

int SGX_CDECL main(int argc, char *argv[]) {
	struct timespec start;
	long time_elapsed_nanos;

	start = timer_start();
	INIT_ENCLAVE(argv[0]);
	time_elapsed_nanos = timer_end(start);
	printf("INIT enclave Time (nanoseconds): %ld\n", time_elapsed_nanos);

	test_enc_add();

	start = timer_start();
	DESTROY_ENCLAVE();
	time_elapsed_nanos = timer_end(start);
	printf("DESTROY enclave Time (nanoseconds): %ld\n", time_elapsed_nanos);

	printf("DONE!\n");
	return 0;
}
