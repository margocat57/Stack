#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include "mistake_information.h"
#include "stack_func.h"
#include "structures.h"
#include "work_with_log_file.h"
#include "init.h"

int main()
{
#ifdef _DEBUG
    DEBUG_STACK = true;
#endif //_DEBUG

#ifdef _DEBUG
    stack_t_t stk1 = {.info = INIT(stk1)};
#else
    stack_t_t stk1 = {};
#endif //_DEBUG

    FILE *log_file1 = OpenLogFile("mistakes.log");

    // FILE *log_file2 = OpenLogFile(NULL);

    printf("\n1test_start\n");
    StackErr_t err1 = StackCtor(&stk1, -5, log_file1);
    printf("\n1st TESTING SIZE\n");
    printf("%d", DEBUG_STACK);

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
    // вот эта ошибка не тестится так как тип данных в стеке не обязательно целочисленный
    // StackErr_t err4 = StackPush(&stk1, 100000000000000, log_file1);
    // printf("\n4th TESTING_BIG_DIGIT\n");

    FreeStack(&stk1, log_file1);

    CloseLogFile(log_file1);

    return 0;
}

// ./stack