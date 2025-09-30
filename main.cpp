#include "log.h"
#include "stack_func.h"
#include <stdio.h>

void stack_work()
{
    stack_t_t *stack = stack_ctor(3, __FILE__, __PRETTY_FUNCTION__, __LINE__);

    int value = 1;
    stack_push(stack, &value);
    stack_dump(stack);

    value = 2;
    stack_push(stack, &value);
    stack_dump(stack);

    value = 3;
    stack_push(stack, &value);
    stack_dump(stack);

    value = 4;
    stack_push(stack, &value);
    stack_dump(stack);

    int pop_value = 0;
    stack_pop(stack, &pop_value);
    printf("pop_value = %d\n", pop_value);
    stack_dump(stack);

    pop_value = 0;
    stack_top(stack, &pop_value);
    printf("top_value = %d\n", pop_value);
    stack_dump(stack);

    stack_dump(stack);

    stack_free(stack);
}

void mistakes_at_stack()
{
    stack_t_t *stack11 = stack_ctor(-6, __FILE__, __PRETTY_FUNCTION__, __LINE__);

    stack_t_t *stack2 = stack_ctor(3, __FILE__, __PRETTY_FUNCTION__, __LINE__);
    stack_push(stack2, NULL);

    int s = 1;
    stack_push(stack2, &s);

    stack_pop(stack2, NULL);

    stack_push(stack2, &s);

    stack_push(stack2, &s);

    stack_pop(NULL, NULL);

    stack_free(stack11);
    stack_free(stack2);
}

int main()
{
    open_log_file("mist.log");

    stack_work();

    DEBUG(mistakes_at_stack();)

    return 0;
}

// ./stack