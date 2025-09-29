//!@file
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stack_func.h"
#include "hash.h"
#include "mistakes_code.h"
#include "my_assert.h"
#include "log.h"

//! Memory for canaries in data array
const size_t RESERVED = 2 * sizeof(uintptr_t);

const size_t FRONTCANARY_STACK = 0x42554A42;
const size_t TAILCANARY_STACK = 0x44454444;

const size_t FRONTCANARY_DATA = 0x4255463132333445;
const size_t TAILCANARY_DATA = 0x4E45534255464145;

//! Structure for containing stack
struct stack_t_t
{
    size_t front_canary; //!< front stack canary
    char *ptr;           //!< pointer to the top of the stack
    char *data;          //!< array of stack elements
    const char *file;    //!< file where stack was inicialized
    const char *func;    //!< function where stack was inicialized
    int line;            //!< line where stack was inicialized
    size_t capacity;     //!< quantaty of elements in stack
    size_t size_of_elem; //!< size of stack element
    size_t djb2_data;    //! hash for data array
    size_t djb2_stack;   //! hash for stack
    size_t tail_canary;  //! tail stack canary
};

//! Stack memory expansion function
//!
//! @return func_errors error code (NO_MISTAKE_FUNC on success)
//!
//! Performs the following operations:
//!    - Validates stack before operation using stack_verify()
//!    - Saves current stack pointer position
//!    - Expands data array memory using realloc()
//!    - Initializes new memory with zeros
//!    - Doubles stack capacity and updates pointer position
//!    - Restores data tail canary
//!    - Recalculates stack hashes
//!    - Validates stack after operation using stack_verify()
//!
//! Returns ALLOC_ERROR if realloc() fails
func_errors stack_realloc(stack_t_t *stack);

//! Byte dump function for hexadecimal memory representation
//!
//! Prints hexadecimal representation of memory bytes
//!     Each byte is printed as two-digit hexadecimal number
//!     Output is formatted with spaces between bytes and newline at end
//! If dump_data is NULL, function returns early
void bytes_dump(char *dump_data, size_t max_size_elem);

//! Stack hash update function 
//!
//! Calculates two separate hashes:
//!     - djb2_stack: Hash of stack structure (from front_canary to tail_canary)
//!     - djb2_data: Hash of data array (including reserved areas)
//! @note Hash calculation includes canaries 
void update_stack_data_hash(stack_t_t *stack);

stack_t_t *stack_ctor(long long int num_of_elem, long long int size_of_elem, const char *file, const char *func, int line)
{
    if (MY_ASSERT(0 < num_of_elem && num_of_elem < LLONG_MAX))
        return NULL;
    if (MY_ASSERT(0 < size_of_elem && size_of_elem < LLONG_MAX))
        return NULL;

    size_t num_of_elem_st = (size_t)num_of_elem;
    size_t size_of_elem_st = (size_t)size_of_elem;

    stack_t_t *stack = (stack_t_t *)calloc(1, sizeof(stack_t_t));
    if (MY_ASSERT(stack != NULL))
        return NULL;

    size_t real_size = num_of_elem_st * size_of_elem_st + RESERVED;
    stack->data = (char *)calloc(real_size, 1);

    if (MY_ASSERT(stack->data != NULL))
    {
        free(stack);
        return NULL;
    }

    stack->front_canary = FRONTCANARY_STACK;
    stack->tail_canary = TAILCANARY_STACK;
    stack->ptr = stack->data + RESERVED / 2; // сдвиг на элемент после служебного
    stack->capacity = num_of_elem_st;
    stack->size_of_elem = size_of_elem_st;
    stack->file = file;
    stack->func = func;
    stack->line = line;

    memcpy(stack->data, &FRONTCANARY_DATA, sizeof(RESERVED / 2));
    memcpy(stack->data + RESERVED / 2 + stack->capacity * stack->size_of_elem, &TAILCANARY_DATA, sizeof(RESERVED / 2));

    update_stack_data_hash(stack);

    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        return NULL;

    return stack;
}

