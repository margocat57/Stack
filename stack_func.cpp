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

const char *FORM_SPEC_STK_EL = "%d";
const int POISON = -6666;

//! Memory for canaries in data array
const size_t CANARY_ELEMS = 2;

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

//! Stack hash update function 
//!
//! Calculates two separate hashes:
//!     - djb2_stack: Hash of stack structure (from front_canary to tail_canary)
//!     - djb2_data: Hash of data array (including reserved areas)
//! @note Hash calculation includes canaries 
void update_stack_data_hash(stack_t_t *stack);

verify_errors fill_poison(stack_t_t* stack, size_t start, size_t end);

stack_t_t *stack_ctor(long long int num_of_elem, const char *file, const char *func, int line)
{
    if (MY_ASSERT(0 < num_of_elem && num_of_elem < LLONG_MAX))
        return NULL;

    size_t num_of_elem_st = (size_t)num_of_elem;

    stack_t_t *stack = (stack_t_t *)calloc(1, sizeof(stack_t_t));
    if (MY_ASSERT(stack != NULL))
        return NULL;

    size_t real_size = num_of_elem_st DEBUG (+ CANARY_ELEMS);
    stack->data = (stack_elem_t *)calloc(real_size, sizeof(stack_elem_t));

    if (MY_ASSERT(stack->data != NULL))
    {
        free(stack);
        return NULL;
    }

    stack->front_canary = FRONTCANARY_STACK;
    stack->tail_canary = TAILCANARY_STACK;
    stack->top = 0 DEBUG(+ CANARY_ELEMS/2); 
    stack->capacity = num_of_elem_st;
    DEBUG(
    stack->file = file;
    stack->func = func;
    stack->line = line;
    )

    DEBUG(
    *stack->data = FRONTCANARY_DATA;
    (stack->data)[stack->capacity - 1 + CANARY_ELEMS] = TAILCANARY_DATA;
    )

    fill_poison(stack, 1, stack->capacity - 1  DEBUG(+ CANARY_ELEMS / 2));

    DEBUG(update_stack_data_hash(stack);)

    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        return NULL;

    return stack;
}

verify_errors fill_poison(stack_t_t* stack, size_t start, size_t end){
    if(MY_ASSERT(stack->top <= start && start < stack->capacity + CANARY_ELEMS)) 
        return INCORR_DIGIT_PARAMS;
    if(MY_ASSERT(stack->top <= end && end < stack->capacity + CANARY_ELEMS)) 
        return INCORR_DIGIT_PARAMS;

    for (size_t index = start; index <= end; index++)
    {
        stack->data[index] = POISON;
    }
    return NO_MISTAKE_FUNC;
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

    if (!stack->data || stack->capacity == 0)
    {
        printf_to_log_file("Stack important data is zero or null-pointed\n");
        STACK_DUMP(stack);
        return NULL_STACK_PTR;
    }

    if (stack->top < 0 DEBUG( + CANARY_ELEMS/2 - 1)){
        printf_to_log_file("Stack pointer adress smaller than data adress\n");
        STACK_DUMP(stack);
        return PTR_SMALLER_THAN_DATA;
    }


    if (stack->top > stack->capacity - 1 DEBUG( + CANARY_ELEMS)) {
        printf_to_log_file("Stack pointer adress bigger than data adress\n");
        STACK_DUMP(stack);
        return PTR_BIGGER_THAN_DATA;
    }


    DEBUG(
        size_t stack_old = stack->djb2_stack;
        size_t data_old = stack->djb2_data;
        update_stack_data_hash(stack);

        if (stack_old != stack->djb2_stack) {
            printf_to_log_file("Stack hash is not correct\n");
            STACK_DUMP(stack);
            error = error | STACK_HASH_NOT_CORRECT;
        }

        stack->djb2_data = create_djb2_hash((char*)stack->data, stack->capacity + CANARY_ELEMS);
        if (data_old != stack->djb2_data) {
            printf_to_log_file("Stack data hash is not correct\n");
            STACK_DUMP(stack);
            error = error | DATA_HASH_NOT_CORRECT;
        }
    )

    DEBUG(
        stack_elem_t tail_canary = (stack->data)[CANARY_ELEMS + stack->capacity - 1];
        if (tail_canary != TAILCANARY_DATA || *(stack->data) != FRONTCANARY_DATA) {
            printf("DATA CANARIES DEAD\n");
            STACK_DUMP(stack);
            error = error | CANARY_DT_NOT_IN_PLACES;
        }
    )

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
    printf_to_log_file("Pointer points at: %lu\n", stack->top);
    printf_to_log_file("Stack capacity: %lu\n", stack->capacity);

    DEBUG(
    printf_to_log_file("DJB2 hash for data array: %lu\n", stack->djb2_data);
    printf_to_log_file("DJB2 hash for whole stack: %lu\n", stack->djb2_stack);
    )

    printf_to_log_file("Tail stack canary: %X\n", stack->tail_canary);

    if (stack->data && stack->capacity != 0)
    {
        for (size_t idx = 0; idx <= stack->capacity DEBUG(+ CANARY_ELEMS) - 1; idx++)
        {
            if (stack->data[idx] == POISON)
            {
                printf_to_log_file("[%lu] = ", idx);
                printf_to_log_file(FORM_SPEC_STK_EL, stack->data[idx]);
                printf_to_log_file( " (POISON)\n");
            }
            DEBUG(
            else if (stack->data[idx] == TAILCANARY_DATA)
            {
                printf_to_log_file("[%lu] = ", idx);
                printf_to_log_file( FORM_SPEC_STK_EL, stack->data[idx]);
                printf_to_log_file(" (TAIL DATA CANARY)\n");
            }
            else if (stack->data[idx] == FRONTCANARY_DATA)
            {
                printf_to_log_file("[%lu] = ", idx);
                printf_to_log_file(FORM_SPEC_STK_EL, stack->data[idx]);
                printf_to_log_file(" (FRONT DATA CANARY)\n");
            })
            else
            {
                printf_to_log_file("^[%lu] = ", idx);
                printf_to_log_file( FORM_SPEC_STK_EL, stack->data[idx]);
                printf_to_log_file("\n", idx);
            }
        }
    }
    printf_to_log_file("\n");
}

