#include <iostream>
#include <winsock2.h>//��� ������������� �������
#include <string>
#include <windows.h>
#include <WS2tcpip.h>//��� ������������� ����������� ��� ������ � ���������� tcp/ip
#pragma comment (lib, "Ws2_32.lib")//��������� ����������
#include <vector>
using namespace std;
#define SRV_PORT 1234 //server port
#define BUF_SIZE 1024 //buffer size

struct Student
{
    int name_n;
    char* name;
    int marks_n;
    int* marks;
};

struct ArrayStudent
{
    int students_n;
    Student* students;
};

int main() {


    WSADATA wsData;//� ��� ��������� ������������ ������ � ���������� ������� �� ������ ������
    int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);//������� ������� �������, ������ �������� - ������������� ������ � ������� WORD, � ����� ������ ����������� ������ 2, ������������ ���� 2
    if (erStat != 0) {
        cout << "Error WinSock version initializaion # ";
        cout << WSAGetLastError();
        system("pause");
        return 1;
    }
    else
        cout << "WinSock initialization is OK" << endl;

    SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);//������������� ������. AF_INET �������� ��� ���������� ������� ������� IPv4, SOCK_STREAM ��� TCP, �������������� �������� ���� ��������� �������� ��� TCP ��� UDP
    if (ServSock == INVALID_SOCKET) {
        cout << "Error initialization socket # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        system("pause");
        return 1;
    }
    else
        cout << "Server socket initialization is OK" << endl;

    sockaddr_in servInfo;//��������� � ������� ����� ���������� � ������ � ����� �������
    ZeroMemory(&servInfo, sizeof(servInfo));//��������� ������ servInfo

    servInfo.sin_family = AF_INET;//��������� ��������� �������
    in_addr ip_to_num;//� ��������� ����� ����������� ����� �������
    erStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);//����������� ����� 127.0.0.1 � ����� �������� �������� � ���������� ��� � ��������� ip_to_num
    if (erStat <= 0) {
        cout << "Error in IP translation to special numeric format" << endl;
        system("pause");
        return 1;
    }
    servInfo.sin_addr = ip_to_num;//��������� �����
    servInfo.sin_port = htons(SRV_PORT);//���� ������ ����������� ����� ����� ������� htons(), ������� ��������������� ��������� �������� �������� ����� ���� unsigned short � ���������� ������� �������� ��� ��������� TCP/IP

    erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));//����������� ����� � ������ � �����
    if (erStat != 0) {
        cout << "Error Socket binding to server. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        system("pause");
        return 1;
    }
    else
        cout << "Binding socket to Server info is OK" << endl;

    erStat = listen(ServSock, 10);//��������� ��������� ������, 10 - ������������ ���������� ���������, ����������� � �����������, ���� ����� ������������ ������ �������������, �� ��� ����� ��������
    if (erStat != 0) {
        cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        system("pause");
        return 1;
    }
    else {
        cout << "Listening..." << endl;
    }

    ArrayStudent msg;
    int* result;

    sockaddr_in clientInfo;//��������� � ������� ����� ����� �������� ���������� �� ��������������� �������
    ZeroMemory(&clientInfo, sizeof(clientInfo));
    int clientInfo_size = sizeof(clientInfo);

    short packet_size = 0;//� ��� ����������� ����� ������������ ������� ���� ���������� �������� �� �������

    char buf[BUF_SIZE] = { 0 };
    while (true) {
        SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);//accept ��������� �������������� ����� �� ������� � ���������� ���
        if (ClientConn == INVALID_SOCKET) {
            cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
            closesocket(ServSock);
            closesocket(ClientConn);
            WSACleanup();
            system("pause");
            return 1;
        }
        else
            cout << "Connection to a client established successfully" << endl;

        while (true) {
            bool flag = 1;
            packet_size = send(ClientConn, (char*)&flag, sizeof(bool), 0);
            if (packet_size == SOCKET_ERROR) {
                cout << "Can't send message. Error # " << WSAGetLastError() << endl;
                closesocket(ClientConn);
                break;
            }
            int n; //���������� ���������
            packet_size = recv(ClientConn, (char*)&n, sizeof(int), 0);
            if (packet_size == SOCKET_ERROR) {
                cout << "Can't recieve message. Error # " << WSAGetLastError() << endl;
                closesocket(ClientConn);
                break;
            }
            cout << n << endl;
            if (n < 0) break;
            msg.students = new Student[n];
            msg.students_n = n;
            result = new int[n];
            for (int i = 0; i < n; i++) {
                packet_size = recv(ClientConn, (char*)&buf, BUF_SIZE, MSG_PEEK);
                if (packet_size == SOCKET_ERROR) {
                    cout << "Can't recieve message. Error # " << WSAGetLastError() << endl;
                    closesocket(ClientConn);
                    break;
                }
                cout << packet_size << endl;
                char* name = new char[packet_size];
                packet_size = recv(ClientConn, (char*)&name[0], packet_size, 0);
                if (packet_size == SOCKET_ERROR) {
                    cout << "Can't recieve message. Error # " << WSAGetLastError() << endl;
                    closesocket(ClientConn);
                    break;
                }
                msg.students[i].name_n = packet_size;
                msg.students[i].name = name;
                for (int j = 0; j < msg.students[i].name_n; j++)
                    cout << msg.students[i].name[j];
                cout << endl;
                packet_size = recv(ClientConn, (char*)&buf, BUF_SIZE, MSG_PEEK);
                if (packet_size == SOCKET_ERROR) {
                    cout << "Can't recieve message. Error # " << WSAGetLastError() << endl;
                    closesocket(ClientConn);
                    break;
                }
                cout << packet_size << endl;
                int* marks = new int[packet_size/sizeof(int)];
                packet_size = recv(ClientConn, (char*)&marks[0], packet_size, 0);
                if (packet_size == SOCKET_ERROR) {
                    cout << "Can't recieve message. Error # " << WSAGetLastError() << endl;
                    closesocket(ClientConn);
                    break;
                }
                msg.students[i].marks_n = packet_size/sizeof(int);
                msg.students[i].marks = marks;
                for (int j = 0; j < msg.students[i].marks_n; j++)
                    cout << msg.students[i].marks[j] << " ";
                cout << endl;
            }
            if (packet_size == SOCKET_ERROR) {
                cout << "Can't recieve message. Error # " << WSAGetLastError() << endl;
                closesocket(ClientConn);
                break;
            }
            cout << "Message Recieved. " << endl;
            for (int i = 0; i < n; i++) {
                result[i] = 0;
                for (int j = 0; j < msg.students[i].marks_n; j++)
                    if (msg.students[i].marks[j] == 2) result[i]++;
            }

            packet_size = send(ClientConn, (char*)&result[0], sizeof(int) * n, 0);//���������� ����� ���� �������
            if (packet_size == SOCKET_ERROR) {
                cout << "Can't send message. Error # " << WSAGetLastError() << endl;
                closesocket(ClientConn);
                //WSACleanup();
                break;
                system("pause");
                return 1;
            }
        }


        cout << "The Client has disconnected" << endl;
        closesocket(ClientConn);
    }
    system("pause");
    return 0;
}