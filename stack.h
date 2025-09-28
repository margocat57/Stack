#ifndef STACK_H
#define STACK_H
#include <stdlib.h>

const size_t FRONTCANARY = 0x42554A42;
const size_t TAILCANARY = 0x44454444;


struct stack_t_t
{
    size_t front_canary;
    char *ptr;
    char *data;
    size_t capacity;
    size_t size_of_elem;
    size_t djb2_data;
    size_t djb2;
    size_t tail_canary;
};



#endif // STACK_H