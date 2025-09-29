#ifndef HANDLERS_H
#define HANDLERS_H
#include "stdlib.h"
#include "stack.h"
#include "stack_func.h"

const size_t HANDLER_MAX_SIZE = 2048;

size_t stack_ctor_handle(long long int num_of_elem, long long int size_of_elem, const char* file, const char* func, int line);

func_errors stack_push_handle(size_t handle, const void* elem);

func_errors stack_pop_handle(size_t handle, void* elem);

func_errors stack_top_handle(size_t handle, void* elem);

func_errors stack_free_handle(size_t handle);

void stack_dump_handle(size_t handle);

#endif //HANDLERS_H