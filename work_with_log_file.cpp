#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

    fprintf(log_file_ptr, "[%s]:\n", time_str);
}

void CloseLogFile(FILE *log_file_ptr)
{
    if (!log_file_ptr)
        return;

    fclose(log_file_ptr);
}