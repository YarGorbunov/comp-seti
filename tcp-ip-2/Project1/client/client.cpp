#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <WS2tcpip.h>
#include <vector>
#pragma comment (lib, "Ws2_32.lib")
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

int main()
{

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


    ArrayStudent mst;
    int* results;
    char buf[BUF_SIZE] = { 0 };
    do
    {
        bool flag;
        packet_size = recv(ClntSock, (char*)&flag, sizeof(bool), 0);
        if (packet_size == SOCKET_ERROR) {
            cout << "Can't recieve message from Server. Error # " << WSAGetLastError() << endl;
            closesocket(ClntSock);
            WSACleanup();
            system("pause");
            return 1;
        }
        cout << "Your (client) request to server, number of students.If you wish to exit enter negative number: ";
        int n;
        cin >> n;
        packet_size = send(ClntSock, (char*)&n, sizeof(int), 0);
        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
            closesocket(ClntSock);
            WSACleanup();
            system("pause");
            return 1;
        }
        if (n < 0) {
            break;
        }
        mst.students_n = n;
        mst.students = new Student[n];
        results = new int[n];
        for (int i = 0; i < n; i++) {
            cout << "Enter name, then number of marks and marks, mark can be integer between 2 and 5 and separated by space ";
            Student s;
            string strname;
            cin >> strname;
            s.name_n = strname.size();
            s.name = new char[s.name_n];
            for (int j = 0; j < s.name_n; j++)
                s.name[j] = strname[j];
            cin >> s.marks_n;
            s.marks = new int[s.marks_n];
            for (int j = 0; j < s.marks_n; j++) {
                cin >> s.marks[j];
            }
            mst.students[i] = s;
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < mst.students[i].name_n; j++)
                cout << mst.students[i].name[j];
            packet_size = send(ClntSock, (char*)&mst.students[i].name[0], mst.students[i].name_n, 0);
            if (packet_size == SOCKET_ERROR) {
                cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
                closesocket(ClntSock);
                WSACleanup();
                system("pause");
                return 1;
            }
            cout << endl << mst.students[i].name_n << " " << mst.students[i].marks_n << endl;
            for (int j = 0; j < mst.students[i].marks_n; j++)
                cout << mst.students[i].marks[j] << " " << endl;
            packet_size = send(ClntSock, (char*)&mst.students[i].marks[0], mst.students[i].marks_n * sizeof(int), 0);
            if (packet_size == SOCKET_ERROR) {
                cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
                closesocket(ClntSock);
                WSACleanup();
                system("pause");
                return 1;
            }
        }

        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
            closesocket(ClntSock);
            WSACleanup();
            system("pause");
            return 1;
        }

        packet_size = recv(ClntSock, (char*)&results[0], n * sizeof(int), 0);
        if (packet_size == SOCKET_ERROR) {
            cout << "Can't recieve message from Server. Error # " << WSAGetLastError() << endl;
            closesocket(ClntSock);
            WSACleanup();
            system("pause");
            return 1;
        }
        cout << "Recieved message from server." << endl;
        for (int i = 0; i < n; i++) {
            if (*(results + i) != 0){
                for (int j = 0; j < mst.students[i].name_n; j++)
                    cout << mst.students[i].name[j];
                cout << " has " << *(results + i) << " debts" << endl;
            }
        }

    } while (true);

    cout << "exit" << endl;
    closesocket(ClntSock);
    WSACleanup();

    system("pause");
    return 0;
}