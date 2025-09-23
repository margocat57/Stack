#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

typedef int stack_elem_t;

size_t SAVE = 10;

// TODO управлять с помощью аргументов командной строки
bool DEBUG = true;

// TODO сделать массив стуктур который принимает масимальные значения каждого из типов и спецификатор формата, возм вставить это в макрос

// TODO добавить ошибки в log file
#define MISTAKE_INFO(stk_ptr, mistake)                                                                              \
    if (DEBUG)                                                                                                      \
    {                                                                                                               \
        fprintf(stderr, "Mistake from file %s function %s and string %d", __FILE__, __PRETTY_FUNCTION__, __LINE__); \
        StackDump(stk_ptr, mistake);                                                                                \
        return mistake;                                                                                             \
    }

struct stack_t_t
{
    stack_elem_t *data;
    size_t ptr;
    size_t capacity;
};

enum StackErr_t
{
    NO_MISTAKE = 0,
    STK_NULL_PTR = 1,
    DATA_NULL_PTR = 2,
    OUT_OF_INDEX = 3,
    INCORR_SIZE = 4,
    VAR_OVERFLOW = 5 // для элементов массива
};

stack_elem_t POISON = -6666;

StackErr_t StackErr(stack_t_t *stk)
{
    if (stk == NULL)
    {
        fprintf(stderr, "Accessing to NULL pointer\n");
        return STK_NULL_PTR;
    }
    else if (stk->ptr > stk->capacity)
    {
        fprintf(stderr, "Trying to get access to the elem out of array\n");
        return OUT_OF_INDEX;
    }
    return NO_MISTAKE;
}

void StackDump(stack_t_t *stk, StackErr_t err)
{
    if (err == NO_MISTAKE)
    {
        return;
    }

    fprintf(stderr, "\n");
    fprintf(stderr, "stack [%p] ", stk);
    if (err == STK_NULL_PTR)
    {
        fprintf(stderr, "BAD!!!");
        return;
    }

    if (err == INCORR_SIZE)
    {
        return;
    }
    fprintf(stderr, "\ncapacity = %lu ", stk->capacity);

    fprintf(stderr, "\nptr = %lu ", stk->ptr);
    if (err == OUT_OF_INDEX)
    {
        fprintf(stderr, "BAD!!!");
    }

    fprintf(stderr, "\ndata [%p] ", stk->data);
    if (err == DATA_NULL_PTR)
    {
        fprintf(stderr, "BAD!!!");
        return;
    }

    for (size_t idx = 1; idx <= stk->capacity; idx++)
    {
        // TODO как правильно работать с типами данных при печати если мы точно не знаем тип
        if (stk->data[idx] != POISON)
        {
            fprintf(stderr, "\n^[%lu] = %d", idx, stk->data[idx]);
        }
        else
        {
            fprintf(stderr, "\n[%lu] = %d (POISON)", idx, stk->data[idx]);
        }
    }
}

StackErr_t StackCtor(stack_t_t *stk, long long int size)
{
    StackErr_t mistake = StackErr(stk);

    if (mistake)
    {
        MISTAKE_INFO(stk, mistake);
    }

    if (size <= 0 || size > LLONG_MAX)
    {
        fprintf(stderr, "Try to set incorrect size of data\n");
        fprintf(stderr, "size = %lld\n", size);
        MISTAKE_INFO(stk, INCORR_SIZE);
    }

    stk->capacity = (size_t)size;
    stk->data = (stack_elem_t *)calloc(stk->capacity + SAVE, sizeof(stack_elem_t *));

    mistake = StackErr(stk);
    if (mistake)
    {
        MISTAKE_INFO(stk, mistake);
    }

    if (stk->data == NULL)
    {
        fprintf(stderr, "Allocation error\n");
        MISTAKE_INFO(stk, DATA_NULL_PTR);
    }

    for (size_t i = 0; i <= stk->capacity; i++)
    {
        stk->data[i] = POISON;
    }

    return NO_MISTAKE;
}

StackErr_t StackPush(stack_t_t *stk, long long int value)
{
    StackErr_t mistake = StackErr(stk);

    if (mistake)
        MISTAKE_INFO(stk, mistake);

    // FIXME работать с разными типами мы сфокусированы на int
    if (value > INT_MAX)
    {
        fprintf(stderr, "Varirable overflow\n");
        MISTAKE_INFO(stk, VAR_OVERFLOW);
    }

    if (stk->ptr == 0)
        stk->ptr = 1;

    stk->data[stk->ptr] = (stack_elem_t)value;

    if (stk->ptr != stk->capacity - 1)
        stk->ptr++;

    mistake = StackErr(stk);

    if (mistake)
        MISTAKE_INFO(stk, mistake);

    return NO_MISTAKE;
}

stack_elem_t StackPop(stack_t_t *stk)
{
    // Вот здесь логика реализации немного смущает
    StackErr_t mistake = StackErr(stk);

    if (mistake)
        MISTAKE_INFO(stk, mistake);

    if (stk->ptr != 1)
        stk->ptr--;

    stack_elem_t elem = stk->data[stk->ptr];
    stk->data[stk->ptr] = POISON;

    mistake = StackErr(stk);

    if (mistake)
        MISTAKE_INFO(stk, mistake);

    return elem;
}

stack_elem_t StackTop(stack_t_t *stk)
{
    StackErr_t mistake = StackErr(stk);

    if (mistake)
        MISTAKE_INFO(stk, mistake);

    return stk->data[stk->ptr];
}

int main()
{
    stack_t_t stk1 = {};

    printf("\n1test_start\n");
    StackErr_t err1 = StackCtor(&stk1, -5);
    if (err1)
        StackDump(&stk1, err1);
    printf("\n1st TESTING SIZE\n");

    printf("\n2test_start\n");
    StackErr_t err2 = StackCtor(NULL, 5);
    if (err2)
        StackDump(NULL, err2);
    printf("\n2nd TESTING NULL_PTR\n");

    StackCtor(&stk1, 5);

    StackPush(&stk1, 1);
    StackPush(&stk1, 2);
    StackPush(&stk1, 3);
    StackPush(&stk1, 4);
    /*
    printf("\n3test_start\n");
    StackErr_t err3 = StackPush(&stk1, 5);
    if (err3)
        StackDump(&stk1, err3);
    printf("\n3rd TESTING_OUT_OF_IDX\n");
    */

    printf("\n4test_start\n");
    StackPop(&stk1);
    StackErr_t err4 = StackPush(&stk1, 100000000000000);
    if (err4)
        StackDump(&stk1, err4);
    printf("\n4th TESTING_BIG_DIGIT\n");

    // int x = 100000000000;
    // printf("%d", x);
    return 0;
}