#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <WS2tcpip.h>
#include <vector>
#pragma comment (lib, "Ws2_32.lib")
using namespace std;
#define SRV_PORT 1234 //server port
#define BUF_SIZE 64 //buffer size

struct Student 
{
    int productid;
    int amount;
};

int main()
{
    //interface realization
    char buff[1024];

    WSADATA wsData;
    int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);
    if (erStat != 0) {
        cout << "Error WinSock version initializaion # ";
        cout << WSAGetLastError();
        system("pause");
        return 1;
    }
    else
        cout << "WinSock initialization is OK" << endl;

    SOCKET ClntSock = socket(AF_INET, SOCK_STREAM, 0);
    if (ClntSock == INVALID_SOCKET) {
        cout << "Error initialization socket # " << WSAGetLastError() << endl;
        closesocket(ClntSock);
        WSACleanup();
        system("pause");
        return 1;
    }
    else
        cout << "Client socket initialization is OK" << endl;
/*
    sockaddr_in clntInfo;
    ZeroMemory(&clntInfo, sizeof(clntInfo));

    clntInfo.sin_family = AF_INET;
    clntInfo.sin_addr.s_addr = 0;
    //clntInfo.sin_port = htons(CLNT_PORT); // Ýòî åñëè òû òèïî õî÷åøü ñâîé ïîðò çàäàâàòü
    clntInfo.sin_port = 0;

    erStat = bind(ClntSock, (sockaddr*)&clntInfo, sizeof(clntInfo));
    if (erStat != 0) {
        cout << "Error Socket binding to client. Error # " << WSAGetLastError() << endl;
        closesocket(ClntSock);
        WSACleanup();
        system("pause");
        return 1;
    }
    else
        cout << "Binding socket to Client info is OK" << endl;
*/
    in_addr ip_to_num;
    erStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);
    if (erStat <= 0) {
        cout << "Error in IP translation to special numeric format" << endl;
        system("pause");
        return 1;
    }

    sockaddr_in servInfo;
    ZeroMemory(&servInfo, sizeof(servInfo));

    servInfo.sin_family = AF_INET;
    servInfo.sin_addr = ip_to_num;
    servInfo.sin_port = htons(SRV_PORT); 

    erStat = connect(ClntSock, (sockaddr*)&servInfo, sizeof(servInfo));//аналог bind и accept только для клиента. Функция connect используется процессом-клиентом для установления связи с сервером.
    if (erStat != 0) {
        cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << endl;
        closesocket(ClntSock);
        WSACleanup();
        system("pause");
        return 1;
    }
    else
        cout << "Connection established SUCCESSFULLY. Ready to send a message to Server"
        << endl;

    short packet_size = 0;


    Student mst;
    float* cost;
    char buf[BUF_SIZE]={0};
    do
    {
        cout << "Your (client) request to server, product id and its amount.If you wish to exit enter negative id: ";
        cin >> mst.productid >> mst.amount;
        if (mst.productid<0){closesocket(ClntSock); return 0;}
        packet_size = send(ClntSock, (char*)&mst, sizeof(mst), 0);

        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
            closesocket(ClntSock);
            WSACleanup();
            system("pause");
            return 1;
        }

        packet_size = recv(ClntSock, (char*)&buf, BUF_SIZE, 0);
        cost = (float*)&buf;
        cout << "Recieved message from server, the cost is: " << *cost << endl;

    } while (*cost >= 0);

    cout << "exit" << endl;
    closesocket(ClntSock);
    WSACleanup();

    system("pause");
    return 0;
}