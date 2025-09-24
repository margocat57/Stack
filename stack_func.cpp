#include <stdlib.h>
#include <limits.h>
#include "structures.h"
#include "work_with_log_file.h"
#include "mistake_information.h"

const stack_elem_t POISON = -6666;
const size_t SAVE = 10;
bool DEBUG_STACK = false;

const char *form_spec_stk_el = "%d";

StackErr_t StackErr(stack_t_t *stk, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);

    if (stk == NULL)
    {
        fprintf(log_file_ptr, "Accessing to NULL pointer\n");
        return STK_NULL_PTR;
    }
    else if (stk->ptr > stk->capacity)
    {
        fprintf(log_file_ptr, "Trying to get access to the elem out of array\n");
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

    for (size_t idx = 1; idx <= stk->capacity; idx++)
    {
        if (stk->data[idx] != POISON)
        {
            fprintf(log_file_ptr, "\n^[%lu] = ", idx);
            fprintf(log_file_ptr, form_spec_stk_el, stk->data[idx]);
        }
        else
        {
            fprintf(log_file_ptr, "\n[%lu] = ", idx);
            fprintf(log_file_ptr, form_spec_stk_el, stk->data[idx]);
            fprintf(log_file_ptr, " (POISON)");
        }
    }
}

StackErr_t StackCtor(stack_t_t *stk, long long int size, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);

    StackErr_t mistake = StackErr(stk, log_file_ptr);
    MISTAKE_INFO(stk, mistake, before, log_file_ptr);

    if ((size <= 0 || size > LLONG_MAX) && DEBUG_STACK)
    {
        fprintf(log_file_ptr, "Try to set incorrect size of data\n");
        fprintf(log_file_ptr, "size = %lld\n", size);
        MISTAKE_INFO(stk, INCORR_SIZE, before, log_file_ptr);
    }

    stk->capacity = (size_t)size;
    stk->data = (stack_elem_t *)calloc(stk->capacity + SAVE, sizeof(stack_elem_t *));

    mistake = StackErr(stk, log_file_ptr);
    MISTAKE_INFO(stk, mistake, after, log_file_ptr);

    if (stk->data == NULL && DEBUG_STACK)
    {
        fprintf(log_file_ptr, "Allocation error\n");
        MISTAKE_INFO(stk, DATA_NULL_PTR, after, log_file_ptr);
    }

    for (size_t i = 0; i <= stk->capacity; i++)
    {
        stk->data[i] = POISON;
    }

    return NO_MISTAKE;
}

StackErr_t StackPush(stack_t_t *stk, long long int value, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);

    StackErr_t mistake = StackErr(stk, log_file_ptr);
    MISTAKE_INFO(stk, mistake, before, log_file_ptr);

    if (stk->ptr == 0)
        stk->ptr = 1;

    stk->data[stk->ptr] = (stack_elem_t)value;

    if (stk->ptr != stk->capacity - 1)
        stk->ptr++;

    mistake = StackErr(stk, log_file_ptr);
    MISTAKE_INFO(stk, mistake, after, log_file_ptr);

    return NO_MISTAKE;
}

StackErr_t StackPop(stack_t_t *stk, stack_elem_t *elem, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);

    StackErr_t mistake = StackErr(stk, log_file_ptr);
    MISTAKE_INFO(stk, mistake, before, log_file_ptr);

    if (elem == NULL && DEBUG_STACK)
    {
        fprintf(log_file_ptr, "NULL POINTER to elem where value is written\n");
        MISTAKE_INFO(stk, VAR_NULL_PTR, before, log_file_ptr);
    }

    if (stk->ptr != 1)
        stk->ptr--;

    *elem = stk->data[stk->ptr];
    stk->data[stk->ptr] = POISON;

    mistake = StackErr(stk, log_file_ptr);
    MISTAKE_INFO(stk, mistake, after, log_file_ptr);

    return NO_MISTAKE;
}

stack_elem_t StackTop(stack_t_t *stk, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);

    StackErr_t mistake = StackErr(stk, log_file_ptr);
    MISTAKE_INFO(stk, mistake, before, log_file_ptr);

    return stk->data[stk->ptr];
}

StackErr_t FreeStack(stack_t_t *stk, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);

    StackErr_t mistake = StackErr(stk, log_file_ptr);
    MISTAKE_INFO(stk, mistake, before, log_file_ptr);

    free(stk->data);
    stk->data = NULL;
    return NO_MISTAKE;
}