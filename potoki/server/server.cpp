#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <WS2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
//#pragma warning(disable: 4996)
using namespace std;
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define SRV_PORT 1234 //server port
#define BUF_SIZE 64 //buffer size

struct Student
{
    int name_n;
    char* name;
    int marks_n;
    int* marks;
};

struct ArrayStudents
{
    int students_n;
    Student* students;
};

#define PRINTUSERS if (nclients)\
cout << " user on-line " << nclients << endl;\
else cout << "No User on line\n";

DWORD WINAPI ConToClient(LPVOID client_socket);

int nclients = 0;

int main() {

    char buff[1024];

    WSADATA wsData;
    int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);
    if (erStat != 0) {
        cout << "Error WinSock version initializaion # ";
        cout << WSAGetLastError();
        return 1;
    }
    else
        cout << "WinSock initialization is OK" << endl;

    SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);
    if (ServSock == INVALID_SOCKET) {
        cout << "Error initialization socket # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Server socket initialization is OK" << endl;

    sockaddr_in servInfo;
    ZeroMemory(&servInfo, sizeof(servInfo));

    servInfo.sin_family = AF_INET;
    servInfo.sin_addr.s_addr = 0;
    servInfo.sin_port = htons(SRV_PORT);

    erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));
    if (erStat != 0) {
        cout << "Error Socket binding to server. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Binding socket to Server info is OK" << endl;

    erStat = listen(ServSock, 0x100);
    if (erStat != 0) {
        cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else {
        cout << "Listening..." << endl;
    }

    ArrayStudents msg;
    int* result;



    SOCKET ClientConn;
    sockaddr_in clientInfo;
    ZeroMemory(&clientInfo, sizeof(clientInfo));
    int clientInfo_size = sizeof(clientInfo);

    short packet_size = 0;

    char buf[BUF_SIZE] = { 0 };
    while ((ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size))) {
        //SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);
        if (ClientConn == INVALID_SOCKET) {
            cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
            closesocket(ServSock);
            closesocket(ClientConn);
            WSACleanup();
            return 1;
        }
        else
            cout << "Connection to a client established successfully" << endl;
        nclients++;
        cout << "+New Connection!\n";
        PRINTUSERS
            cout << endl;

        DWORD thID;
        CreateThread(NULL, NULL, ConToClient, &ClientConn, NULL, &thID);

        
    }
    return 0;
}

DWORD WINAPI ConToClient(LPVOID ClientConn)
{
    SOCKET my_sock;
    int len;
    my_sock = ((SOCKET*)ClientConn)[0];
    char buf[BUF_SIZE] = { 0 };

    ArrayStudents msg;
    int* result;


    short packet_size = 0;

    while (true) {
        packet_size = recv(my_sock, (char*)&buf, BUF_SIZE, MSG_PEEK);
        if (packet_size == sizeof(int)) break;
        size_t bufsize = packet_size;
        char* msgstr = new char[bufsize];
        packet_size = recv(my_sock, msgstr, bufsize, 0);
        int n;
        memcpy(&n, msgstr, sizeof(int));
        msg.students_n = n;
        size_t sizeprev = sizeof(int);
        msg.students = new Student[msg.students_n];
        for (int i = 0; i < n; i++) {
            memcpy(&msg.students[i].name_n, (msgstr + sizeprev), sizeof(int));
            sizeprev += sizeof(int);
            msg.students[i].name = new char[msg.students[i].name_n];
            memcpy(msg.students[i].name, (msgstr + sizeprev), sizeof(char) * msg.students[i].name_n);
            sizeprev += sizeof(char) * msg.students[i].name_n;
            memcpy(&msg.students[i].marks_n, (msgstr + sizeprev), sizeof(int));
            sizeprev += sizeof(int);
            msg.students[i].marks = new int[msg.students[i].marks_n];
            memcpy(msg.students[i].marks, (msgstr + sizeprev), sizeof(int) * msg.students[i].marks_n);
            sizeprev += sizeof(int) * msg.students[i].marks_n;
        }
        cout << "Number: " << msg.students_n << endl;
        for (int i = 0; i < msg.students_n; i++) {
            cout << "Name: ";
            for (int j = 0; j < msg.students[i].name_n; j++) {
                cout << msg.students[i].name[j];
            }
            cout << endl << "Marks: ";
            for (int j = 0; j < msg.students[i].marks_n; j++) {
                cout << msg.students[i].marks[j] << " ";
            }
            cout << endl;
        }

        result = new int[msg.students_n];
        for (int i = 0; i < msg.students_n; i++) {
            result[i] = 0;
            for (int j = 0; j < msg.students[i].marks_n; j++) {
                if (msg.students[i].marks[j] == 2)
                    result[i]++;
            }
        }


        packet_size = send(my_sock, (char*)result, msg.students_n * sizeof(int), 0);
        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message. Error # " << WSAGetLastError() << endl;
            closesocket(my_sock);
            WSACleanup();
            return 1;
        }
    }

    //packet_size = send(my_sock, (char*)result, msg.students_n * sizeof(int), 0);
    if (packet_size == SOCKET_ERROR) {
        cout << "Can't send message. Error # " << WSAGetLastError() << endl;
        closesocket(my_sock);
        WSACleanup();
        return 1;
    }

    cout << "The Client has disconnected" << endl;
    closesocket(my_sock);

}