void update_stack_data_hash(stack_t_t *stack)
{
    stack->djb2_stack = 0;
    stack->djb2_data = 0;
    size_t size = (size_t)(&stack->tail_canary - &stack->front_canary) + sizeof(stack->tail_canary);
    stack->djb2_stack = create_djb2_hash((char *)stack, size);
    stack->djb2_data = create_djb2_hash((char *)stack->data, stack->capacity + CANARY_ELEMS);
}

func_errors stack_push(stack_t_t *stack, stack_elem_t *elem)
{
    func_errors error = NO_MISTAKE_FUNC;
    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        error = error | VERIFY_FAILED;
    if (MY_ASSERT(elem != NULL))
        error = error | FUNC_PARAM_IS_NULL;
    if (error)
        return error;

    if (stack->top + 1 >= stack->capacity)
    {  
        DEBUG(update_stack_data_hash(stack);)
        stack_realloc(stack);
    }
    stack->data[stack -> top] = *elem;
    stack->top++;

    DEBUG(update_stack_data_hash(stack);)

    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        error = error | VERIFY_FAILED;
    return error;
}

func_errors stack_pop(stack_t_t *stack, stack_elem_t *elem)
{
    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        return VERIFY_FAILED;
    if (!elem)
    {
        if (stack->top > 0 DEBUG(+ CANARY_ELEMS / 2))
        {   
            stack->top--;
            stack->data[stack -> top] = POISON;
            DEBUG(update_stack_data_hash(stack);)
        }
        return NO_MISTAKE_FUNC;
    }

    if (stack->top > 0 DEBUG(+ CANARY_ELEMS / 2))
    {   
        stack->top--;
        *elem = stack->data[stack -> top];
        stack->data[stack -> top] = POISON;
    }

    DEBUG(update_stack_data_hash(stack);)

    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        return VERIFY_FAILED;
    return NO_MISTAKE_FUNC;
}

func_errors stack_top(stack_t_t *stack, stack_elem_t *elem)
{
    func_errors error = NO_MISTAKE_FUNC;
    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        error = error | VERIFY_FAILED;
    if (MY_ASSERT(elem != NULL))
        error = error | FUNC_PARAM_IS_NULL;
    if (error)
        return error;

    if (stack->top > 0 DEBUG(+ CANARY_ELEMS / 2))
    {   
        stack->top--;
        *elem = stack->data[stack -> top];
        stack->top++;
    }

    return NO_MISTAKE_FUNC;
}


func_errors stack_realloc(stack_t_t *stack)
{
    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        return VERIFY_FAILED;

    size_t old_capacity = stack->capacity;
    stack->capacity = stack->capacity * 2;

    stack_elem_t* new_data = (stack_elem_t*)realloc(stack->data, (stack->capacity DEBUG(+ CANARY_ELEMS))*sizeof(stack_elem_t));
    if (MY_ASSERT(new_data != NULL))
        return ALLOC_ERROR;

    stack->data = new_data;
    fill_poison(stack, old_capacity DEBUG (+ CANARY_ELEMS) - 1, stack->capacity - 1 DEBUG (+ CANARY_ELEMS / 2));

    DEBUG(
    stack->data[CANARY_ELEMS + stack->capacity - 1] = TAILCANARY_DATA;
    )

    DEBUG(update_stack_data_hash(stack);)

    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        return VERIFY_FAILED;
    return NO_MISTAKE_FUNC;
}


func_errors stack_free(stack_t_t *stack)
{
    if (MY_ASSERT(stack_verify(stack) == NO_MISTAKE))
        return VERIFY_FAILED;

    free(stack->data);
    free(stack);

    return NO_MISTAKE_FUNC;
}