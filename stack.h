#ifndef STACK_H
#define STACK_H
#include <stdlib.h>

const size_t FRONTCANARY_STACK = 0x42554A42;
const size_t TAILCANARY_STACK = 0x44454444;

const size_t FRONTCANARY_DATA = 0x4255463132333445;
const size_t TAILCANARY_DATA = 0x4E45534255464145;

struct stack_t_t
{
    size_t front_canary;
    char *ptr;
    char *data;
    const char *file;
    const char *func;
    int line;
    size_t capacity;
    size_t size_of_elem;
    size_t djb2_data;
    size_t djb2_stack;
    size_t tail_canary;
};



#endif // STACK_H