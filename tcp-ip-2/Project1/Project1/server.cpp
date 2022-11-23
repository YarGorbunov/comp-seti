#include <iostream>
#include <winsock2.h>//для использования сокетов
#include <string>
#include <windows.h>
#include <WS2tcpip.h>//для использования интерфейсов для работы с протоколом tcp/ip
#pragma comment (lib, "Ws2_32.lib")//системная библиотека
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


    WSADATA wsData;//в эту структуру подгружаются данные о реализации сокетов на данной машине
    int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);//функция запуска сокетов, первый аргумент - запрашиваемая версия в формате WORD, в нашем случае минимальная версия 2, максимальная тоже 2
    if (erStat != 0) {
        cout << "Error WinSock version initializaion # ";
        cout << WSAGetLastError();
        system("pause");
        return 1;
    }
    else
        cout << "WinSock initialization is OK" << endl;

    SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);//инициализация сокета. AF_INET означает что используем систему адресов IPv4, SOCK_STREAM что TCP, необязательный параметр если указываем значение для TCP или UDP
    if (ServSock == INVALID_SOCKET) {
        cout << "Error initialization socket # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        system("pause");
        return 1;
    }
    else
        cout << "Server socket initialization is OK" << endl;

    sockaddr_in servInfo;//структура в которой будет информация о адресе и порте сервера
    ZeroMemory(&servInfo, sizeof(servInfo));//заполняем нулями servInfo

    servInfo.sin_family = AF_INET;//указываем семейство адресов
    in_addr ip_to_num;//в структуре будет закодирован адрес сервера
    erStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);//преобразуем адрес 127.0.0.1 в некое числовое значение и записывает его в структуру ip_to_num
    if (erStat <= 0) {
        cout << "Error in IP translation to special numeric format" << endl;
        system("pause");
        return 1;
    }
    servInfo.sin_addr = ip_to_num;//указываем адрес
    servInfo.sin_port = htons(SRV_PORT);//порт всегда указывается через вызов функции htons(), которая переупаковывает привычное цифровое значение порта типа unsigned short в побайтовый порядок понятный для протокола TCP/IP

    erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));//привязываем сокет к адресу и порту
    if (erStat != 0) {
        cout << "Error Socket binding to server. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        system("pause");
        return 1;
    }
    else
        cout << "Binding socket to Server info is OK" << endl;

    erStat = listen(ServSock, 10);//подрубаем прослушку сокета, 10 - максимальное количество процессов, разрешенных к подключению, Если будет подключаться больше пользователей, то они будут сброшены
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

    sockaddr_in clientInfo;//структура в которую будет позже записана информация по подключившемуся клиенту
    ZeroMemory(&clientInfo, sizeof(clientInfo));
    int clientInfo_size = sizeof(clientInfo);

    short packet_size = 0;//в эту переменнную будет записываться сколько байт информации получили от клиента

    char buf[BUF_SIZE] = { 0 };
    while (true) {
        SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);//accept извлекает подключившийся сокет из очереди и возвращает его
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
            int n; //количество студентов
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

            packet_size = send(ClientConn, (char*)&result[0], sizeof(int) * n, 0);//отправляем новую инфу клиенту
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