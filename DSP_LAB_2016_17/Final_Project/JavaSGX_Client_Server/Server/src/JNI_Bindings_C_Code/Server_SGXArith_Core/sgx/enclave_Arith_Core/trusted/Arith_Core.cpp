#include <cstring>
#include <stdarg.h>
#include <stdio.h>
#include <stack>
/* vsnprintf */
#include "Arith_Core.h"
#include "Arith_Core_t.h"  /* print_string */
using namespace std;


/**
*
*@author manna: DSP_LAB 2016_17
*@author
*
*/


int Num_Proccess(char *exprs, int pos, stack<int> &numStack);
void Close_Brackt_Process(stack<int> &numStack, stack<char> &operator_Stack);
void Op_can_ProcessOperands(char op, stack<int> &numStack, stack<char> &operator_Stack);
void Op_execution(stack<int> &numStack, stack<char> &operator_Stack);
bool Op_Eval(char op, char prevOp);
int Eval_Expr(char* exprs);

//Encryption and Decryption logic:

#define MACLEN 16
#define HASHLEN 32
#define IVLEN 12

// not production software
const sgx_aes_gcm_128bit_key_t aes_key[16] = { 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b',
		'c', 'd', 'a', 'b', 'c', 'd' };
const uint8_t aes_iv[12] = { 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd' };

/*
 * printf:Invokes OCALL to display the enclave buffer to the terminal.
 */
void printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_Arith_Core_sample(buf);
}


int ecall_add(int x, int y) {
	return x + y;
}

/**
 * Encrypt using AES GCM
 *
 * Size of returned string is equal to the size of the input string + size of MAC (MACLEN)
 *
 */
void aes_encrypt(char* ptxtt, int ptxt_size, char* ctxtt) {
	uint8_t macbuffer[MACLEN];

	unsigned char* ptxt=(unsigned char*)ptxtt;
	unsigned char* ctxt=(unsigned char*)ctxtt;


	sgx_status_t status = sgx_rijndael128GCM_encrypt(aes_key,ptxt, ptxt_size, ctxt, aes_iv, IVLEN, NULL, 0, &macbuffer);

	// append mac at the end of ciphertext
	memcpy(ctxt + ptxt_size, macbuffer, MACLEN);
}

/**
 * Decrypt using AES GCM
 *
 * Size of returned string is equal to the size of the input string - size of MAC (MACLEN)
 *
 */
void aes_decrypt(char* ctxtt, int ctxt_size, char* ptxtt) {

	// size of ciphertext minus the MAC
	ctxt_size -= MACLEN;

	// start position of MAC
	char* mac_pos = ctxtt + ctxt_size;

	// separate MAC
	uint8_t macbuffer[MACLEN];
	memcpy(macbuffer, mac_pos, MACLEN);

	// decrypt
    //Huge fix and conversion: manna March 2017
	unsigned char* ptxt=(unsigned char*)ptxtt;
	unsigned char* ctxt=(unsigned char*)ctxtt;

	sgx_status_t status = sgx_rijndael128GCM_decrypt(aes_key, ctxt, ctxt_size, ptxt, aes_iv, IVLEN,NULL, 0, (const sgx_aes_gcm_128bit_tag_t*) &macbuffer);
}

void ecall_encrypt(char* ptxt, int ptxt_size, char* ctxt, int ctxt_size) {
	aes_encrypt(ptxt, ptxt_size, ctxt);
}

void ecall_decrypt(char* ctxt, int ctxt_size, char* ptxt, int ptxt_size) {
	aes_decrypt(ctxt, ctxt_size, ptxt);
}

