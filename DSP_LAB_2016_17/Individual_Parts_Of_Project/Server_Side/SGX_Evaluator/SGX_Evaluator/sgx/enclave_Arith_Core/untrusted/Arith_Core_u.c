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

