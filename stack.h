#ifndef STACK_H
#define STACK_H
#include <stdlib.h>

const size_t FRONTCANARY_STACK = 0x42554A42;
const size_t TAILCANARY_STACK = 0x44454444;

const stack_elem_t FRONTCANARY_DATA = 0xBAAD;
const stack_elem_t TAILCANARY_DATA = 0xCAFE;

//! Structure for containing stack
struct stack_t_t
{
    size_t front_canary; //!< front stack canary
    size_t top;          //!< pointer to the top of the stack
    stack_elem_t *data;  //!< array of stack elements
    const char *file;    //!< file where stack was inicialized
    const char *func;    //!< function where stack was inicialized
    int line;            //!< line where stack was inicialized
    size_t capacity;     //!< quantaty of elements in stack
    size_t djb2_data;    //!< hash for data array
    size_t djb2_stack;   //!< hash for stack
    size_t tail_canary;  //!< tail stack canary
};

#endif //STACK_H