#ifndef MUTEX_H
#define MUTEX_H

#include <windows.h>

HANDLE create_global_mutex(const char* name);
int is_mutex_exist(const char* name);
void close_mutex(HANDLE hMutex);

#endif