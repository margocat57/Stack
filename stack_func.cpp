#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stack_func.h"
#include "stack.h"
#include "hash.h"
#include "mistakes_code.h"
#include "my_assert.h"
#include "log.h"

const size_t RESERVED = 2 * sizeof(uintptr_t);

func_errors stack_realloc(stack_t_t* stack);

stack_t_t* stack_ctor(long long int num_of_elem, long long int size_of_elem){

    if(MY_ASSERT(0 < num_of_elem && num_of_elem<LLONG_MAX)) return NULL;
    if(MY_ASSERT(0 < size_of_elem && size_of_elem<LLONG_MAX)) return NULL;

    size_t num_of_elem_st = (size_t) num_of_elem;
    size_t size_of_elem_st = (size_t) size_of_elem;

    stack_t_t* stack = (stack_t_t*) calloc(1, sizeof(stack_t_t));
    if(MY_ASSERT(stack!=NULL)) return NULL;

    size_t real_size = num_of_elem_st*size_of_elem_st + RESERVED;
    stack->data = (char*)calloc(real_size, 1);

    if(MY_ASSERT(stack->data!=NULL)){
        free(stack);
        return NULL;
    }

    stack -> front_canary = FRONTCANARY;
    stack -> tail_canary = TAILCANARY;
    stack -> ptr = stack -> data + RESERVED/2; // сдвиг на элемент после служебного
    stack -> capacity = num_of_elem_st;
    stack -> size_of_elem = size_of_elem_st;

    stack -> djb2 = calculate_struct_hash(stack);

    stack -> djb2_data = create_djb2_hash(stack -> data, real_size);

    if(MY_ASSERT(stack_verify(stack) == NO_MISTAKE)) return NULL;

    return stack;
}

verify_errors stack_verify(stack_t_t* stack){
    verify_errors error = NO_MISTAKE;

    if(!stack){
        printf_to_log_file("NULL stack pointer\n");
        return NULL_STACK_PTR;
    }

    else if(stack->front_canary != FRONTCANARY || stack->tail_canary != TAILCANARY){
        printf_to_log_file("CANARIES CORRUPTED\n");
        return CANARY_ST_NOT_IN_PLACES;
    }

    else if(!stack -> data || !stack->ptr  || stack->capacity == 0 || stack->size_of_elem == 0){
        printf_to_log_file("Stack important data is zero or null-pointed\n");
        STACK_DUMP(stack);
        return NULL_STACK_PTR;
    }

    DEBUG(
    else if(stack->ptr < stack->data + RESERVED/2){
        printf_to_log_file("Stack pointer adress smaller than data adress\n");
        STACK_DUMP(stack);
        error = error | PTR_SMALLER_THAN_DATA;
    }
    )

    DEBUG(
    else if(stack -> ptr > stack->data + RESERVED/2 + stack->capacity * stack->size_of_elem){
        printf_to_log_file("Stack pointer adress bigger than data adress\n");
        STACK_DUMP(stack);
        error = error | PTR_BIGGER_THAN_DATA;
    }
    )

    DEBUG(
    else if(((size_t)stack->ptr - (size_t)(stack->data + sizeof(uintptr_t))) % stack->size_of_elem != 0){
        printf_to_log_file("Align not correct\n");
        STACK_DUMP(stack);
        error = error | ALIGN_NOT_CORRECT;
    }
    )

    DEBUG(
    else if (stack -> djb2 != calculate_struct_hash(stack)){
        printf_to_log_file("Stack hash is not correct\n");
        STACK_DUMP(stack);
        error = error | STACK_HASH_NOT_CORRECT;
    }
    )

    DEBUG(
    else if (stack -> djb2_data != create_djb2_hash(stack -> data, stack->capacity * stack->size_of_elem + RESERVED)){
        printf_to_log_file("Stack data hash is not correct\n");
        STACK_DUMP(stack);
        error = error | DATA_HASH_NOT_CORRECT;
    }
    )

    return error;
}

void stack_dump(stack_t_t* stack){
    if(!stack){
        printf_to_log_file("Stack can't be printed due to NULL ptr\n");
        return;
    }

    printf_to_log_file("Stack adress %p\n", stack);
    printf_to_log_file("Front canary: %X\n", stack->front_canary);
    printf_to_log_file("Data adress: %p\n", stack->data);
    printf_to_log_file("Elem pointer adress: %p\n", stack->ptr);
    printf_to_log_file("Stack capacity: %lu\n", stack->capacity);
    printf_to_log_file("Stack size of elem: %lu bytes\n", stack->size_of_elem);
    printf_to_log_file("DJB2 hash for data array: %lu\n", stack->djb2_data);
    printf_to_log_file("DJB2 hash for whole stack: %lu\n", stack->djb2);
    printf_to_log_file("Tail canary: %X\n", stack->tail_canary);

    if(stack -> data && stack->ptr  && stack->capacity != 0 && stack->size_of_elem != 0){
        char* data_start = stack->data + RESERVED/2;
        for(size_t idx = 0; idx < stack->capacity; idx++) {
            printf_to_log_file("Number of element in stack %zu\n", idx);
            printf_to_log_file("Adress of elem [%p]\n", data_start + idx * stack->size_of_elem);
            
            for(size_t byte = 0; byte < stack->size_of_elem; byte++) {
                printf_to_log_file("%02X ", (unsigned char)(data_start + idx * stack->size_of_elem)[byte]);
            }
            printf_to_log_file("\n");
        }
    }
}

