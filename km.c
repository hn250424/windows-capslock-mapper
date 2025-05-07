#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "mutex.h"

const char APP_CMD[] = "km.exe";
const char APP_RUNNER[] = "km_mapper.exe";
const char APP_SUFFIX[] = "_mapper.exe";
const char APP_EXTENSION[] = ".exe";

const int APP_CMD_LENGTH = 6;
const int APP_RUNNER_LENGTH = 13;
const int APP_CMD_SIZE = 7;
const int APP_RUNNER_SIZE = 14;
const int APP_SUFFIX_LENGTH = 11;
const int APP_SUFFIX_SIZE = 12;
const int APP_EXTENSION_LENGTH = 4;
const int APP_EXTENSION_SIZE = 5;


int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        printf("Invalid command. Use '--help' to see available options.\n");
        return 0;
    }

    char* first = argv[1];
    char* second = argv[2];

    if ( (strcmp(first, "on") == 0) && ! second ) {
        int response = is_mutex_exist("Global\\KM_MAPPER_MUTEX");
        if (response) {
            printf("Program is already running.\n");
        } else {
            system("start /B km_mapper.exe");
            printf("Program is running...\n");
        }
    } else if ( (strcmp(first, "off") == 0) && ! second) {
        system("taskkill /F /IM km_mapper.exe >nul 2>&1");
        printf("Program has stopped");
    } else if ( (strcmp(first, "start") == 0) && (strcmp(second, "--auto") == 0) ) {
        char path[MAX_PATH];
        DWORD result = GetModuleFileName(NULL, path, MAX_PATH);

        if (result == 0) {
            printf("Failed to get the executable path\n");
            return 1;
        }

        char *ext = strrchr(path, '.');
        if (ext != NULL && strcmp(ext, ".exe") == 0) {
            *ext = '\0';
        }
        strcat(path, "_mapper.exe");

        HKEY hKey;
        LONG regResult = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey);

        if (regResult == ERROR_SUCCESS) {
            regResult = RegSetValueEx(hKey, "km", 0, REG_SZ, (BYTE*)path, strlen(path) + 1);
            RegCloseKey(hKey);
            if (regResult == ERROR_SUCCESS) {
                printf("Startup registration complete: km_main.exe will run at boot.\n");
            } else {
                printf("Failed to register in registry.\n");
            }
        } else {
            printf("Failed to open registry key.\n");
        }
    } else if ( (strcmp(first, "start") == 0) && (strcmp(second, "--manual") == 0) ) {
        HKEY hKey;
        LONG regResult = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey);

        if (regResult == ERROR_SUCCESS) {
            regResult = RegDeleteValue(hKey, "km");
            RegCloseKey(hKey);
            if (regResult == ERROR_SUCCESS) {
                printf("Startup registration removed.\n");
            } else {
                printf("Failed to remove or key does not exist.\n");
            }
        } else {
            printf("Failed to open registry key.\n");
        }
    } else if ( (strcmp(first, "env") == 0) && (strcmp(second, "--add") == 0) ) {
        HKEY hKey;
        DWORD size = 0;

        if (RegOpenKeyEx(HKEY_CURRENT_USER, "Environment", 0, KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS) {
            printf("Failed to open registry key.\n");
            return 1;
        }

        if (RegGetValue(hKey, NULL, "Path", RRF_RT_ANY, NULL, NULL, &size) != ERROR_SUCCESS) {
            printf("Failed to query size of environment variable.\n");
            RegCloseKey(hKey);
            return 1;
        }

        char* envPath = (char*)malloc(size + MAX_PATH + 2);
        if (! envPath) {
            printf("Memory allocation failed.\n");
            RegCloseKey(hKey);
            return 1;
        }

        if (RegGetValue(hKey, NULL, "Path", RRF_RT_ANY, NULL, envPath, &size) != ERROR_SUCCESS) {
            printf("Failed to get the current environment variable.\n");
            free(envPath);
            RegCloseKey(hKey);
            return 1;
        }

        char exePath[MAX_PATH];
        DWORD result = GetModuleFileName(NULL, exePath, MAX_PATH);
        if (result == 0) {
            printf("Failed to get the executable path.\n");
            free(envPath);
            RegCloseKey(hKey);
            return 1;
        }

        char* cutPoint = strstr(exePath, APP_CMD);
        *cutPoint = '\0';

        if (strstr(envPath, exePath) == NULL) {
            size_t envLen = strlen(envPath);
            if (envLen > 0 && envPath[envLen - 1] != ';') {
                strcat(envPath, ";");
            }
            strcat(envPath, exePath);

            if (RegSetValueEx(hKey, "Path", 0, REG_SZ, (BYTE*)envPath, strlen(envPath) + 1) != ERROR_SUCCESS) {
                printf("Failed to set environment variable.\n");
                free(envPath);
                RegCloseKey(hKey);
                return 1;
            }

            SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_ABORTIFHUNG, 5000, NULL);
            printf("Path updated successfully.\n");
        } else {
            printf("Path already contains this executable's path.\n");
        }

        free(envPath);
        RegCloseKey(hKey);
    } else if ( (strcmp(first, "env") == 0) && (strcmp(second, "--remove") == 0) ) {
        HKEY hKey;
        DWORD size = 0;

        if (RegOpenKeyEx(HKEY_CURRENT_USER, "Environment", 0, KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS) {
            printf("Failed to open registry key.\n");
            return 1;
        }

        if (RegGetValue(hKey, NULL, "Path", RRF_RT_ANY, NULL, NULL, &size) != ERROR_SUCCESS) {
            printf("Failed to query size of environment variable.\n");
            RegCloseKey(hKey);
            return 1;
        }

        char* envPath = (char*)malloc(size + MAX_PATH + 2);
        if (! envPath) {
            printf("Memory allocation failed.\n");
            RegCloseKey(hKey);
            return 1;
        }

        if (RegGetValue(hKey, NULL, "Path", RRF_RT_ANY, NULL, envPath, &size) != ERROR_SUCCESS) {
            printf("Failed to get the current environment variable.\n");
            free(envPath);
            RegCloseKey(hKey);
            return 1;
        }

        char exePath[MAX_PATH];
        DWORD result = GetModuleFileName(NULL, exePath, MAX_PATH);
        if (result == 0) {
            printf("Failed to get the executable path.\n");
            free(envPath);
            RegCloseKey(hKey);
            return 1;
        }

        char* cutPoint = strstr(exePath, APP_CMD);
        *cutPoint = '\0';

        char* pathStart = strstr(envPath, exePath);
        if (pathStart != NULL) {
            size_t pathLen = strlen(exePath);
            memmove(pathStart, pathStart + pathLen, strlen(pathStart + pathLen) + 1);

            size_t envLen = strlen(envPath);
            if (envLen > 0 && envPath[envLen - 1] == ';') {
                envPath[envLen - 1] = '\0';  
            }

            if (RegSetValueEx(hKey, "Path", 0, REG_SZ, (BYTE*)envPath, strlen(envPath) + 1) != ERROR_SUCCESS) {
                printf("Failed to set environment variable.\n");
                free(envPath);
                RegCloseKey(hKey);
                return 1;
            }

            SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_ABORTIFHUNG, 5000, NULL);
            printf("Path removed successfully.\n");
        } else {
            printf("Path not found in environment variable.\n");
        }

        free(envPath);
        RegCloseKey(hKey);
    } else if ( (strcmp(first, "status") == 0) && ! second) {
        int response = is_mutex_exist("Global\\KM_MAPPER_MUTEX");
        if (response) printf("Program is already running.\n");
        else printf("Program is not running.\n");
    } else if ( (strcmp(first, "--version") == 0) && ! second) {
        printf("km version 1.0.0\n");
    } else if ( (strcmp(first, "--help") == 0) && ! second) {
        printf("Usage:\n");
        printf("  on                    - Start km_mapper.exe\n");
        printf("  off                   - Terminate km_mapper.exe\n");
        printf("  start --auto          - Register km_mapper.exe to run on startup (registry)\n");
        printf("  start --manual        - Remove km_mapper.exe from startup registry\n");
        printf("  env --add             - Add the current user path to the environment\n");
        printf("  env --remove          - Remove the current user path from the environment\n");
        printf("  status                - Show operating status\n");
        printf("  --version             - Show version info\n");
        printf("  --help                - Show this help message\n");
    } else {
        printf("Invalid command. Use '--help' to see available options.\n");
    }

    return 0;
}
