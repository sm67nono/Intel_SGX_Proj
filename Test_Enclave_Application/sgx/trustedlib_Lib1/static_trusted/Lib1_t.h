#ifndef LIB1_T_H__
#define LIB1_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif


int ecall_Lib1_sample();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