// TODO в начало и конец массива канарейки не нулевые указатели
func_errors stack_push(stack_t_t* stack, const void* elem){

    func_errors error = NO_MISTAKE_FUNC; 
    if(MY_ASSERT(stack_verify(stack) == NO_MISTAKE)) error = error | VERIFY_FAILED;
    if(MY_ASSERT(elem != NULL)) error = error | FUNC_PARAM_IS_NULL;
    if(error) return error;

    size_t current_size = ((size_t)stack->ptr - (size_t)(stack->data + RESERVED/ 2)) / (size_t)stack->size_of_elem;
    if (current_size >= stack->capacity){
        stack_realloc(stack);
    }
    memcpy(stack->ptr, elem, stack->size_of_elem);
    stack->ptr += stack -> size_of_elem;

    stack ->djb2 = calculate_struct_hash(stack);
    stack -> djb2_data = create_djb2_hash(stack -> data, stack->size_of_elem * stack->capacity + RESERVED);

    if(MY_ASSERT(stack_verify(stack) == NO_MISTAKE)) error = error | VERIFY_FAILED;
    return error;

}

func_errors stack_pop(stack_t_t* stack, void* elem){

    if(MY_ASSERT(stack_verify(stack) == NO_MISTAKE)) return VERIFY_FAILED;
    if(!elem){
        if (stack->ptr >= stack->data + RESERVED / 2 + stack->size_of_elem){
            stack -> ptr -= stack -> size_of_elem;
            memset(stack->ptr, 0, stack->size_of_elem);
        }
        return NO_MISTAKE_FUNC;
    }
    
    if (stack->ptr >= stack->data + RESERVED / 2 + stack->size_of_elem){
        stack -> ptr -= stack -> size_of_elem;
        memcpy(elem, stack -> ptr, stack->size_of_elem);
        memset(stack->ptr, 0, stack->size_of_elem);
    }

    stack ->djb2 = calculate_struct_hash(stack);
    stack -> djb2_data = create_djb2_hash(stack -> data, stack->size_of_elem * stack->capacity + RESERVED);

    if(MY_ASSERT(stack_verify(stack) == NO_MISTAKE)) return VERIFY_FAILED;
    return NO_MISTAKE_FUNC;
}

func_errors stack_top(stack_t_t* stack, void* elem){

    func_errors error = NO_MISTAKE_FUNC;
    if(MY_ASSERT(stack_verify(stack) == NO_MISTAKE)) error = error | VERIFY_FAILED;
    if(MY_ASSERT(elem != NULL)) error = error | FUNC_PARAM_IS_NULL;
    if(error) return error;
    
    if (stack->ptr >= stack->data + RESERVED / 2 + stack->size_of_elem){
        stack -> ptr -= stack -> size_of_elem;
        memcpy(elem, stack -> ptr, stack->size_of_elem);
        stack -> ptr += stack -> size_of_elem;
    }

    return NO_MISTAKE_FUNC; 
}

func_errors stack_realloc(stack_t_t* stack){

    if(MY_ASSERT(stack_verify(stack) == NO_MISTAKE)) return VERIFY_FAILED;
    
    size_t new_capacity = stack->capacity * 2;
    size_t ptr_pos = (size_t)stack->ptr - (size_t)(stack->data + RESERVED/2);

    char* new_data = (char*)realloc(stack->data, new_capacity*stack->size_of_elem + RESERVED);
    if(MY_ASSERT(new_data != NULL)) return ALLOC_ERROR;

    stack->data = new_data;
    memset(stack->data + RESERVED/2 + stack->capacity * stack->size_of_elem, 0, (new_capacity-stack->capacity)*stack->size_of_elem + RESERVED/2);

    stack->capacity = new_capacity;
    stack->ptr = stack->data + RESERVED/2 + ptr_pos;

    stack ->djb2 = calculate_struct_hash(stack);
    stack -> djb2_data = create_djb2_hash(stack -> data, stack->size_of_elem * stack->capacity + RESERVED);

    if(MY_ASSERT(stack_verify(stack) == NO_MISTAKE)) return VERIFY_FAILED;
    return NO_MISTAKE_FUNC;

}

func_errors free_stack(stack_t_t* stack){
    if(MY_ASSERT(stack_verify(stack) == NO_MISTAKE)) return VERIFY_FAILED;

    memset(stack->data, 0, stack->capacity * stack->size_of_elem);
    free(stack->data);

    memset(stack, 0, sizeof(stack_t_t));
    free(stack);
    return NO_MISTAKE_FUNC;
}