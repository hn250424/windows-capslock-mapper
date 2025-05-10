#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "constants/app_constants.h"
#include "constants/key_constants.h"
#include "constants/result_constants.h"
#include "utils/common.h"
#include "utils/env.h"
#include "utils/mutex.h"

#include "process.h"

int on_runner() {
    int response = is_mutex_exist(MUTEX_KEY_RUNNER);
    if (response) {
        printf("Program is already running.\n");
    } else {
        char path[MAX_PATH];
        DWORD result = GetModuleFileName(NULL, path, MAX_PATH);
        if (result == 0) {
            printf("Failed to get the executable path.\n");
            return FAIL;
        }

        *strstr(path, APP_CMD) = '\0';
        strcat(path, APP_RUNNER);

        char command[MAX_PATH + 20];
        snprintf(command, sizeof(command), "start /B %s", path);
        system(command);

        printf("Program is running...\n");
    }

    return SUCCESS;
}

int off_runner() {
    char command[MAX_PATH + 20];
    snprintf(command, sizeof(command), "taskkill /F /IM %s >nul 2>&1", APP_RUNNER);
    system(command);
    printf("Program has stopped\n");
    return SUCCESS;
}

int show_status() {
    // 1. Check Mutex.
    int mutex_response = is_mutex_exist(MUTEX_KEY_RUNNER);
    printf("[Mutex] %s\n", mutex_response ? "Program is running." : "Program is not running.");

    // 2. Check Env.
    // Get env path.
    HKEY hKey_env;
    DWORD size = 0;
    char* envPath;

    int envPathResult = get_env_path(&hKey_env, &size, &envPath);
    if (envPathResult != SUCCESS) {
        if (envPathResult == ERR_REG_KEY_OPEN) printf("Failed to open registry key.\n");
        else if (envPathResult == ERR_ENV_QUERY_SIZE) printf("Failed to query size of environment variable.\n");
        else if (envPathResult == ERR_MEMORY_ALLOCATION) printf("Memory allocation failed.\n");
        else if (envPathResult == ERR_GET_ENVIRONMENT_VAR) printf("Failed to get the current environment variable.\n");

        RegCloseKey(hKey_env);
        free(envPath);

        return envPathResult;
    }

    // Get Exe dir path.
    char exeDirPath[MAX_PATH];
    int currentPathResult = get_current_path(exeDirPath, APP_CMD);
    
    if (currentPathResult != SUCCESS) {
        if  (currentPathResult == ERR_GET_EXE_PATH) printf("Failed to get the executable path.\n");
        else if (currentPathResult == ERR_CUTPOINT_NOT_FOUND) printf("Failed to find cut pointer.\n");

        free(envPath);
        RegCloseKey(hKey_env);

        return currentPathResult;
    } else {
        if (strstr(envPath, exeDirPath) == NULL) printf("[env] %s\n", "not exist env.");
        else printf("[env] %s\n", "Exist env.");

        free(envPath);
        RegCloseKey(hKey_env);
    }

    // 3. Check Registry.
    HKEY hKey_reg;
    LONG regResult = RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH, 0, KEY_QUERY_VALUE, &hKey_reg);
    
    if (regResult == ERROR_SUCCESS) {
        regResult = RegQueryValueEx(hKey_reg, APP_NAME, NULL, NULL, NULL, NULL);
        RegCloseKey(hKey_reg);
        if (regResult == ERROR_SUCCESS) {
            printf("Registry key exists.\n");
        } else {
            printf("Registry key does not exist.\n");
        }
    } else {
        printf("Failed to open registry key.\n");
        return ERR_REG_KEY_OPEN;
    }

    return SUCCESS;
}

