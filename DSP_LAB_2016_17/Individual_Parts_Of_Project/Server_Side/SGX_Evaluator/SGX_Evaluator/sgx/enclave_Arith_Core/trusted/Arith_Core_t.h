#ifndef ARITH_CORE_T_H__
#define ARITH_CORE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif


int ecall_Arith_Core_sample();
int ecall_Arith_Core(char* x, char* result, int buffer_size);

sgx_status_t SGX_CDECL ocall_Arith_Core_sample(const char* str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
