#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */
#include <string.h>

#include "calc.h"
#include "calc_t.h"  /* print_string */

#define MACLEN 16
#define HASHLEN 32
#define IVLEN 12

// not production software
const sgx_aes_gcm_128bit_key_t aes_key[16] = { 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b',
		'c', 'd', 'a', 'b', 'c', 'd' };
const uint8_t aes_iv[12] = { 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd' };

/*
 * printf:
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
void printf(const char *fmt, ...) {
	char buf[BUFSIZ] = { '\0' };
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, BUFSIZ, fmt, ap);
	va_end(ap);
	ocall_calc_sample(buf);
}

int ecall_add(int x, int y) {
	return x + y;
}

/**
 * Encrypt using AES GCM
 *
 * Size of returned string is equal to the size of the input string + size of MAC (MACLEN)
 *
 */
void aes_encrypt(char* ptxt, int ptxt_size, char* ctxt) {
	uint8_t macbuffer[MACLEN];

	sgx_status_t status = sgx_rijndael128GCM_encrypt(aes_key, ptxt, ptxt_size, ctxt, aes_iv, IVLEN,
			NULL, 0, &macbuffer);

	// append mac at the end of ciphertext
	memcpy(ctxt + ptxt_size, macbuffer, MACLEN);
}

/**
 * Decrypt using AES GCM
 *
 * Size of returned string is equal to the size of the input string - size of MAC (MACLEN)
 *
 */
void aes_decrypt(char* ctxt, int ctxt_size, char* ptxt) {

	// size of ciphertext minus the MAC
	ctxt_size -= MACLEN;

	// start position of MAC
	char* mac_pos = ctxt + ctxt_size;

	// separate MAC
	uint8_t macbuffer[MACLEN];
	memcpy(macbuffer, mac_pos, MACLEN);

	// decrypt
	sgx_status_t status = sgx_rijndael128GCM_decrypt(aes_key, ctxt, ctxt_size, ptxt, aes_iv, IVLEN,
			NULL, 0, (const sgx_aes_gcm_128bit_tag_t*) &macbuffer);
}

void ecall_encrypt(char* ptxt, int ptxt_size, char* ctxt, int ctxt_size) {
	aes_encrypt(ptxt, ptxt_size, ctxt);
}

void ecall_decrypt(char* ctxt, int ctxt_size, char* ptxt, int ptxt_size) {
	aes_decrypt(ctxt, ctxt_size, ptxt);
}

void ecall_add_enc(char* ctxt1, int ctxt1_size, char* ctxt2, int ctxt2_size, char* result,
		int result_size) {

	// decrypt value 1
	char* ptxt1 = (uint8_t*) malloc(ctxt1_size - MACLEN);
	aes_decrypt(ctxt1, ctxt1_size, ptxt1);

	// decrypt value 2
	char* ptxt2 = (uint8_t*) malloc(ctxt2_size - MACLEN);
	aes_decrypt(ctxt2, ctxt2_size, ptxt2);

	// cast to int and add the 2 numbers
	int a = atoi(ptxt1);
	// printf("a=%d\n", a);
	int b = atoi(ptxt2);
	// printf("b=%d\n", b);
	int r = a + b;

	// cast to string
	char resBuf[ctxt1_size - MACLEN];

	snprintf(resBuf, ctxt1_size, "%d", r);

	// encrypt result
	aes_encrypt(resBuf, ctxt1_size - MACLEN, result);

}
