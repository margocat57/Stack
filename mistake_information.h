#ifndef MISTAKE_INFORMAION_H
#define MISTAKE_INFORMAION_H
#include "structures.h"

#define MISTAKE_INFO(stk_ptr, mistake, str, log_file_ptr)                                                                 \
    if (DEBUG_STACK && (mistake))                                                                                         \
    {                                                                                                                     \
        PrintMistakeTime(log_file_ptr);                                                                                   \
        fprintf(log_file_ptr, "Mistake from file %s function %s and string %d", __FILE__, __PRETTY_FUNCTION__, __LINE__); \
        StackDump(stk_ptr, mistake, log_file_ptr);                                                                        \
        fprintf(log_file_ptr, "\nData was printed in reason verify failed " #str " using func\n");                        \
        fprintf(log_file_ptr, "\n");                                                                                      \
        if (log_file_ptr != NULL && log_file_ptr != stderr)                                                               \
        {                                                                                                                 \
            fflush(log_file_ptr);                                                                                         \
        }                                                                                                                 \
        return mistake;                                                                                                   \
    }

#endif // MISTAKE_INFORMAION_H