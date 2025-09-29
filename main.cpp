#include "log.h"
#include "handlers.h"
#include "functions.h"
#include <stdio.h>

void stack_work()
{
    size_t stack = stack_ctor_main(3, sizeof(int), __FILE__, __PRETTY_FUNCTION__, __LINE__);

    int value = 1;
    stack_push_main(stack, &value);

    value = 2;
    stack_push_main(stack, &value);

    value = 3;
    stack_push_main(stack, &value);

    value = 4;
    stack_push_main(stack, &value);

    int pop_value = 0;
    stack_pop_main(stack, &pop_value);
    printf("pop_value = %d", pop_value);

    pop_value = 0;
    stack_top_main(stack, &pop_value);
    printf("pop_value = %d", pop_value);

    stack_dump_main(stack);

    stack_free_main(stack);
}

void mistakes_at_stack(){

    size_t stack1 = stack_ctor_main(3, NULL, __FILE__, __PRETTY_FUNCTION__, __LINE__);
    size_t stack11 = stack_ctor_main(-6, sizeof(int), __FILE__, __PRETTY_FUNCTION__, __LINE__);

    size_t stack2 = stack_ctor_main(3, sizeof(int), __FILE__, __PRETTY_FUNCTION__, __LINE__);
    stack_push_main(stack2, NULL);
    
    int s = 1;
    stack_push_main(stack2, &s);

    stack_pop_main(stack2, NULL);

    stack_push_main(stack2, &s);

    stack_push_main(stack2, &s);

    stack_pop_main(NULL, NULL);

    stack_free_main(stack1);
    stack_free_main(stack11);
    stack_free_main(stack2);

}


int main()
{   
    open_log_file("mist.log");

    stack_work();

    mistakes_at_stack();

    return 0;
}

// ./stack