#include "log.h"
#include "handlers.h"
#include <stdio.h>

void stack_work()
{
    size_t stack = stack_ctor_handle(3, sizeof(int));

    int value = 1;
    stack_push_handle(stack, &value);

    value = 2;
    stack_push_handle(stack, &value);

    value = 3;
    stack_push_handle(stack, &value);

    value = 4;
    stack_push_handle(stack, &value);

    int pop_value = 0;
    stack_pop_handle(stack, &pop_value);
    printf("pop_value = %d", pop_value);

    pop_value = 0;
    stack_top_handle(stack, &pop_value);
    printf("pop_value = %d", pop_value);

    stack_dump_handle(stack);

    stack_free_handle(stack);
}

void mistakes_at_stack(){

    size_t stack1 = stack_ctor_handle(3, NULL);
    size_t stack11 = stack_ctor_handle(-6, sizeof(int));

    size_t stack2 = stack_ctor_handle(3, sizeof(int));
    stack_push_handle(stack2, NULL);

    int s = 1;
    stack_push_handle(stack2, &s);

    stack_pop_handle(stack2, NULL);

    stack_pop_handle(NULL, NULL);

    stack_free_handle(stack1);
    stack_free_handle(stack11);
    stack_free_handle(stack2);

}


int main()
{   
    open_log_file("mist.log");

    stack_work();

    mistakes_at_stack();

    return 0;
}

// ./stack