verify_errors stack_verify(stack_t_t *stack)
{
    verify_errors error = NO_MISTAKE;

    if (!stack)
    {
        printf_to_log_file("NULL stack pointer\n");
        return NULL_STACK_PTR;
    }

    if (stack->front_canary != FRONTCANARY_STACK || stack->tail_canary != TAILCANARY_STACK)
    {
        printf_to_log_file("CANARIES of stack CORRUPTED\n");
        return CANARY_ST_NOT_IN_PLACES;
    }

    if (!stack->data || !stack->ptr || stack->capacity == 0 || stack->size_of_elem == 0)
    {
        printf_to_log_file("Stack important data is zero or null-pointed\n");
        STACK_DUMP(stack);
        return NULL_STACK_PTR;
    }

    DEBUG(
        if (stack->ptr < stack->data + RESERVED / 2) {
            printf_to_log_file("Stack pointer adress smaller than data adress\n");
            STACK_DUMP(stack);
            error = error | PTR_SMALLER_THAN_DATA;
        })

    DEBUG(
        if (stack->ptr > stack->data + RESERVED / 2 + stack->capacity * stack->size_of_elem) {
            printf_to_log_file("Stack pointer adress bigger than data adress\n");
            STACK_DUMP(stack);
            error = error | PTR_BIGGER_THAN_DATA;
        })

    DEBUG(
        if (((size_t)stack->ptr - (size_t)(stack->data + sizeof(uintptr_t))) % stack->size_of_elem != 0) {
            printf_to_log_file("Align not correct\n");
            STACK_DUMP(stack);
            error = error | ALIGN_NOT_CORRECT;
        })

    DEBUG(
        size_t stack_old = stack->djb2_stack;
        size_t data_old = stack->djb2_data;
        update_stack_data_hash(stack);

        if (stack_old != stack->djb2_stack) {
            printf_to_log_file("Stack hash is not correct\n");
            STACK_DUMP(stack);
            error = error | STACK_HASH_NOT_CORRECT;
        }

        stack->djb2_data = create_djb2_hash(stack->data, stack->capacity * stack->size_of_elem + RESERVED);
        if (data_old != stack->djb2_data) {
            printf_to_log_file("Stack data hash is not correct\n");
            STACK_DUMP(stack);
            error = error | DATA_HASH_NOT_CORRECT;
        })

    DEBUG(
        char *tail_canary_ptr = stack->data + RESERVED / 2 + stack->capacity * stack->size_of_elem;
        if (memcmp(stack->data, &FRONTCANARY_DATA, RESERVED / 2) != 0 || memcmp(tail_canary_ptr, &TAILCANARY_DATA, RESERVED / 2) != 0) {
            printf("DATA CANARIES DEAD\n");
            STACK_DUMP(stack);
            error = error | CANARY_DT_NOT_IN_PLACES;
        })

    return error;
}

void stack_dump(stack_t_t *stack)
{
    if (!stack)
    {
        printf_to_log_file("Stack can't be printed due to NULL ptr\n");
        return;
    }

    DEBUG(printf_to_log_file("Stack was created at %s %s %d\n", stack->file, stack->func, stack->line);)

    printf_to_log_file("Stack adress %p\n", stack);
    printf_to_log_file("Front stack canary: %X\n", stack->front_canary);
    printf_to_log_file("Data adress: %p\n", stack->data);
    printf_to_log_file("Elem pointer adress: %p\n", stack->ptr);
    printf_to_log_file("Stack capacity: %lu\n", stack->capacity);
    printf_to_log_file("Stack size of elem: %lu bytes\n", stack->size_of_elem);
    printf_to_log_file("DJB2 hash for data array: %lu\n", stack->djb2_data);
    printf_to_log_file("DJB2 hash for whole stack: %lu\n", stack->djb2_stack);
    printf_to_log_file("Tail stack canary: %X\n", stack->tail_canary);

    if (stack->data && stack->ptr && stack->capacity != 0 && stack->size_of_elem != 0)
    {
        printf_to_log_file("Data front canary:\n");
        bytes_dump(stack->data, RESERVED / 2);

        printf_to_log_file("Data tail canary:\n");
        char *tail_canary_ptr = stack->data + RESERVED / 2 + stack->capacity * stack->size_of_elem;
        bytes_dump(tail_canary_ptr, RESERVED / 2);

        char *data_start = stack->data + RESERVED / 2;
        for (size_t idx = 0; idx < stack->capacity; idx++)
        {
            printf_to_log_file("Number of element in stack %zu\n", idx);
            printf_to_log_file("Adress of elem [%p]\n", data_start + idx * stack->size_of_elem);

            printf_to_log_file("Elem by bytes:\n");
            bytes_dump(data_start + idx * stack->size_of_elem, stack->size_of_elem);
        }
    }
    printf_to_log_file("\n");
}

