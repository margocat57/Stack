#ifndef MY_ASSERT_H
#define MY_ASSERT_H
#include "mistakes_code.h"

#define MY_ASSERT(condition)   \
    my_assert_func(condition, #condition, __FILE__, __PRETTY_FUNCTION__, __LINE__)

bool my_assert_func(bool condition, const char* str_condition, const char* file, const char* func, int line);
#endif //MY_ASSERT_H