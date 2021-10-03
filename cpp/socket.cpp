// compile in visual studio preview
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <locale>
#include <sstream>

using namespace std;
#pragma comment(lib,"ws2_32.lib")
const char* failed = "socket request failed";
#define MY_SIZE 1024
#define MY_FILE "foo.txt"

const char* get_Website(string url, int port) {
    char buffer[MY_SIZE];
    int i = 0;
    locale local;
    WSADATA wsaData;
    SOCKET Socket;
    SOCKADDR_IN SockAddr;
    int lineCount = 0;
    int rowCount = 0;
    struct hostent* host;
    string get_http = "GET / HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
        return "error: failed";

    Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Socket == INVALID_SOCKET) 
        return failed;

    host = gethostbyname(url.c_str());
    if (!host) 
        return failed;

    SockAddr.sin_port = htons(port);
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

    if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0)
        return failed;
 
    send(Socket, get_http.c_str(), strlen(get_http.c_str()), 0);

    int nDataLength;
    FILE* f;
    fopen_s(&f, MY_FILE, "wb");
    if (f == NULL)
        return failed;
    while ((nDataLength = recv(Socket, buffer, MY_SIZE, 0)) > 0) {
        fwrite(buffer, nDataLength, 1, f);
    }
    fclose(f);
    closesocket(Socket);
    WSACleanup();
    return "success";
}
#define ID_EDIT 1
#define ID_BUTTON 2

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_  HINSTANCE hPrevInstance, 
    _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
    MSG  msg;
    WNDCLASSW wc = { 0 };
    wc.lpszClassName = L"Edit control";
    wc.hInstance = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_DESKTOP);
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);

    RegisterClassW(&wc);
    CreateWindowW(wc.lpszClassName, L"Edit control",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        10, 10, 1900, 1100, 0, 0, hInstance, 0);

    while (GetMessage(&msg, NULL, 0, 0)) {

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam) {

    static HWND hwndEdit, textarea, hwndPort;
    HWND hwndButton;

    switch (msg) {

    case WM_CREATE:

        hwndEdit = CreateWindowW(L"Edit", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            20, 20, 180, 20, hwnd, (HMENU)ID_EDIT,
            NULL, NULL);
        hwndPort = CreateWindowW(L"Edit", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            20, 45, 180, 20, hwnd, (HMENU)ID_EDIT,
            NULL, NULL);
        hwndButton = CreateWindowW(L"button", L"GET",
            WS_VISIBLE | WS_CHILD, 50, 75, 80, 25,
            hwnd, (HMENU)ID_BUTTON, NULL, NULL);
        textarea = CreateWindowW(L"Static", L"", 
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            30, 110, 1800, 900,
            hwnd, (HMENU)1, NULL, NULL);
        SetWindowTextA(hwndPort, "80");
        SetWindowTextA(hwndEdit, "www.python.org");
        // www.facebook.com
        // www.google.com
        // ww.stackoverflow.com
        break;

    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED) {
            int len = GetWindowTextLengthA(hwndEdit) + 1;
            char *text = new char[len];
            char buf[11];
            (void)GetWindowTextA(hwndPort, buf, 10);
            (void)GetWindowTextA(hwndEdit, text, len);
            const char* a = get_Website(string(text), atoi(buf));
            if (a != failed) {
                FILE* f;
                fopen_s(&f, MY_FILE, "rb");
                if (f == NULL)
                    goto error;
                char buf[MY_SIZE] = {'\0'};
                fread(buf, MY_SIZE-1, 1, f);
                fclose(f);
                SetWindowTextA(textarea, buf);
                system("notepad " MY_FILE);
            }else {
                char buf[50];
                strerror_s(buf, 50, errno);
                (void)MessageBoxA(NULL, buf, a, 0);
            }
            error:
            delete [] text;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
