#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>

typedef int stack_elem_t;

size_t SAVE = 10;

// TODO управлять с помощью аргументов командной строки
bool DEBUG = true;

// TODO сделать массив стуктур который принимает масимальные значения каждого из типов и спецификатор формата, возм вставить это в макрос

#define MISTAKE_INFO(stk_ptr, mistake, str, log_file_ptr)                                                                 \
    if (DEBUG && (mistake))                                                                                               \
    {                                                                                                                     \
        PrintMistakeTime(log_file_ptr);                                                                                   \
        fprintf(log_file_ptr, "Mistake from file %s function %s and string %d", __FILE__, __PRETTY_FUNCTION__, __LINE__); \
        StackDump(stk_ptr, mistake, log_file_ptr);                                                                        \
        fprintf(log_file_ptr, "\nData was printed in reason verify failed " #str " using func\n");                        \
        fprintf(log_file_ptr, "\n");                                                                                      \
        if (log_file_ptr != NULL)                                                                                         \
        {                                                                                                                 \
            fflush(log_file_ptr);                                                                                         \
        }                                                                                                                 \
        return mistake;                                                                                                   \
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
    VAR_OVERFLOW = 5,
    VAR_NULL_PTR = 6,
    LOG_FILE_OPEN_ERROR = 7
};

stack_elem_t POISON = -6666;

// FILE *log_file_ptr = NULL;

FILE *GetLogStream(FILE *log_file_ptr)
{
    if (log_file_ptr == NULL)
    {
        fprintf(stderr, "NULL .log file pointer. Info will be outputted to stderr\n");
        return stderr;
    }
    return log_file_ptr;
}

FILE *OpenLogFile(const char *file_name)
{
    if (file_name == NULL)
    {
        fprintf(stderr, "NULL pointer to file name. Function will return pointer to stderr");
        return stderr;
    }
    FILE *log_file_ptr = fopen(file_name, "a+");
    log_file_ptr = GetLogStream(log_file_ptr);

    return log_file_ptr;
}

void PrintMistakeTime(FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);

    time_t now = time(NULL);
    char time_str[20] = {};
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(log_file_ptr, "[%s]: ", time_str);
}

void CloseLogFile(FILE *log_file_ptr)
{
    if (!log_file_ptr)
        return;

    fclose(log_file_ptr);
}

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
        // TODO как правильно работать с типами данных при печати если мы точно не знаем тип
        if (stk->data[idx] != POISON)
        {
            fprintf(log_file_ptr, "\n^[%lu] = %d", idx, stk->data[idx]);
        }
        else
        {
            fprintf(log_file_ptr, "\n[%lu] = %d (POISON)", idx, stk->data[idx]);
        }
    }
}

StackErr_t StackCtor(stack_t_t *stk, long long int size, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);

    StackErr_t mistake = StackErr(stk, log_file_ptr);
    MISTAKE_INFO(stk, mistake, before, log_file_ptr);

    if (size <= 0 || size > LLONG_MAX)
    {
        fprintf(log_file_ptr, "Try to set incorrect size of data\n");
        fprintf(log_file_ptr, "size = %lld\n", size);
        MISTAKE_INFO(stk, INCORR_SIZE, before, log_file_ptr);
    }

    stk->capacity = (size_t)size;
    stk->data = (stack_elem_t *)calloc(stk->capacity + SAVE, sizeof(stack_elem_t *));

    mistake = StackErr(stk, log_file_ptr);
    MISTAKE_INFO(stk, mistake, after, log_file_ptr);

    if (stk->data == NULL)
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

    // FIXME работать с разными типами мы сфокусированы на int
    if (value > INT_MAX)
    {
        fprintf(log_file_ptr, "Varirable overflow\n");
        MISTAKE_INFO(stk, VAR_OVERFLOW, before, log_file_ptr);
    }

    if (stk->ptr == 0)
        stk->ptr = 1;

    stk->data[stk->ptr] = (stack_elem_t)value;

    if (stk->ptr != stk->capacity - 1)
        stk->ptr++;

    mistake = StackErr(stk, log_file_ptr);
    MISTAKE_INFO(stk, mistake, after, log_file_ptr);

    return NO_MISTAKE;
}

stack_elem_t StackPop(stack_t_t *stk, stack_elem_t *elem, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);

    // Вот здесь логика реализации немного смущает
    StackErr_t mistake = StackErr(stk, log_file_ptr);
    MISTAKE_INFO(stk, mistake, before, log_file_ptr);

    if (elem == NULL)
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

    return *elem;
}

stack_elem_t StackTop(stack_t_t *stk, FILE *log_file_ptr)
{
    log_file_ptr = GetLogStream(log_file_ptr);

    StackErr_t mistake = StackErr(stk, log_file_ptr);
    MISTAKE_INFO(stk, mistake, before, log_file_ptr);

    return stk->data[stk->ptr];
}

int main()
{
    stack_t_t stk1 = {};

    FILE *log_file1 = OpenLogFile("mistakes.log");

    // FILE *log_file2 = OpenLogFile(NULL);

    printf("\n1test_start\n");
    StackErr_t err1 = StackCtor(&stk1, -5, log_file1);
    printf("\n1st TESTING SIZE\n");

    printf("\n2test_start\n");
    StackErr_t err2 = StackCtor(NULL, 5, log_file1);
    printf("\n2nd TESTING NULL_PTR\n");

    StackCtor(&stk1, 5, log_file1);
    StackPush(&stk1, 1, log_file1);
    StackPush(&stk1, 2, NULL);
    StackPush(&stk1, 3, log_file1);
    StackPush(&stk1, 4, log_file1);
    /*
    printf("\n3test_start\n");
    StackErr_t err3 = StackPush(&stk1, 5);
    if (err3)
        StackDump(&stk1, err3);
    printf("\n3rd TESTING_OUT_OF_IDX\n");
    */

    printf("\n4test_start\n");
    stack_elem_t x = 0;
    StackPop(&stk1, NULL, log_file1);
    StackErr_t err4 = StackPush(&stk1, 100000000000000, log_file1);
    printf("\n4th TESTING_BIG_DIGIT\n");

    CloseLogFile(log_file1);

    // int x = 100000000000;
    // printf("%d", x);
    return 0;
}