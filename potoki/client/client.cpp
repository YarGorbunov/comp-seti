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
    int name_n;
    char* name;
    int marks_n;
    int* marks;
};

struct ArrayStudents {
    int students_n;
    Student* students;
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
        return 1;
    }
    else
        cout << "Binding socket to Client info is OK" << endl;

    in_addr ip_to_num;
    erStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);
    if (erStat <= 0) {
        cout << "Error in IP translation to special numeric format" << endl;
        return 1;
    }

    sockaddr_in servInfo;
    ZeroMemory(&servInfo, sizeof(servInfo));

    servInfo.sin_family = AF_INET;
    servInfo.sin_addr = ip_to_num;
    servInfo.sin_port = htons(SRV_PORT);

    erStat = connect(ClntSock, (sockaddr*)&servInfo, sizeof(servInfo));
    if (erStat != 0) {
        cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << endl;
        closesocket(ClntSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Connection established SUCCESSFULLY. Ready to send a message to Server"
        << endl;

    short packet_size = 0;

    ArrayStudents mst;
    int* result;
    char buf[BUF_SIZE] = { 0 };
    do
    {
        cout << "Your (client) request to server, number of students. If you wish to exit enter negative number: ";
        cin >> mst.students_n;
        if (mst.students_n < 0) { packet_size = send(ClntSock, (char*)&mst.students_n, sizeof(int), 0); break; }
        size_t size = sizeof(int);
        mst.students = new Student[mst.students_n];
        for (int i = 0; i < mst.students_n; i++) {
            cout << "Enter name ";
            string strname;
            cin >> strname;
            mst.students[i].name_n = strname.size();
            size += sizeof(int) + mst.students[i].name_n;
            mst.students[i].name = new char[mst.students[i].name_n];
            for (int j = 0; j < mst.students[i].name_n; j++) {
                mst.students[i].name[j] = strname[j];
            }

            cout << "Enter number of marks and marks ";
            cin >> mst.students[i].marks_n;
            mst.students[i].marks = new int[mst.students[i].marks_n];
            size += sizeof(int) + sizeof(int) * mst.students[i].marks_n;
            for (int j = 0; j < mst.students[i].marks_n; j++) {
                cin >> mst.students[i].marks[j];
            }
        }
        char* mststr = new char[size];
        memcpy(mststr, &mst.students_n, sizeof(int));
        size_t sizeprev = sizeof(int);
        for (int i = 0; i < mst.students_n; i++) {
            //cout << "Name: ";
            memcpy((mststr + sizeprev), &mst.students[i].name_n, sizeof(int));
            sizeprev += sizeof(int);
            for (int j = 0; j < mst.students[i].name_n; j++) {
                //cout << mst.students[i].name[j];
                memcpy((mststr + sizeprev), &mst.students[i].name[j], sizeof(char));
                sizeprev += sizeof(char);
            }
            //cout << endl << "Number of marks: " << mst.students[i].marks_n;
            memcpy((mststr + sizeprev), &mst.students[i].marks_n, sizeof(int));
            sizeprev += sizeof(int);
            for (int j = 0; j < mst.students[i].marks_n; j++) {
                //cout << mst.students[i].marks[j];
                memcpy((mststr + sizeprev), &mst.students[i].marks[j], sizeof(int));
                sizeprev += sizeof(int);
            }
            //cout << endl;
        }
        /*for (int i = 0; i < size; i++) {
            cout << mststr[i] << " ";
        }*/
        packet_size = send(ClntSock, (char*)&mststr[0], size, 0);

        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
            closesocket(ClntSock);
            WSACleanup();
            return 1;
        }

        result = new int[mst.students_n];

        packet_size = recv(ClntSock, (char*)result, mst.students_n * sizeof(int), 0);
        cout << "Recieved message from server" << endl;
        for (int i = 0; i < mst.students_n; i++) {
            if (result[i] != 0) {
                for (int j = 0; j < mst.students[i].name_n; j++)
                    cout << mst.students[i].name[j];
                cout << " has " << result[i] << " debts" << endl;
            }
        }

    } while (true);

    cout << "exit" << endl;
    closesocket(ClntSock);
    WSACleanup();

    return 0;
}