void bytes_dump(char *dump_data, size_t max_size_elem)
{
    if (MY_ASSERT(dump_data != NULL))
        return;
    for (size_t byte = 0; byte < max_size_elem; byte++)
    {
        printf_to_log_file("%02X ", (unsigned char)(dump_data)[byte]);
    }
    printf_to_log_file("\n");
}

void update_stack_data_hash(stack_t_t *stack)
{
    stack->djb2_stack = 0;
    stack->djb2_data = 0;
    size_t size = (size_t)((char *)&stack->tail_canary - (char *)&stack->front_canary) + sizeof(stack->tail_canary);
    stack->djb2_stack = create_djb2_hash((char *)stack, size);
    stack->djb2_data = create_djb2_hash(stack->data, stack->size_of_elem * stack->capacity + RESERVED);
}

func_errors stack_push(stack_t_t *stack, const void *elem)
{
    func_errors error = NO_MISTAKE_FUNC;
    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        error = error | VERIFY_FAILED;
    if (MY_ASSERT(elem != NULL))
        error = error | FUNC_PARAM_IS_NULL;
    if (error)
        return error;

    size_t current_size = ((size_t)stack->ptr - (size_t)(stack->data + RESERVED / 2)) / (size_t)stack->size_of_elem;
    if (current_size >= stack->capacity)
    {
        stack_realloc(stack);
    }
    memcpy(stack->ptr, elem, stack->size_of_elem);
    stack->ptr += stack->size_of_elem;

    update_stack_data_hash(stack);

    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        error = error | VERIFY_FAILED;
    return error;
}

func_errors stack_pop(stack_t_t *stack, void *elem)
{
    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        return VERIFY_FAILED;
    if (!elem)
    {
        if (stack->ptr >= stack->data + RESERVED / 2 + stack->size_of_elem)
        {
            stack->ptr -= stack->size_of_elem;
            memset(stack->ptr, 0, stack->size_of_elem);

            update_stack_data_hash(stack);
        }
        return NO_MISTAKE_FUNC;
    }

    if (stack->ptr >= stack->data + RESERVED / 2 + stack->size_of_elem)
    {
        stack->ptr -= stack->size_of_elem;
        memcpy(elem, stack->ptr, stack->size_of_elem);
        memset(stack->ptr, 0, stack->size_of_elem);
    }

    update_stack_data_hash(stack);

    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        return VERIFY_FAILED;
    return NO_MISTAKE_FUNC;
}

func_errors stack_top(stack_t_t *stack, void *elem)
{
    func_errors error = NO_MISTAKE_FUNC;
    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        error = error | VERIFY_FAILED;
    if (MY_ASSERT(elem != NULL))
        error = error | FUNC_PARAM_IS_NULL;
    if (error)
        return error;

    if (stack->ptr >= stack->data + RESERVED / 2 + stack->size_of_elem)
    {
        stack->ptr -= stack->size_of_elem;
        memcpy(elem, stack->ptr, stack->size_of_elem);
        stack->ptr += stack->size_of_elem;
    }

    return NO_MISTAKE_FUNC;
}

func_errors stack_realloc(stack_t_t *stack)
{
    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        return VERIFY_FAILED;

    size_t new_capacity = stack->capacity * 2;
    size_t ptr_pos = (size_t)stack->ptr - (size_t)(stack->data + RESERVED / 2);

    char *new_data = (char *)realloc(stack->data, new_capacity * stack->size_of_elem + RESERVED);
    if (MY_ASSERT(new_data != NULL))
        return ALLOC_ERROR;

    stack->data = new_data;
    memset(stack->data + RESERVED / 2 + stack->capacity * stack->size_of_elem, 0, (new_capacity - stack->capacity) * stack->size_of_elem + RESERVED / 2);

    stack->capacity = new_capacity;
    stack->ptr = stack->data + RESERVED / 2 + ptr_pos;

    memcpy(stack->data + RESERVED / 2 + stack->capacity * stack->size_of_elem, &TAILCANARY_DATA, sizeof(RESERVED / 2));

    update_stack_data_hash(stack);

    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        return VERIFY_FAILED;
    return NO_MISTAKE_FUNC;
}

func_errors stack_free(stack_t_t *stack)
{
    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        return VERIFY_FAILED;

    memset(stack->data, 0, stack->capacity * stack->size_of_elem);
    free(stack->data);

    memset(stack, 0, sizeof(stack_t_t));
    free(stack);
    return NO_MISTAKE_FUNC;
}