void ecall_add_enc(char* ctxt1, int ctxt1_size, char* ctxt2, int ctxt2_size, char* result,
		int result_size) {

	// decrypt value 1

	char* ptxt1 = (char*) malloc(ctxt1_size - MACLEN);
	aes_decrypt(ctxt1, ctxt1_size, ptxt1);



	// decrypt value 2
	char* ptxt2 = (char*) malloc(ctxt2_size - MACLEN);
	aes_decrypt(ctxt2, ctxt2_size, ptxt2);

	// cast to int and add the 2 numbers
	//int a = atoi(ptxt1);
	// printf("a=%d\n", a);
	//int b = atoi(ptxt2);
	// printf("b=%d\n", b);
	//Evaluating the expression call
	int r = Eval_Expr(ptxt1);

	// cast to string
	char resBuf[ctxt1_size - MACLEN];

	snprintf(resBuf, ctxt1_size, "%d", r);

	// encrypt result
	aes_encrypt(resBuf, ctxt1_size - MACLEN, result);

}

/*Core logic for the Arithmetic Evaluator Below
 * manna March 2017
 */



/*
 * printf:
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */

int ecall_Arith_Core_sample()
{
  printf("IN ARITH_CORE\n");
  return 0;
}

int ecall_Arith_Core(char* inpExpr, char* result, int buffer_size)
//int ecall_Arith_Core()
{


  printf("Calculating Inside the Enclave. Input Expression: %s :",inpExpr);
  int res = Eval_Expr(inpExpr);
  printf("\nResult: %d ",res);
  return 0;
}


int Eval_Expr(char* exprs)
{
	stack<int> numStack ;
	stack<char> operator_Stack ;


    operator_Stack.push('(');
    //numStack.push(10);
    //char c = operator_Stack.top();
    //cout<<c;



    int pos = 0;
    char expr[30];
    //strnCpy fix after referring SGX Developer guide : manna Feb 2017
    strncpy(expr,exprs,30);
    int length = strlen(expr);
    //cout<<length;
    while(pos <= strlen(expr))
    {
        if (pos == strlen(expr)|| expr[pos] == ')')
        {
            Close_Brackt_Process(numStack, operator_Stack);
            pos++;
        }
        else if (expr[pos] >= '0' && expr[pos] <= '9')
        {
            pos = Num_Proccess(expr, pos, numStack);
        }
        else
        {
            Op_can_ProcessOperands(expr[pos], numStack, operator_Stack);
            pos++;
        }

    }

    int numtop =  numStack.top();
   // cout<<numtop;
   // numStack.pop();
    return numtop;

}

void Close_Brackt_Process(stack<int> &numStack, stack<char> &operator_Stack)
{

    while (operator_Stack.top() != '(')
        Op_execution(numStack, operator_Stack);

    operator_Stack.pop(); // Remove the opening parenthesis

}


int Num_Proccess(char *exprs, int pos, stack<int> &numStack)
{

    int value = 0;
    char expr[30];
    //strnCpy fix after referring SGX Developer guide : manna Feb 2017
    strncpy(expr,exprs,30);
    while (pos < strlen(expr) && expr[pos] >= '0' && expr[pos] <= '9')
        value = 10 * value + (int)(expr[pos++] - '0');//Making the number by parsing the expr string

    numStack.push(value);

    return pos;

}

void Op_can_ProcessOperands(char op, stack<int> &numStack, stack<char> &operator_Stack)
{

    while (operator_Stack.size() > 0 && Op_Eval(op, operator_Stack.top()))
        Op_execution(numStack, operator_Stack);

    operator_Stack.push(op);

}

bool Op_Eval(char op, char prevOp)
{

    bool evaluate = false;

    switch (op)
    {
        case '+':
        case '-':
            evaluate = (prevOp != '(');
            break;
        case '*':
        case '\0':
            evaluate = (prevOp == '*' || prevOp == '\0');
            break;
        case ')':
            evaluate = true;
            break;
    }

    return evaluate;

}

void Op_execution(stack<int> &numStack, stack<char> &operator_Stack)
{

	int rightOperand = numStack.top();
    numStack.pop();
    int leftOperand = numStack.top();
    numStack.pop();
    char op = operator_Stack.top();
    operator_Stack.pop();

    int result = 0;
    switch (op)
    {
        case '+':
            result = leftOperand + rightOperand;
            break;
        case '-':
            result = leftOperand - rightOperand;
            break;
        case '*':
            result = leftOperand * rightOperand;
            break;
        case '\0':
            result = 0;
            break;
    }

    numStack.push(result);

}


