#include <stdio.h>
#include <windows.h>
#include "mutex.h"

HANDLE create_global_mutex(const char* name) {
    return CreateMutexA(NULL, FALSE, name);
}

int is_mutex_exist(const char* name) {
    HANDLE hMutex = OpenMutexA(SYNCHRONIZE, FALSE, name);
    if (hMutex) {
        CloseHandle(hMutex);
        return 1; 
    }
    return 0;
}

void close_mutex(HANDLE hMutex) {
    if (hMutex) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
    }
}