#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void process(char* first, char* second) {
    if ( (strcmp(first, "on") == 0) && ! second ) {
        system("start /B km_main.exe");
    } else if ( (strcmp(first, "off") == 0) && ! second) {
        system("taskkill /F /IM km_main.exe");
    // } else if ( (strcmp(first, "services") == 0) && (strcmp(second, "--add") == 0) ) {
        // char path[MAX_PATH];
        // DWORD result = GetModuleFileName(NULL, path, MAX_PATH);
        
        // if (result == 0) {
        //     printf("Failed to get the executable path\n");
        // }
        
        // int startIdx = strlen(path) - 4;
        // path[startIdx++] = '_';
        // path[startIdx++] = 'm';
        // path[startIdx++] = 'a';
        // path[startIdx++] = 'i';
        // path[startIdx++] = 'n';
        // path[startIdx++] = '.';
        // path[startIdx++] = 'e';
        // path[startIdx++] = 'x';
        // path[startIdx++] = 'e';

        // char commandLine[MAX_PATH + 50];
        // sprintf(commandLine, "sc create km_service binPath= \"%s\" start= auto", path);

        // system(commandLine);
        // system("sc start km_service");

        // printf("add services\n");
    // } else if ( (strcmp(first, "services") == 0) && (strcmp(second, "--remove") == 0) ) {
        // system("sc delete km_service");
        // printf("remove services\n");
    } else if ( (strcmp(first, "start") == 0) && (strcmp(second, "--auto") == 0) ) {
        char path[MAX_PATH];
        DWORD result = GetModuleFileName(NULL, path, MAX_PATH);

        if (result == 0) {
            printf("Failed to get the executable path\n");
            return ;
        }

        int startIdx = strlen(path) - 4;
        strcpy(&path[startIdx], "_main.exe");

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
    } else if ( (strcmp(first, "status") == 0) && ! second) {
        FILE *fp;
        char path[MAX_PATH];
        char buffer[128];

        fp = _popen("tasklist /FI \"IMAGENAME eq km_main.exe\"", "r");
        if (fp == NULL) {
            printf("Failed to run tasklist command.\n");
            return ;
        }

        int isRunning = 0;
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            if (strstr(buffer, "km_main.exe") != NULL) {
                isRunning = 1;
                break ;
            }
        }
        fclose(fp);

        if (isRunning) {
            printf("km_main is running.\n");
        } else {
            printf("km_main is not running.\n");
        }
    } else if ( (strcmp(first, "--version") == 0) && ! second) {
        printf("km version 1.0.0\n");
    } else if ( (strcmp(first, "--help") == 0) && ! second) {
        printf("Usage:\n");
        printf("  on                    - Start km_main.exe\n");
        printf("  off                   - Terminate km_main.exe\n");
        printf("  start --auto          - Register km_main.exe to run on startup (registry)\n");
        printf("  start --manual        - Remove km_main.exe from startup registry\n");
        // printf("  services --add        - Add km_main.exe as a Windows service (placeholder)\n");
        // printf("  services --remove     - Remove Windows service (placeholder)\n");
        printf("  --version             - Show version info\n");
        printf("  --help                - Show this help message\n");
    } else {
        printf("Invalid command. Use '--help' to see available options.\n");
    }
}

int main(int argc, char* argv[]) {
    char* first = argv[1];
    char* second = argv[2];

    process(first, second);

    return 0;
}
