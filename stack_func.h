#ifndef STACK_FUNC_H
#define STACK_FUNC_H
#include "stack.h"
#include "mistakes_code.h"

#ifdef _DEBUG
    #define STACK_DUMP(stack) stack_dump(stack);
#else 
    #define STACK_DUMP(stack)
#endif //_DEBUG

stack_t_t* stack_ctor(long long int num_of_elem, long long int size_of_elem);

stack_err_t stack_verify(stack_t_t* stack);

void stack_dump(stack_t_t* stack);

void stack_push(stack_t_t* stack, const void* elem);

void stack_pop(stack_t_t* stack, void* elem);

void stack_top(stack_t_t* stack, void* elem);

void stack_realloc(stack_t_t* stack);

void free_stack(stack_t_t* stack);
#endif //STACK_FUNC_H