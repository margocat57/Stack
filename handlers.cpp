#include <stdio.h>
#include "handlers.h"
#include "stack.h"
#include "my_assert.h"
#include "stack_func.h"

static stack_t_t* HANDLERS[HANDLER_MAX_SIZE] = {NULL};
static size_t FREE_HANDLE = 0;

static size_t create_handle(stack_t_t* stack){

    if(MY_ASSERT(stack_verify(stack) == NO_MISTAKE)) return 0;
    if(MY_ASSERT(FREE_HANDLE < HANDLER_MAX_SIZE)) return 0;

    while(HANDLERS[FREE_HANDLE]){
        FREE_HANDLE++;
    }

    HANDLERS[FREE_HANDLE] = stack;

    return FREE_HANDLE;
}

static stack_t_t* delete_handle(size_t handle){

    if(MY_ASSERT(handle < HANDLER_MAX_SIZE)) return NULL;

    if(HANDLERS[handle]){
        stack_t_t* stack = HANDLERS[handle];
        HANDLERS[handle] = NULL;
        if(handle <= FREE_HANDLE)
            FREE_HANDLE = handle;
        return stack;
    }
    return NULL;
}

size_t stack_ctor_handle(long long int num_of_elem, long long int size_of_elem){
    stack_t_t* stack = stack_ctor(num_of_elem, size_of_elem);
    return create_handle(stack);
}

void stack_push_handle(size_t handle, const void* elem){
    if(MY_ASSERT(handle < HANDLER_MAX_SIZE)) return;
    if(MY_ASSERT(HANDLERS[handle] != NULL)) return;

    stack_t_t *stack = HANDLERS[handle];
    stack_push(stack, elem);
}

void stack_pop_handle(size_t handle, void* elem){
    if(MY_ASSERT(handle < HANDLER_MAX_SIZE)) return;
    if(MY_ASSERT(HANDLERS[handle] != NULL)) return;

    stack_t_t *stack = HANDLERS[handle];
    stack_pop(stack, elem);
}

void stack_top_handle(size_t handle, void* elem){
    if(MY_ASSERT(handle < HANDLER_MAX_SIZE)) return;
    if(MY_ASSERT(HANDLERS[handle] != NULL)) return;

    stack_t_t *stack = HANDLERS[handle];
    stack_pop(stack, elem);
}

void stack_free_handle(size_t handle){
    if(MY_ASSERT(handle < HANDLER_MAX_SIZE)) return;
    if(MY_ASSERT(HANDLERS[handle] != NULL)) return;

    free_stack(delete_handle(handle));
}

void stack_dump_handle(size_t handle){
    if(MY_ASSERT(handle < HANDLER_MAX_SIZE)) return;
    if(MY_ASSERT(HANDLERS[handle] != NULL)) return;

    stack_t_t *stack = HANDLERS[handle];
    stack_dump(stack);
}