int add_env() {
    HKEY hKey;
    DWORD size = 0;
    char* envPath = NULL;

    int envPathResult = get_env_path(&hKey, &size, &envPath);
    if (envPathResult != SUCCESS) {
        if (envPathResult == ERR_REG_KEY_OPEN) printf("Failed to open registry key.\n");
        else if (envPathResult == ERR_ENV_QUERY_SIZE) printf("Failed to query size of environment variable.\n");
        else if (envPathResult == ERR_MEMORY_ALLOCATION) printf("Memory allocation failed.\n");
        else if (envPathResult == ERR_GET_ENVIRONMENT_VAR) printf("Failed to get the current environment variable.\n");

        RegCloseKey(hKey);
        free(envPath);

        return envPathResult;
    }

    char exeDirPath[MAX_PATH];
    int currentPathResult = get_current_path(exeDirPath, APP_CMD);

    if (currentPathResult != SUCCESS) {
        if  (currentPathResult == ERR_GET_EXE_PATH) printf("Failed to get the executable path.\n");
        else if (currentPathResult == ERR_CUTPOINT_NOT_FOUND) printf("Failed to find cut pointer.\n");

        free(envPath);
        RegCloseKey(hKey);

        return currentPathResult;
    } 
    
    if (strstr(envPath, exeDirPath) == NULL) {
        size_t envLen = strlen(envPath);
        if (envLen > 0 && envPath[envLen - 1] != ';') {
            strcat(envPath, ";");
        }
        strcat(envPath, exeDirPath);

        if (RegSetValueEx(hKey, "Path", 0, REG_SZ, (BYTE*)envPath, strlen(envPath) + 1) != ERROR_SUCCESS) {
            printf("Failed to set environment variable.\n");
            free(envPath);
            RegCloseKey(hKey);
            return ERR_SET_ENVIRONMENT_VAR;
        }

        SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_ABORTIFHUNG, 5000, NULL);
        printf("Path updated successfully.\n");
    } else {
        printf("Path already contains this executable's path.\n");
    }
    
    free(envPath);
    RegCloseKey(hKey);
    return SUCCESS;
}

int remove_env() {
    HKEY hKey;
    DWORD size = 0;
    char* envPath = NULL;

    int envPathResult = get_env_path(&hKey, &size, &envPath);
    if (envPathResult != SUCCESS) {
        if (envPathResult == ERR_REG_KEY_OPEN) printf("Failed to open registry key.\n");
        else if (envPathResult == ERR_ENV_QUERY_SIZE) printf("Failed to query size of environment variable.\n");
        else if (envPathResult == ERR_MEMORY_ALLOCATION) printf("Memory allocation failed.\n");
        else if (envPathResult == ERR_GET_ENVIRONMENT_VAR) printf("Failed to get the current environment variable.\n");

        RegCloseKey(hKey);
        free(envPath);

        return envPathResult;
    }

    char exeDirPath[MAX_PATH];
    int currentPathResult = get_current_path(exeDirPath, APP_CMD);

    if (currentPathResult != SUCCESS) {
        if  (currentPathResult == ERR_GET_EXE_PATH) printf("Failed to get the executable path.\n");
        else if (currentPathResult == ERR_CUTPOINT_NOT_FOUND) printf("Failed to find cut pointer.\n");

        free(envPath);
        RegCloseKey(hKey);

        return currentPathResult;
    } 

    char* pathStart = strstr(envPath, exeDirPath);
    if (pathStart != NULL) {
        size_t pathLen = strlen(exeDirPath);
        memmove(pathStart, pathStart + pathLen, strlen(pathStart + pathLen) + 1);

        size_t envLen = strlen(envPath);
        if (envLen > 0 && envPath[envLen - 1] == ';') {
            envPath[envLen - 1] = '\0';  
        }

        if (RegSetValueEx(hKey, "Path", 0, REG_SZ, (BYTE*)envPath, strlen(envPath) + 1) != ERROR_SUCCESS) {
            printf("Failed to set environment variable.\n");
            free(envPath);
            RegCloseKey(hKey);
            return ERR_SET_ENVIRONMENT_VAR;
        }

        SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_ABORTIFHUNG, 5000, NULL);
        printf("Path removed successfully.\n");
    } else {
        printf("Path not found in environment variable.\n");
        return ERR_PATH_NOT_FOUND;
    }

    free(envPath);
    RegCloseKey(hKey);
    return SUCCESS;
}

