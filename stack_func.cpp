#include <stdlib.h>
#include <limits.h>
#include "structures.h"
#include "work_with_log_file.h"
#include "mistake_information.h"

const stack_elem_t POISON = -6666;
const stack_elem_t CANAREIKA = 3381;
const size_t MEMORY_FOR_CANARY = 2;
bool DEBUG_STACK = false;
const char *FORM_SPEC_STK_EL = "%d";

StackErr_t StackErr(stack_t_t *stk, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);

    if (stk == NULL)
    {
        fprintf(log_file_ptr, "Accessing to NULL pointer\n");
        return STK_NULL_PTR;
    }
    else if (stk->data == NULL && stk->is_memory_alloc)
    {
        fprintf(log_file_ptr, "DATA NULL PTR\n");
        return DATA_NULL_PTR;
    }
    else if (stk->data != NULL && (stk->ptr < 1 || stk->ptr > stk->capacity + 1))
    {
        fprintf(log_file_ptr, "Pointer index out of bounds: %lu (capacity: %lu)\n", stk->ptr, stk->capacity);
        return OUT_OF_INDEX;
    }
    return NO_MISTAKE;
}

void StackDump(stack_t_t *stk, StackErr_t err, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);

    fprintf(log_file_ptr, "\n");

    if (DEBUG_STACK && err != STK_NULL_PTR) // доп проверка если надо использовать функцию просто как для вывода
    {
        fprintf(log_file_ptr, "name of stack with mistake %s\n", stk->info.name);
        fprintf(log_file_ptr, "line where it was inicialized %d\n", stk->info.line);
        fprintf(log_file_ptr, "func where it was inicialized %s\n", stk->info.func);
        fprintf(log_file_ptr, "file where it was inicialized %s\n", stk->info.file);
    }

    fprintf(log_file_ptr, "stack [%p] ", stk);
    if (err == STK_NULL_PTR)
    {
        fprintf(log_file_ptr, "BAD!!!");
        return;
    }

    if (err == INCORR_SIZE)
    {
        return;
    }
    fprintf(log_file_ptr, "\ncapacity = %lu ", stk->capacity);

    fprintf(log_file_ptr, "\nptr = %lu ", stk->ptr);
    if (err == OUT_OF_INDEX)
    {
        fprintf(log_file_ptr, "BAD!!!");
    }

    fprintf(log_file_ptr, "\ndata [%p] ", stk->data);
    if (err == DATA_NULL_PTR)
    {
        fprintf(log_file_ptr, "BAD!!!");
        return;
    }

    for (size_t idx = 0; idx <= stk->capacity + 1; idx++)
    {
        if (stk->data[idx] == POISON)
        {
            fprintf(log_file_ptr, "\n[%lu] = ", idx);
            fprintf(log_file_ptr, FORM_SPEC_STK_EL, stk->data[idx]);
            fprintf(log_file_ptr, " (POISON)");
        }
        else if (stk->data[idx] == CANAREIKA)
        {
            fprintf(log_file_ptr, "\n[%lu] = ", idx);
            fprintf(log_file_ptr, FORM_SPEC_STK_EL, stk->data[idx]);
            fprintf(log_file_ptr, " (CANAREIKA)");
        }
        else
        {
            fprintf(log_file_ptr, "\n^[%lu] = ", idx);
            fprintf(log_file_ptr, FORM_SPEC_STK_EL, stk->data[idx]);
        }
    }
}

StackErr_t FillPoison(stack_t_t *stk, long long int idx, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);
    StackErr_t mistake = NO_MISTAKE;

    if (DEBUG_STACK)
    {
        mistake = StackErr(stk, log_file_ptr);
        MISTAKE_INFO(stk, mistake, before, log_file_ptr);
    }

    if (idx <= 0 || idx > LLONG_MAX)
    {
        fprintf(log_file_ptr, "Try to set incorrect size of data\n");
        fprintf(log_file_ptr, "size = %lld\n", idx);
        MISTAKE_INFO(stk, INCORR_IDX, before, log_file_ptr);
    }

    size_t index = (size_t)idx;

    if (stk->data[index] == CANAREIKA)
    {
        fprintf(log_file_ptr, "Trying to get access to the elem out of array\n");
        MISTAKE_INFO(stk, OUT_OF_INDEX, before, log_file_ptr);
    }

    for (; index <= stk->capacity; index++)
    {
        stk->data[index] = POISON;
    }

    if (DEBUG_STACK)
    {
        mistake = StackErr(stk, log_file_ptr);
        MISTAKE_INFO(stk, mistake, after, log_file_ptr);
    }

    // TODO или лучше return mistake; как более понятнее?
    return NO_MISTAKE;
}

