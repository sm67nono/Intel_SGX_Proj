#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */

#include "Enclave_One.h"
#include "Enclave_One_t.h"  /* print_string */

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
void printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_Enclave_One_sample(buf);
}

int ecall_Enclave_One_sample()
{
  printf("Hi! You are in ENCLAVE_ONE\n");
  return 0;
}

