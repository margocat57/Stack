#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <stdlib.h>

typedef int stack_elem_t;

extern bool DEBUG_STACK;

struct stack_info
{
    const char *name;
    int line;
    const char *func;
    const char *file;
};

struct stack_t_t
{
#ifdef _DEBUG
    stack_info info;
#endif //_DEBUG
    stack_elem_t *data;
    size_t ptr;
    size_t capacity;
    bool is_data_init;
};

enum StackErr_t
{
    NO_MISTAKE = 0,
    STK_NULL_PTR = 1,
    DATA_NULL_PTR = 2,
    OUT_OF_INDEX = 3,
    INCORR_SIZE = 4,
    VAR_NULL_PTR = 5,
    ALLOC_MISTAKE = 6,
    INCORR_IDX = 7
};

struct flags_for_tests
{
    const char *flag_test;
    bool is_flag_test;
};

#endif // STRUCTURES_H
