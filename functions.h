#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// Почему я это сделала - потому что размер хэндеров ограничен
#define NOT_MANY_ELEMS_BUT_SECURE true

#if NOT_MANY_ELEMS_BUT_SECURE
    #define stack_ctor_main(num_of_elem, size_of_elem, file, func, line) stack_ctor_handle(num_of_elem, size_of_elem, file, func, line)
    #define stack_push_main(handle, elem)                                stack_push_handle(handle, elem)
    #define stack_pop_main(handle, elem)                                 stack_pop_handle(handle, elem)
    #define stack_top_main(handle, elem)                                 stack_top_handle(handle, elem)
    #define stack_dump_main(handle)                                      stack_dump_handle(handle)
    #define stack_free_main(handle)                                      stack_free_handle(handle)
#else 
    #define stack_ctor_main(num_of_elem, size_of_elem, file, func, line) stack_ctor(num_of_elem, size_of_elem, file, func, line)
    #define stack_push_main(stack, elem)                                 stack_push(stack, elem)
    #define stack_pop_main(stack, elem)                                  stack_pop(stack, elem)
    #define stack_top_main(stack, elem)                                  stack_top(stack, elem)
    #define stack_dump_main(stack)                                       stack_dump(stack)
    #define stack_free_main(stack)                                       stack_free(stack)
#endif //NOT_MANY_ELEMS_BUT_SECURE 

#endif //FUNCTIONS_H