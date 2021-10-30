// this serveris not working when no ascii char in path
// TODO: add url encode & decode to path(big-5)
#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#include <direct.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <csignal>
#include <ctime>
#include <chrono>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
constexpr int CR = 13;
constexpr int LF = 10;
constexpr const char* CRLF = "\r\n";
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN  (6*1024) // 6KB
static_assert(DEFAULT_BUFLEN <= INT_MAX, "send length must be int (overflow)");
#define DEFAULT_PORT "80"
bool send_all(SOCKET socket, const char* ptr, int length)
{
    while (length > 0)
    {
        int i = send(socket, ptr, length, 0);
        if (i < 1) return false;
        ptr += i;
        length -= i;
    }
    return true;
}
void clean(int) {
    WSACleanup();
    puts("catch signal: exit");
    exit(1);
}

static SOCKET ClientSocket = INVALID_SOCKET;
void reporterr(void) {
    char buf[100];
    strerror_s(buf, 100, errno);
    send_all(ClientSocket, "<!DOCTYPE html><html>", 21);
    send_all(ClientSocket, buf, (int)strlen(buf));
    send_all(ClientSocket, "</html>", 7);
}
bool dirExists(const char* path) {
    struct stat info;
    if (stat(path, &info) == 0 && info.st_mode & S_IFDIR) {
        return true;
    }
    return false;
}
static char recvbuf[DEFAULT_BUFLEN];
void listdir(const char* path) {
    send_all(ClientSocket, "<!DOCTYPE html><html><head><meta charset=\"big-5\"></head><body>", 62);
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;
    strcpy_s(recvbuf, DEFAULT_BUFLEN, path);
    strcat_s(recvbuf, DEFAULT_BUFLEN, "\\*");
    if ((hFind = FindFirstFile(recvbuf, &fdFile)) == INVALID_HANDLE_VALUE) {
        reporterr();
    }
    else {
        send_all(ClientSocket, "<ul>", 4);
        char full[256] = {0};
        do
        {
            int len = (int)strlen(fdFile.cFileName);
            send_all(ClientSocket, "<li><a href='", 13);
            strcpy_s(recvbuf, DEFAULT_BUFLEN, "http://localhost/");
            if (GetFullPathNameA(fdFile.cFileName, 256, full, NULL) == 0) {
                reporterr();
                return;
            }
            strcat_s(recvbuf, DEFAULT_BUFLEN, full);
            send_all(ClientSocket, recvbuf, (int)strlen(recvbuf));
            send_all(ClientSocket, "'>", 2);
            send_all(ClientSocket, fdFile.cFileName, len);
            send_all(ClientSocket, "</a></li>", 9);
        } while (FindNextFile(hFind, &fdFile));
        FindClose(hFind);
        send_all(ClientSocket, "</ul></body></html>", 19);
    }
}
int __cdecl main(void)
{
    signal(SIGINT, clean);
    //SetConsoleCP(CP_UTF8);
    WSADATA wsaData;
    int iResult;
    SOCKET ListenSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL;
    struct addrinfo hints = {};
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);
    iResult = listen(ListenSocket, 5);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    const char* path = nullptr;char *ptr = nullptr;
    puts("serving at localhost:" DEFAULT_PORT);
    for (;;) {
        {
            ClientSocket = accept(ListenSocket, NULL, NULL);
            if (ClientSocket == INVALID_SOCKET) {
                printf("accept failed with error: %d\n", WSAGetLastError());
                closesocket(ListenSocket);
                WSACleanup();
                return 1;
            }
            {
                    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                    if (!ctime_s(recvbuf, DEFAULT_BUFLEN, &now))
                        fputs(recvbuf, stdout);
                    else
                        perror("ctime_s");
            }
            socklen_t len;
            struct sockaddr_storage addr;
            char ipstr[INET6_ADDRSTRLEN];
            int port;
            len = sizeof addr;
            getpeername(ClientSocket, (struct sockaddr*)&addr, &len);
            if (addr.ss_family == AF_INET) {
                struct sockaddr_in* s = (struct sockaddr_in*)&addr;
                port = ntohs(s->sin_port);
                inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
            }
            else {
                struct sockaddr_in6* s = (struct sockaddr_in6*)&addr;
                port = ntohs(s->sin6_port);
                inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
            }
            printf(" [%s:%d] ", ipstr, port);
        }
        iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult <= 0) {
            printf("recv=%d; closing", iResult);
            goto RET;
        }
        for (char* ptr = recvbuf; *ptr; ptr += 1) {
            if (isspace(*ptr))
                break;
            putchar(*ptr);
        }
        path=NULL;
        ptr = strstr(recvbuf, CRLF);
        {
            char* p = strchr(recvbuf, '/');
            if (p == NULL) {
                puts("parse Header error");
                fwrite(recvbuf, 1, iResult, stdout);
                goto RET;
            }
            p += 1;
            for (path = p;*p!=' '&&*p!='\t' && *p; p += 1)
                ;
            *p = '\0';
        }
        printf(" <%s>", path);
        if (*path == '\0') {
            path = ".";
        }
        if (ptr == NULL) {
            puts("no CRLF found in first line");
            goto DONE;
        }
        ptr += 1;
        char* m;
        for (;;) {
            m = strchr(ptr, ':');
            if (m == NULL) {
                //puts("----finished----");
                goto DONE;
            }
            for (; ptr != m; ptr += 1) {
                //putchar(*ptr);
            }
            ptr = m + 1;
            //putchar('\t');
            m = strstr(ptr, CRLF);
            if (ptr == NULL) {
                //puts("----finished----");
                goto DONE;
            }
            for (; ptr != m; ptr+=1) {
                //putchar(*ptr);
            }
            m += 2;
            ptr = m;
            //putchar(LF);
        }
    DONE:
        if (!send_all(ClientSocket, "\
HTTP/1.1 200 OK\r\n\
Server : Winsock\r\n\
Accept-Ranges: bytes\r\n\r\n", 59))
        goto RET;
        FILE* f;
        iResult = fopen_s(&f, path, "rb");
        if (f == NULL) {
            if (dirExists(path)) {
                if (!_chdir(path))
                    reporterr();
                listdir(path);
                goto RET;
            }
            reporterr();
            goto RET;
        }
        for (;!feof(f);) {
            iResult = (int)fread(recvbuf, 1, DEFAULT_BUFLEN, f);
            if (!send_all(ClientSocket, recvbuf, iResult))
                goto RET;
        }
        fclose(f);
    RET:
        iResult = shutdown(ClientSocket, SD_BOTH);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
        closesocket(ClientSocket);
    }
    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}