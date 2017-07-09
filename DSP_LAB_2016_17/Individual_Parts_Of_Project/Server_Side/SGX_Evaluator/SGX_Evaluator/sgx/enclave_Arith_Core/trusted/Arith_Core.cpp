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
*
*/



int Num_Proccess(char *exprs, int pos, stack<int> &numStack);
void Close_Brackt_Process(stack<int> &numStack, stack<char> &operator_Stack);
void Op_can_ProcessOperands(char op, stack<int> &numStack, stack<char> &operator_Stack);
void Op_execution(stack<int> &numStack, stack<char> &operator_Stack);
bool Op_Eval(char op, char prevOp);
int Eval_Expr(char* exprs);

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
    ocall_Arith_Core_sample(buf);
}

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


