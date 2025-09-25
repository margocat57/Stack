#ifndef STACK_FUNC_H
#define STACK_FUNC_H
#include <stdlib.h>
#include <stdio.h>
#include "structures.h"

StackErr_t StackErr(stack_t_t *stk, FILE *log_file_ptr);

void StackDump(stack_t_t *stk, StackErr_t err, FILE *log_file_ptr);

StackErr_t StackCtor(stack_t_t *stk, long long int size, FILE *log_file_ptr);

StackErr_t FillPoison(stack_t_t *stk, long long int idx, FILE *log_file_ptr);

StackErr_t StackPush(stack_t_t *stk, long long int value, FILE *log_file_ptr);

StackErr_t MakeDataSizeBigger(stack_t_t *stk, FILE *log_file_ptr);

StackErr_t StackPop(stack_t_t *stk, stack_elem_t *elem, FILE *log_file_ptr);

StackErr_t StackTop(stack_t_t *stk, FILE *log_file_ptr, stack_elem_t *elem);

StackErr_t FreeStack(stack_t_t *stk, FILE *log_file_ptr);

#endif // STACK_FUNC_H