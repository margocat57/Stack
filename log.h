#ifndef LOG_H
#define LOG_H

void open_log_file(const char *file_name);

void printf_to_log_file(const char* string, ...);

void close_log_file();

#endif //LOG_H