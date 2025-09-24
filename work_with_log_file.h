#ifndef WORK_WITH_LOG_FILE_H
#define WORK_WITH_LOG_FILE_H
#include <stdio.h>

FILE *GetLogStream(FILE *log_file_ptr);

FILE *OpenLogFile(const char *file_name);

void PrintMistakeTime(FILE *log_file_ptr);

void CloseLogFile(FILE *log_file_ptr);

#endif // WORK_WITH_LOG_FILE_H