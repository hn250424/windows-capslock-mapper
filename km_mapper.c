#include <stdio.h>
#include <windows.h>
#include "mutex.h"

int isLeftDown = 0;

void sendMouseLeftDown() {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(INPUT));
}

void sendMouseLeftUp() {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
        if (p->vkCode == VK_CAPITAL) {
            if (wParam == WM_KEYDOWN && ! isLeftDown) {
                sendMouseLeftDown();
                isLeftDown = 1;
                return 1;
            } else if (wParam == WM_KEYUP && isLeftDown) {
                sendMouseLeftUp();
                isLeftDown = 0;
                return 1;
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    FreeConsole();
    
    HANDLE hMutex;

    int response = is_mutex_exist("Global\\KM_MAPPER_MUTEX");
    if (response) {
        // printf("km_mapper is already running.\n");
        return 0;
    } else {
        // printf("km_mapper is not running.\n");
        hMutex = create_global_mutex("Global\\KM_MAPPER_MUTEX");

        if (! hMutex) {
            // printf("Failed to create mutex.\n");
            return 1;
        }
    }

    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!hHook) {
        // printf("Fail to install hook\n");
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hHook);
    close_mutex(hMutex);

    return 0;
}
