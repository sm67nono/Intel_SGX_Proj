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

typedef struct ms_ecall_add_t {
	int ms_retval;
	int ms_x;
	int ms_y;
} ms_ecall_add_t;

typedef struct ms_ecall_encrypt_t {
	char* ms_ptxt;
	int ms_ptxt_size;
	char* ms_ctxt;
	int ms_ctxt_size;
} ms_ecall_encrypt_t;

typedef struct ms_ecall_decrypt_t {
	char* ms_ctxt;
	int ms_ctxt_size;
	char* ms_ptxt;
	int ms_ptxt_size;
} ms_ecall_decrypt_t;

typedef struct ms_ecall_add_enc_t {
	char* ms_ctxt1;
	int ms_size1;
	char* ms_ctxt2;
	int ms_size2;
	char* ms_buffer;
	int ms_buffer_size;
} ms_ecall_add_enc_t;

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

static sgx_status_t SGX_CDECL sgx_ecall_add(void* pms)
{
	ms_ecall_add_t* ms = SGX_CAST(ms_ecall_add_t*, pms);
	sgx_status_t status = SGX_SUCCESS;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_add_t));

	ms->ms_retval = ecall_add(ms->ms_x, ms->ms_y);


	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_encrypt(void* pms)
{
	ms_ecall_encrypt_t* ms = SGX_CAST(ms_ecall_encrypt_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_ptxt = ms->ms_ptxt;
	int _tmp_ptxt_size = ms->ms_ptxt_size;
	size_t _len_ptxt = _tmp_ptxt_size;
	char* _in_ptxt = NULL;
	char* _tmp_ctxt = ms->ms_ctxt;
	int _tmp_ctxt_size = ms->ms_ctxt_size;
	size_t _len_ctxt = _tmp_ctxt_size;
	char* _in_ctxt = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_encrypt_t));
	CHECK_UNIQUE_POINTER(_tmp_ptxt, _len_ptxt);
	CHECK_UNIQUE_POINTER(_tmp_ctxt, _len_ctxt);

	if (_tmp_ptxt != NULL) {
		_in_ptxt = (char*)malloc(_len_ptxt);
		if (_in_ptxt == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_ptxt, _tmp_ptxt, _len_ptxt);
	}
	if (_tmp_ctxt != NULL) {
		if ((_in_ctxt = (char*)malloc(_len_ctxt)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_ctxt, 0, _len_ctxt);
	}
	ecall_encrypt(_in_ptxt, _tmp_ptxt_size, _in_ctxt, _tmp_ctxt_size);
err:
	if (_in_ptxt) free(_in_ptxt);
	if (_in_ctxt) {
		memcpy(_tmp_ctxt, _in_ctxt, _len_ctxt);
		free(_in_ctxt);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_decrypt(void* pms)
{
	ms_ecall_decrypt_t* ms = SGX_CAST(ms_ecall_decrypt_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_ctxt = ms->ms_ctxt;
	int _tmp_ctxt_size = ms->ms_ctxt_size;
	size_t _len_ctxt = _tmp_ctxt_size;
	char* _in_ctxt = NULL;
	char* _tmp_ptxt = ms->ms_ptxt;
	int _tmp_ptxt_size = ms->ms_ptxt_size;
	size_t _len_ptxt = _tmp_ptxt_size;
	char* _in_ptxt = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_decrypt_t));
	CHECK_UNIQUE_POINTER(_tmp_ctxt, _len_ctxt);
	CHECK_UNIQUE_POINTER(_tmp_ptxt, _len_ptxt);

	if (_tmp_ctxt != NULL) {
		_in_ctxt = (char*)malloc(_len_ctxt);
		if (_in_ctxt == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_ctxt, _tmp_ctxt, _len_ctxt);
	}
	if (_tmp_ptxt != NULL) {
		if ((_in_ptxt = (char*)malloc(_len_ptxt)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_ptxt, 0, _len_ptxt);
	}
	ecall_decrypt(_in_ctxt, _tmp_ctxt_size, _in_ptxt, _tmp_ptxt_size);
err:
	if (_in_ctxt) free(_in_ctxt);
	if (_in_ptxt) {
		memcpy(_tmp_ptxt, _in_ptxt, _len_ptxt);
		free(_in_ptxt);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_add_enc(void* pms)
{
	ms_ecall_add_enc_t* ms = SGX_CAST(ms_ecall_add_enc_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_ctxt1 = ms->ms_ctxt1;
	int _tmp_size1 = ms->ms_size1;
	size_t _len_ctxt1 = _tmp_size1;
	char* _in_ctxt1 = NULL;
	char* _tmp_ctxt2 = ms->ms_ctxt2;
	int _tmp_size2 = ms->ms_size2;
	size_t _len_ctxt2 = _tmp_size2;
	char* _in_ctxt2 = NULL;
	char* _tmp_buffer = ms->ms_buffer;
	int _tmp_buffer_size = ms->ms_buffer_size;
	size_t _len_buffer = _tmp_buffer_size;
	char* _in_buffer = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_add_enc_t));
	CHECK_UNIQUE_POINTER(_tmp_ctxt1, _len_ctxt1);
	CHECK_UNIQUE_POINTER(_tmp_ctxt2, _len_ctxt2);
	CHECK_UNIQUE_POINTER(_tmp_buffer, _len_buffer);

	if (_tmp_ctxt1 != NULL) {
		_in_ctxt1 = (char*)malloc(_len_ctxt1);
		if (_in_ctxt1 == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_ctxt1, _tmp_ctxt1, _len_ctxt1);
	}
	if (_tmp_ctxt2 != NULL) {
		_in_ctxt2 = (char*)malloc(_len_ctxt2);
		if (_in_ctxt2 == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_ctxt2, _tmp_ctxt2, _len_ctxt2);
	}
	if (_tmp_buffer != NULL) {
		if ((_in_buffer = (char*)malloc(_len_buffer)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_buffer, 0, _len_buffer);
	}
	ecall_add_enc(_in_ctxt1, _tmp_size1, _in_ctxt2, _tmp_size2, _in_buffer, _tmp_buffer_size);
err:
	if (_in_ctxt1) free(_in_ctxt1);
	if (_in_ctxt2) free(_in_ctxt2);
	if (_in_buffer) {
		memcpy(_tmp_buffer, _in_buffer, _len_buffer);
		free(_in_buffer);
	}

	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv;} ecall_table[6];
} g_ecall_table = {
	6,
	{
		{(void*)(uintptr_t)sgx_ecall_Arith_Core_sample, 0},
		{(void*)(uintptr_t)sgx_ecall_Arith_Core, 0},
		{(void*)(uintptr_t)sgx_ecall_add, 0},
		{(void*)(uintptr_t)sgx_ecall_encrypt, 0},
		{(void*)(uintptr_t)sgx_ecall_decrypt, 0},
		{(void*)(uintptr_t)sgx_ecall_add_enc, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
	uint8_t entry_table[1][6];
} g_dyn_entry_table = {
	1,
	{
		{0, 0, 0, 0, 0, 0, },
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

