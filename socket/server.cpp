// build in visual studio preview
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <windows.h>

#pragma comment (lib, "Ws2_32.lib")
#define IP4ADDR "127.0.0.1"
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "80"
char recvbuf[DEFAULT_BUFLEN];
static HWND m=nullptr;

void error() {
    wchar_t* s = NULL;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&s, 0, NULL);
    MessageBox(m, s, L"socket error", 0);
    LocalFree(s);
    WSACleanup();
}
static SOCKET ClientSocket = INVALID_SOCKET;
int __cdecl converse(char* msg, size_t msglen)
{
    if (send(ClientSocket, msg, msglen, 0) == -1) {
        closesocket(ClientSocket);
        error();
        return 1;
    }
    (void)recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
    MessageBoxA(NULL, recvbuf, "client say", 0);
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
    m=CreateWindowW(wc.lpszClassName, L"Edit server",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        500, 500, 500, 500, 0, 0, hInstance, 0);

    WSADATA wsaData;
    int iResult;
    SOCKET ListenSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL;
    struct addrinfo hints;
    int iSendResult = 0;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        MessageBoxA(NULL, "WSAStartup", "error", 0);
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
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        MessageBoxA(NULL, "socket", "error", 0);
        error();
        return 1;
    }
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == -1) {
        freeaddrinfo(result);
        closesocket(ListenSocket);
        MessageBoxA(NULL, "bind", "error", 0);
        error();
        return 1;
    }
    freeaddrinfo(result);
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == -1) {
        closesocket(ListenSocket);
        error();
        return 1;
    }
    ClientSocket = accept(ListenSocket, nullptr, nullptr);
    if (ClientSocket == INVALID_SOCKET) {
        closesocket(ListenSocket);
        MessageBoxA(NULL, "accept", "error", 0);
        error();
        return 1;
    }
    closesocket(ListenSocket);
    sockaddr_in client_info;
    int getlen = 16;
    if (getpeername(ClientSocket, (sockaddr*)&client_info, &getlen) == -1) {
        error();
        return 1;
    }
    char* ip = inet_ntoa(client_info.sin_addr);
    MessageBoxA(m, ip, "IP", 0);
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
            WS_VISIBLE | WS_CHILD, 200, 300, 80, 25,
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
        shutdown(ClientSocket, SD_BOTH);
        closesocket(ClientSocket);
        WSACleanup();
        PostQuitMessage(0);
        break;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
