// build in visual studio preview
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <windows.h>

#pragma comment (lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "80"
#define IP4ADDR "127.0.0.1"
char recvbuf[DEFAULT_BUFLEN];
void error() {
    wchar_t* s = NULL;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&s, 0, NULL);
    MessageBox(NULL, s, L"socket error", 1);
    LocalFree(s);
    WSACleanup();
}
static SOCKET ConnectSocket = INVALID_SOCKET;
int __cdecl converse(char* msg, size_t msglen)
{
    if (send(ConnectSocket, msg, msglen, 0) == -1) {
        closesocket(ConnectSocket);
        error();
        return 1;
    }
    (void)recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
    MessageBoxA(NULL, recvbuf, "server say", 0);
    return 0;
}
#define ID_EDIT 1
#define ID_BUTTON 2

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
) {
    MSG  msg;
    WNDCLASSW wc = { 0 };
    wc.lpszClassName = L"Edit control";
    wc.hInstance = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    RegisterClassW(&wc);
    CreateWindowW(wc.lpszClassName, L"Edit client",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        220, 220, 500, 500, 0, 0, hInstance, 0);
    WSADATA wsaData;
    int iResult;
    struct addrinfo* result = NULL;
    struct addrinfo hints;
    int iSendResult = 0;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        error();
        return 1;
    }
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    //2001:b011:7c06:8bc6:8825:51f6:aeaf:3514
    // 59.115.13.210
    iResult = getaddrinfo(IP4ADDR, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        error();
        return 1;
    }
    freeaddrinfo(result);
    for (struct addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            continue;
        }
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam) {
    static HWND hwndEdit;
    HWND hwndButton;
    switch (msg) {
    case WM_CREATE:
        hwndEdit = CreateWindowW(L"Edit", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            30, 20, 420, 250, hwnd, (HMENU)ID_EDIT,
            NULL, NULL);
        hwndButton = CreateWindowW(L"button", L"send",
            WS_VISIBLE | WS_CHILD, 200, 300, 100, 25,
            hwnd, (HMENU)ID_BUTTON, NULL, NULL);
        break;
    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED) {
            int len = GetWindowTextLengthA(hwndEdit);
            char* text = new char[len + 1];
            GetWindowTextA(hwndEdit, text, len);
            converse(text, (size_t)len);
            delete[] text;
        }
        break;
    case WM_DESTROY:
        shutdown(ConnectSocket, SD_BOTH);
        closesocket(ConnectSocket);
        WSACleanup();
        PostQuitMessage(0);
        break;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
