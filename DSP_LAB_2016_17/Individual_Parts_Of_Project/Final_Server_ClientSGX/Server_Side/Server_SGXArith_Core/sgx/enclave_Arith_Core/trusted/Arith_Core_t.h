#ifndef ARITH_CORE_T_H__
#define ARITH_CORE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */

#include "sgx_tcrypto.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif


int ecall_Arith_Core_sample();
int ecall_Arith_Core(char* x, char* result, int buffer_size);
int ecall_add(int x, int y);
void ecall_encrypt(char* ptxt, int ptxt_size, char* ctxt, int ctxt_size);
void ecall_decrypt(char* ctxt, int ctxt_size, char* ptxt, int ptxt_size);
void ecall_add_enc(char* ctxt1, int size1, char* ctxt2, int size2, char* buffer, int buffer_size);

sgx_status_t SGX_CDECL ocall_Arith_Core_sample(const char* str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
