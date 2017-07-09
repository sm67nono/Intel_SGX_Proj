#include "Arith_Core_u.h"
#include <errno.h>

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

static sgx_status_t SGX_CDECL Arith_Core_ocall_Arith_Core_sample(void* pms)
{
	ms_ocall_Arith_Core_sample_t* ms = SGX_CAST(ms_ocall_Arith_Core_sample_t*, pms);
	ocall_Arith_Core_sample((const char*)ms->ms_str);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[1];
} ocall_table_Arith_Core = {
	1,
	{
		(void*)Arith_Core_ocall_Arith_Core_sample,
	}
};
sgx_status_t ecall_Arith_Core_sample(sgx_enclave_id_t eid, int* retval)
{
	sgx_status_t status;
	ms_ecall_Arith_Core_sample_t ms;
	status = sgx_ecall(eid, 0, &ocall_table_Arith_Core, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_Arith_Core(sgx_enclave_id_t eid, int* retval, char* x, char* result, int buffer_size)
{
	sgx_status_t status;
	ms_ecall_Arith_Core_t ms;
	ms.ms_x = x;
	ms.ms_result = result;
	ms.ms_buffer_size = buffer_size;
	status = sgx_ecall(eid, 1, &ocall_table_Arith_Core, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_add(sgx_enclave_id_t eid, int* retval, int x, int y)
{
	sgx_status_t status;
	ms_ecall_add_t ms;
	ms.ms_x = x;
	ms.ms_y = y;
	status = sgx_ecall(eid, 2, &ocall_table_Arith_Core, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_encrypt(sgx_enclave_id_t eid, char* ptxt, int ptxt_size, char* ctxt, int ctxt_size)
{
	sgx_status_t status;
	ms_ecall_encrypt_t ms;
	ms.ms_ptxt = ptxt;
	ms.ms_ptxt_size = ptxt_size;
	ms.ms_ctxt = ctxt;
	ms.ms_ctxt_size = ctxt_size;
	status = sgx_ecall(eid, 3, &ocall_table_Arith_Core, &ms);
	return status;
}

sgx_status_t ecall_decrypt(sgx_enclave_id_t eid, char* ctxt, int ctxt_size, char* ptxt, int ptxt_size)
{
	sgx_status_t status;
	ms_ecall_decrypt_t ms;
	ms.ms_ctxt = ctxt;
	ms.ms_ctxt_size = ctxt_size;
	ms.ms_ptxt = ptxt;
	ms.ms_ptxt_size = ptxt_size;
	status = sgx_ecall(eid, 4, &ocall_table_Arith_Core, &ms);
	return status;
}

sgx_status_t ecall_add_enc(sgx_enclave_id_t eid, char* ctxt1, int size1, char* ctxt2, int size2, char* buffer, int buffer_size)
{
	sgx_status_t status;
	ms_ecall_add_enc_t ms;
	ms.ms_ctxt1 = ctxt1;
	ms.ms_size1 = size1;
	ms.ms_ctxt2 = ctxt2;
	ms.ms_size2 = size2;
	ms.ms_buffer = buffer;
	ms.ms_buffer_size = buffer_size;
	status = sgx_ecall(eid, 5, &ocall_table_Arith_Core, &ms);
	return status;
}

