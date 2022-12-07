#include <winsock2.h>
#include <string>
#include <sstream>
#include <windows.h>
#include <WS2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{
    WSADATA wsData;
    int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);
    if (erStat != 0) {
        cout << "Error WinSock version initializaion # ";
        cout << WSAGetLastError();
        return 1;
    }
    else
        cout << "WinSock initialization is OK" << endl;

    addrinfo* addr = NULL;
    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    erStat = getaddrinfo("127.0.0.1", "8000", &hints, &addr);
    if (erStat != 0) {
        cout << "Gettint address has failed # ";
        cout << WSAGetLastError();
        return 1;
    }

    SOCKET ServSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (ServSock == INVALID_SOCKET) {
        cout << "Error initialization socket # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Server socket initialization is OK" << endl;

    erStat = bind(ServSock, addr->ai_addr, (int)addr->ai_addrlen);
    if (erStat != 0) {
        cout << "Error Socket binding to server. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Binding socket to Server info is OK" << endl;

    erStat = listen(ServSock, SOMAXCONN);
    if (erStat != 0) {
        cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else {
        cout << "Listening..." << endl;
    }

    const int max_client_buffer_size = 1024;
    char buf[max_client_buffer_size];
    long long unsigned int ClntSock = INVALID_SOCKET;

    while (true) {
        ClntSock = accept(ServSock, NULL, NULL);
        if (ClntSock == INVALID_SOCKET) {
            cout << "Can't accept connection. Error # " << WSAGetLastError() << endl;
            closesocket(ServSock);
            WSACleanup();
            return 1;
        }

        erStat = recv(ClntSock, buf, max_client_buffer_size, 0);

        stringstream response;
        stringstream response_body;

        if (erStat == SOCKET_ERROR) {
            cout << "Recieving data failed. Error # " << WSAGetLastError() << endl;
            closesocket(ServSock);
            WSACleanup();
            return 1;
        }
        else if (erStat == 0) {
            cout << "Connection was closed " << endl;
        }
        else if (erStat > 0) {
            buf[erStat] = '\0';

            response_body << "<title>Test C++ HTTP Server</title>\n"
                << "<h1>Test page</h1>\n"
                << "<p>This is body of the test page...</p>\n"
                << "<h2>Request headers</h2>\n"
                << "<pre>" << buf << "</pre>\n"
                << "<em><small>Test C++ Http Server</small></em>\n";

            response << "HTTP/1.1 200 OK\r\n"
                << "Version: HTTP/1.1\r\n"
                << "Content-Type: text/html; charset=utf-8\r\n"
                << "Content-Length: " << response_body.str().length()
                << "\r\n\r\n"
                << response_body.str();

            erStat = send(ClntSock, response.str().c_str(),
                response.str().length(), 0);

            if (erStat == SOCKET_ERROR) {
                cout << "Sending message failed. Error # " << WSAGetLastError() << endl;
                closesocket(ServSock);
                WSACleanup();
                return 1;
            }
            closesocket(ClntSock);
        }
    }

    closesocket(ServSock);
    freeaddrinfo(addr);
    WSACleanup();
    return 0;
}