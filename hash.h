#ifndef HASH_H
#define HASH_H
#include "stdlib.h"
#include "stack.h"


struct temp_struct{
    size_t front_canary;
    char *ptr;
    char *data;
    size_t capacity;
    size_t size_of_elem;
    size_t tail_canary;
};

size_t calculate_struct_hash(stack_t_t* stack);

size_t create_djb2_hash(char* data, size_t size);

#endif //HASH_H