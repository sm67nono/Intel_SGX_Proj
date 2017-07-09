#include "Arith_Core_t.h"

#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */

#include <errno.h>
#include <string.h> /* for memcpy etc */
#include <stdlib.h> /* for malloc/free etc */

#define CHECK_REF_POINTER(ptr, siz) do {	\
	if (!(ptr) || ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_UNIQUE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)


typedef struct ms_ecall_Arith_Core_sample_t {
	int ms_retval;
} ms_ecall_Arith_Core_sample_t;

typedef struct ms_ecall_Arith_Core_t {
	int ms_retval;
	char* ms_x;
	char* ms_result;
	int ms_buffer_size;
} ms_ecall_Arith_Core_t;

typedef struct ms_ocall_Arith_Core_sample_t {
	char* ms_str;
} ms_ocall_Arith_Core_sample_t;

static sgx_status_t SGX_CDECL sgx_ecall_Arith_Core_sample(void* pms)
{
	ms_ecall_Arith_Core_sample_t* ms = SGX_CAST(ms_ecall_Arith_Core_sample_t*, pms);
	sgx_status_t status = SGX_SUCCESS;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_Arith_Core_sample_t));

	ms->ms_retval = ecall_Arith_Core_sample();


	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_Arith_Core(void* pms)
{
	ms_ecall_Arith_Core_t* ms = SGX_CAST(ms_ecall_Arith_Core_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_x = ms->ms_x;
	size_t _len_x = _tmp_x ? strlen(_tmp_x) + 1 : 0;
	char* _in_x = NULL;
	char* _tmp_result = ms->ms_result;
	int _tmp_buffer_size = ms->ms_buffer_size;
	size_t _len_result = _tmp_buffer_size;
	char* _in_result = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_Arith_Core_t));
	CHECK_UNIQUE_POINTER(_tmp_x, _len_x);
	CHECK_UNIQUE_POINTER(_tmp_result, _len_result);

	if (_tmp_x != NULL) {
		_in_x = (char*)malloc(_len_x);
		if (_in_x == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_x, _tmp_x, _len_x);
		_in_x[_len_x - 1] = '\0';
	}
	if (_tmp_result != NULL) {
		if ((_in_result = (char*)malloc(_len_result)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_result, 0, _len_result);
	}
	ms->ms_retval = ecall_Arith_Core(_in_x, _in_result, _tmp_buffer_size);
err:
	if (_in_x) free(_in_x);
	if (_in_result) {
		memcpy(_tmp_result, _in_result, _len_result);
		free(_in_result);
	}

	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv;} ecall_table[2];
} g_ecall_table = {
	2,
	{
		{(void*)(uintptr_t)sgx_ecall_Arith_Core_sample, 0},
		{(void*)(uintptr_t)sgx_ecall_Arith_Core, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
	uint8_t entry_table[1][2];
} g_dyn_entry_table = {
	1,
	{
		{0, 0, },
	}
};


sgx_status_t SGX_CDECL ocall_Arith_Core_sample(const char* str)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_str = str ? strlen(str) + 1 : 0;

	ms_ocall_Arith_Core_sample_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_Arith_Core_sample_t);
	void *__tmp = NULL;

	ocalloc_size += (str != NULL && sgx_is_within_enclave(str, _len_str)) ? _len_str : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_Arith_Core_sample_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_Arith_Core_sample_t));

	if (str != NULL && sgx_is_within_enclave(str, _len_str)) {
		ms->ms_str = (char*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_str);
		memcpy((void*)ms->ms_str, str, _len_str);
	} else if (str == NULL) {
		ms->ms_str = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	status = sgx_ocall(0, ms);


	sgx_ocfree();
	return status;
}

