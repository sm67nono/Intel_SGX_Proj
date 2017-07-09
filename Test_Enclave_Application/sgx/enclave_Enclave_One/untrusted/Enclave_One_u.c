#include "Enclave_One_u.h"
#include <errno.h>

typedef struct ms_ecall_Enclave_One_sample_t {
	int ms_retval;
} ms_ecall_Enclave_One_sample_t;

typedef struct ms_ocall_Enclave_One_sample_t {
	char* ms_str;
} ms_ocall_Enclave_One_sample_t;

static sgx_status_t SGX_CDECL Enclave_One_ocall_Enclave_One_sample(void* pms)
{
	ms_ocall_Enclave_One_sample_t* ms = SGX_CAST(ms_ocall_Enclave_One_sample_t*, pms);
	ocall_Enclave_One_sample((const char*)ms->ms_str);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[1];
} ocall_table_Enclave_One = {
	1,
	{
		(void*)Enclave_One_ocall_Enclave_One_sample,
	}
};
sgx_status_t ecall_Enclave_One_sample(sgx_enclave_id_t eid, int* retval)
{
	sgx_status_t status;
	ms_ecall_Enclave_One_sample_t ms;
	status = sgx_ecall(eid, 0, &ocall_table_Enclave_One, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

