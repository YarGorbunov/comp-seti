#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <WS2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
//#pragma warning(disable: 4996)
using namespace std;
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define SRV_PORT 999 //server port
#define BUF_SIZE 256 //buffer size

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


int main() {


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

    SOCKET ServSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (ServSock == INVALID_SOCKET) {
        cout << "Error initialization socket # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        system("pause");
        return 1;
    }
    else
        cout << "Server socket initialization is OK" << endl;

    sockaddr_in servInfo;
    ZeroMemory(&servInfo, sizeof(servInfo));

    servInfo.sin_family = AF_INET;
    servInfo.sin_addr.s_addr = INADDR_ANY; //Äëÿ ïîäðîáíîñòåé https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming
    servInfo.sin_port = htons(SRV_PORT);

    erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));
    if (erStat != 0) {
        cout << "Error Socket binding to server. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        system("pause");
        return 1;
    }
    else
        cout << "Binding socket to Server info is OK" << endl;

    ArrayStudents msg;
    int* result;



    short packet_size = 0;

    char buf[BUF_SIZE] = { 0 };
    while (true) {
        sockaddr_in clientInfo;
        int clientInfo_size = sizeof(clientInfo);

        packet_size = recvfrom(ServSock, (char*)&buf, BUF_SIZE, MSG_PEEK, (sockaddr*)&clientInfo, &clientInfo_size);
        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message. Error # " << WSAGetLastError() << endl;
            closesocket(ServSock);
            WSACleanup();
            system("pause");
            return 1;
        }
        size_t bufsize = packet_size;
        char* msgstr = new char[bufsize];
        packet_size = recvfrom(ServSock, msgstr, bufsize, 0, (sockaddr*)&clientInfo, &clientInfo_size);
        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message. Error # " << WSAGetLastError() << endl;
            closesocket(ServSock);
            WSACleanup();
            system("pause");
            return 1;
        }
        char ipstr[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientInfo.sin_addr, (PSTR)ipstr, sizeof(ipstr));
        cout << "Message was recieved from client: " << ipstr << ":" << ntohs(clientInfo.sin_port) << endl;
        /*for (int i = 0; i < bufsize; i++) {
            cout << msgstr[i];
        }*/

        int n;
        memcpy(&n, msgstr, sizeof(int));
        msg.students_n = n;
        size_t sizeprev = sizeof(int);
        msg.students = new Student[msg.students_n];
        for (int i = 0; i < n; i++) {
            memcpy(&msg.students[i].name_n, (msgstr+sizeprev), sizeof(int));
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
        packet_size = sendto(ServSock, (char*)result, msg.students_n*sizeof(int), 0, (sockaddr*)&clientInfo, sizeof(clientInfo));
        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message. Error # " << WSAGetLastError() << endl;
            closesocket(ServSock);
            WSACleanup();
            system("pause");
            return 1;
        }
    }
    system("pause");
    return 0;
}