StackErr_t StackCtor(stack_t_t *stk, long long int size, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);
    StackErr_t mistake = NO_MISTAKE;

    if (DEBUG_STACK)
    {
        mistake = StackErr(stk, log_file_ptr);
        MISTAKE_INFO(stk, mistake, before, log_file_ptr);
    }

    if (size <= 0 || size > LLONG_MAX)
    {
        fprintf(log_file_ptr, "Try to set incorrect size of data\n");
        fprintf(log_file_ptr, "size = %lld\n", size);
        MISTAKE_INFO(stk, INCORR_SIZE, before, log_file_ptr);
    }

    stk->capacity = (size_t)size;
    stk->data = (stack_elem_t *)calloc(stk->capacity + MEMORY_FOR_CANARY, sizeof(stack_elem_t));

    if (stk->data == NULL)
    {
        fprintf(log_file_ptr, "Allocation error\n");
        MISTAKE_INFO(stk, DATA_NULL_PTR, after, log_file_ptr);
    }

    stk->data[0] = CANAREIKA;
    stk->data[stk->capacity + 1] = CANAREIKA;

    stk->is_memory_alloc = true;
    stk->ptr = 1;

    FillPoison(stk, 1, log_file_ptr);

    if (DEBUG_STACK)
    {
        mistake = StackErr(stk, log_file_ptr);
        MISTAKE_INFO(stk, mistake, after, log_file_ptr);
    }
    return NO_MISTAKE;
}

StackErr_t MakeDataSizeBigger(stack_t_t *stk, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);
    StackErr_t mistake = NO_MISTAKE;

    if (DEBUG_STACK)
    {
        StackErr_t mistake = StackErr(stk, log_file_ptr);
        MISTAKE_INFO(stk, mistake, before, log_file_ptr);
    }
    size_t capacity_old = stk->capacity;
    stk->capacity = stk->capacity * 2;

    stack_elem_t *new_data = (stack_elem_t *)realloc(stk->data, (stk->capacity + MEMORY_FOR_CANARY) * sizeof(stack_elem_t));
    if (new_data == NULL)
    {
        stk->capacity = capacity_old;
        fprintf(log_file_ptr, "ALLOCATION MISTAKE at reallocation");
        return REALLOC_MISTAKE;
    }
    stk->data = new_data;

    stk->data[capacity_old + 1] = POISON;
    stk->data[stk->capacity + 1] = CANAREIKA;
    FillPoison(stk, capacity_old + 2, log_file_ptr);

    if (DEBUG_STACK)
    {
        mistake = StackErr(stk, log_file_ptr);
        MISTAKE_INFO(stk, mistake, after, log_file_ptr);
    }

    return NO_MISTAKE;
}

StackErr_t StackPush(stack_t_t *stk, long long int value, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);
    StackErr_t mistake = NO_MISTAKE;

    if (DEBUG_STACK)
    {
        StackErr_t mistake = StackErr(stk, log_file_ptr);
        MISTAKE_INFO(stk, mistake, before, log_file_ptr);
    }

    stk->data[stk->ptr] = (stack_elem_t)value;

    if (stk->ptr >= stk->capacity)
    {
        MakeDataSizeBigger(stk, log_file_ptr);
    }

    stk->ptr++;

    if (DEBUG_STACK)
    {
        mistake = StackErr(stk, log_file_ptr);
        MISTAKE_INFO(stk, mistake, after, log_file_ptr);
    }

    return NO_MISTAKE;
}

StackErr_t StackPop(stack_t_t *stk, stack_elem_t *elem, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);
    StackErr_t mistake = NO_MISTAKE;

    if (DEBUG_STACK)
    {
        StackErr_t mistake = StackErr(stk, log_file_ptr);
        MISTAKE_INFO(stk, mistake, before, log_file_ptr);
    }

    if (elem == NULL)
    {
        fprintf(log_file_ptr, "NULL POINTER to elem where value is written\n");
        MISTAKE_INFO(stk, VAR_NULL_PTR, before, log_file_ptr);
    }

    if (stk->ptr != 1)
        stk->ptr--;

    *elem = stk->data[stk->ptr];
    stk->data[stk->ptr] = POISON;

    if (DEBUG_STACK)
    {
        mistake = StackErr(stk, log_file_ptr);
        MISTAKE_INFO(stk, mistake, after, log_file_ptr);
    }

    return NO_MISTAKE;
}

StackErr_t StackTop(stack_t_t *stk, FILE *log_file_ptr, stack_elem_t *elem)
{
    log_file_ptr = GetLogStream(log_file_ptr);

    if (DEBUG_STACK)
    {
        StackErr_t mistake = StackErr(stk, log_file_ptr);
        MISTAKE_INFO(stk, mistake, before, log_file_ptr);
    }

    if (elem == NULL)
    {
        fprintf(log_file_ptr, "NULL POINTER to elem where value is written\n");
        MISTAKE_INFO(stk, VAR_NULL_PTR, before, log_file_ptr);
    }

    *elem = stk->data[stk->ptr];
    return NO_MISTAKE;
}

StackErr_t FreeStack(stack_t_t *stk, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);

    if (DEBUG_STACK)
    {
        StackErr_t mistake = StackErr(stk, log_file_ptr);
        MISTAKE_INFO(stk, mistake, before, log_file_ptr);
    }

    free(stk->data);
    stk->data = NULL;
    return NO_MISTAKE;
}