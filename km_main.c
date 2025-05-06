#include <stdio.h>
#include <windows.h>

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

    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!hHook) {
        printf("Fail to install hook\n");
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hHook);

    return 0;
}
