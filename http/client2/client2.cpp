#include <winsock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define request "GET /json-en.html HTTP/1.0 \r\nHost: www.json.org \r\n\r\n"
#define MAX_PACKET_SIZE 4096

using namespace std;

int main() {
    WSADATA wsData;
    int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);
    if (erStat != 0) {
        cout << "Error WinSock version initializaion # ";
        cout << WSAGetLastError();
        return 1;
    }
    else
        cout << "WinSock initialization is OK" << endl;

    SOCKET ClntSock = socket(AF_INET, SOCK_STREAM, 0);
    if (ClntSock == INVALID_SOCKET) {
        cout << "Error initialization socket # " << WSAGetLastError() << endl;
        closesocket(ClntSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Client socket initialization is OK" << endl;

    // string hostname = "localhost";
    struct addrinfo hints, * res;
    struct in_addr addr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    // новый аналог gethostbyname()
    erStat = getaddrinfo("www.json.org", NULL, &hints, &res);
    if (erStat != 0) {
        cout << "Error getting website information # ";
        cout << WSAGetLastError();
        return 1;
    }
    else
        cout << "WinSock initialization is OK" << endl;

    sockaddr_in* serv_info = (sockaddr_in*)res->ai_addr;

    char ipstr[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serv_info->sin_addr), (PSTR)ipstr, sizeof(ipstr));
    cout << "Commencing connection via address: " << ipstr << endl;

    serv_info->sin_port = htons(80);

    erStat = connect(ClntSock, (sockaddr*)serv_info, sizeof(*serv_info));
    if (erStat != 0) {
        cout << "Error connecting to the website # ";
        cout << WSAGetLastError();
        return 1;
    }
    else
        cout << "Website connection is OK" << endl;

    erStat = send(ClntSock, request, sizeof(request), 0);

    int len = 0;
    char buff[MAX_PACKET_SIZE];
    do
    {
        len = recv(ClntSock, (char*)&buff, MAX_PACKET_SIZE, 0);
        buff[len] = 0;
        cout << buff;
    } while (len != 0);

    if (SOCKET_ERROR == closesocket(ClntSock))
        return -1;
    system("pause");
    return 1;
}