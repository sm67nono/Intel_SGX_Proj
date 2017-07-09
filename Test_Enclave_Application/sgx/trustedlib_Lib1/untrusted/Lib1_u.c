#include "Lib1_u.h"
#include <errno.h>

typedef struct ms_ecall_Lib1_sample_t {
	int ms_retval;
} ms_ecall_Lib1_sample_t;

static const struct {
	size_t nr_ocall;
	void * table[1];
} ocall_table_Lib1 = {
	0,
	{ NULL },
};
sgx_status_t ecall_Lib1_sample(sgx_enclave_id_t eid, int* retval)
{
	sgx_status_t status;
	ms_ecall_Lib1_sample_t ms;
	status = sgx_ecall(eid, 0, &ocall_table_Lib1, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

