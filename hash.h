#ifndef HASH_H
#define HASH_H
#include "stdlib.h"
#include "stack_func.h"

size_t create_djb2_hash(char* data, size_t size);

#endif //HASH_H