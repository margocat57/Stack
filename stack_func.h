#ifndef STACK_FUNC_H
#define STACK_FUNC_H
#include "stack.h"
#include "mistakes_code.h"

typedef int64_t verify_errors; 
typedef int64_t func_errors; 

#ifdef _DEBUG
    #define STACK_DUMP(stack) stack_dump(stack);
#else 
    #define STACK_DUMP(stack)
#endif //_DEBUG

#ifdef _DEBUG
    #define DEBUG(...) __VA_ARGS__
#else 
    #define DEBUG(...)
#endif //_DEBUG

stack_t_t* stack_ctor(long long int num_of_elem, long long int size_of_elem, const char* file, const char* func, int line);

verify_errors stack_verify(stack_t_t* stack);

void stack_dump(stack_t_t* stack);

func_errors stack_push(stack_t_t* stack, const void* elem);

func_errors stack_pop(stack_t_t* stack, void* elem);

func_errors stack_top(stack_t_t* stack, void* elem);

func_errors stack_free(stack_t_t* stack);
#endif //STACK_FUNC_H