int add_registry() {
    char exeDirPath[MAX_PATH];
    int currentPathResult = get_current_path(exeDirPath, APP_CMD);

    if (currentPathResult != SUCCESS) {
        if  (currentPathResult == ERR_GET_EXE_PATH) printf("Failed to get the executable path.\n");
        else if (currentPathResult == ERR_CUTPOINT_NOT_FOUND) printf("Failed to find cut pointer.\n");

        return currentPathResult;
    } 

    strcat(exeDirPath, APP_RUNNER);

    HKEY hKey;
    LONG regResult = RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH, 0, KEY_SET_VALUE, &hKey);

    if (regResult == ERROR_SUCCESS) {
        regResult = RegSetValueEx(hKey, APP_NAME, 0, REG_SZ, (BYTE*)exeDirPath, strlen(exeDirPath) + 1);
        RegCloseKey(hKey);
        if (regResult == ERROR_SUCCESS) {
            printf("Startup registration complete: %s will run at boot.\n", APP_RUNNER);
        } else {
            printf("Failed to register in registry.\n");
            return ERR_SET_REGISTRY;
        }
    } else {
        printf("Failed to open registry key.\n");
        return ERR_REG_KEY_OPEN;
    }

    return SUCCESS;
}

int remove_registry() {
    HKEY hKey;
    LONG regResult = RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH, 0, KEY_SET_VALUE, &hKey);

    if (regResult == ERROR_SUCCESS) {
        regResult = RegDeleteValue(hKey, APP_NAME);
        RegCloseKey(hKey);
        if (regResult == ERROR_SUCCESS) {
            printf("Startup registration removed.\n");
        } else {
            printf("Failed to remove or key does not exist.\n");
            return ERR_SET_REGISTRY;
        }
    } else {
        printf("Failed to open registry key.\n");
        return ERR_REG_KEY_OPEN;
    }

    return SUCCESS;
}

int show_version() {
    printf("%s version 1.0.0\n", APP_NAME);
    return SUCCESS;
}

int show_help() {
    printf("Usage:\n");
    printf("  on                    - Start %s\n", APP_RUNNER);
    printf("  off                   - Terminate %s\n", APP_RUNNER);
    printf("  env --add             - Add the current user path to the environment\n");
    printf("  env --remove          - Remove the current user path from the environment\n");
    printf("  registry --add        - Register %s to run on startup (registry)\n", APP_RUNNER);
    printf("  registry --remove     - Remove %s from startup registry\n", APP_RUNNER);
    printf("  status                - Show operating status\n");
    printf("  --version             - Show version info\n");
    printf("  --help                - Show this help message\n");

    return SUCCESS;
}

int show_help_invalid() {
    printf("Invalid command. Use '--help' to see available options.\n");
    return SUCCESS;
}

struct CommandWithOptions commandWithOptions[] = {
    {
        .command = {
            "on",
            on_runner
        },
        .options = {
            { NULL, NULL, NULL }
        }
    },
    {
        .command = {
            "off",
            off_runner
        },
        .options = {
            { NULL, NULL, NULL }
        }
    },
    {
        .command = {
            "status",
            show_status
        },
        .options = {
            { NULL, NULL, NULL }
        }
    },
    {
        .command = {
            "env",
            NULL
        },
        .options = {
            { "--add", "-a", add_env },
            { "--remove", "-r", remove_env },
            { NULL, NULL, NULL }
        }
    },
    {
        .command = {
            "registry",
            NULL
        },
        .options = {
            { "--add", "-a", add_registry },
            { "--remove", "-r", remove_registry },
            { NULL, NULL, NULL }
        }
    },
    {
        .command = {
            NULL,
            NULL
        },
        .options = {
            { "--help", "-h", show_help },
            { "--version", "-v", show_version },
            { NULL, NULL, NULL }
        }
    },
    {
        .command = {
            NULL,
            NULL
        },
        .options = {
            { NULL, NULL, NULL }
        }
    },
};