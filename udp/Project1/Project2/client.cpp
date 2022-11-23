#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <WS2tcpip.h>
#include <vector>
#pragma comment (lib, "Ws2_32.lib")
using namespace std;
#define SRV_PORT 999 //server port
#define BUF_SIZE 256 //buffer size

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

bool equals(sockaddr_in a, sockaddr_in b) {
    if (ntohs(a.sin_port) != ntohs(b.sin_port)) return false;
    char ipstr_a[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET, &a.sin_addr, (PSTR)ipstr_a, sizeof(ipstr_a));
    char ipstr_b[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET, &a.sin_addr, (PSTR)ipstr_b, sizeof(ipstr_b));
    for (int i = 0; i < INET6_ADDRSTRLEN; i++) {
        if (ipstr_a[i] != ipstr_b[i]) return false;
    }
    return true;
}

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

    SOCKET ClntSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (ClntSock == INVALID_SOCKET) {
        cout << "Error initialization socket # " << WSAGetLastError() << endl;
        closesocket(ClntSock);
        WSACleanup();
        system("pause");
        return 1;
    }
    else
        cout << "Client socket initialization is OK" << endl;

    sockaddr_in srvInfo;
    ZeroMemory(&srvInfo, sizeof(srvInfo));


    erStat = inet_pton(AF_INET, "127.0.0.1", &srvInfo.sin_addr);
    if (erStat <= 0) {
        cout << "Error in IP translation to special numeric format" << endl;
        system("pause");
        return 1;
    }
    srvInfo.sin_family = AF_INET;
    srvInfo.sin_port = htons(SRV_PORT); // Ýòî åñëè òû òèïî õî÷åøü ñâîé ïîðò çàäàâàòü

    short packet_size = 0;

    ArrayStudents mst;
    int* result;
    char buf[BUF_SIZE] = { 0 };
    do
    {
        cout << "Your (client) request to server, number of students. If you wish to exit enter negative number: ";
        cin >> mst.students_n;
        if (mst.students_n < 0) break;
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
            cout << "Name: ";
            memcpy((mststr+sizeprev), &mst.students[i].name_n, sizeof(int));
            sizeprev += sizeof(int);
            for (int j = 0; j < mst.students[i].name_n; j++) {
                cout << mst.students[i].name[j];
                memcpy((mststr + sizeprev), &mst.students[i].name[j], sizeof(char));
                sizeprev += sizeof(char);
            }
            cout << endl << "Number of marks: " << mst.students[i].marks_n;
            memcpy((mststr + sizeprev), &mst.students[i].marks_n, sizeof(int));
            sizeprev += sizeof(int);
            for (int j = 0; j < mst.students[i].marks_n; j++) {
                cout << mst.students[i].marks[j];
                memcpy((mststr + sizeprev), &mst.students[i].marks[j], sizeof(int));
                sizeprev += sizeof(int);
            }
            cout << endl;
        }
        for (int i = 0; i < size; i++) {
            cout << mststr[i] << " ";
        }
        packet_size = sendto(ClntSock, (char*)&mststr[0], size, 0, (sockaddr*)&srvInfo, sizeof(srvInfo));

        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
            closesocket(ClntSock);
            WSACleanup();
            system("pause");
            return 1;
        }

        sockaddr_in newInfo; //Ïîñêîëüêó ìû ðàáîòàåì â äåéòàãðàììíûõ ñîêåòàõ, òåõíè÷åñêè ìû ìîæåì ñëóøàòü íå òîëüêî ñåðâåð, íî è êîãî óãîäíî, à ïîòîìó ñîçäà¸ì íîâûé adr, êîòîðûé áóäåò çàïîëíÿòüñÿ äàííûìè îòïðàâèòåëÿ â recvfrom 
        int newInfoSize = sizeof(newInfo); //recvfrom òðåáóåò èìåííî óêàçàòåëü, ïîýòîìó ïðèõîäèòñÿ òàê ÿâíî îáúÿâëÿòü
        result = new int[mst.students_n];
        do {
            packet_size = recvfrom(ClntSock, (char*)result, mst.students_n * sizeof(int), 0, (sockaddr*)&newInfo, &newInfoSize); // !!
        } while (!equals(srvInfo,newInfo));
        if (packet_size == SOCKET_ERROR) {
            cout << "Can't recieve message from Server. Error # " << WSAGetLastError() << endl;
            closesocket(ClntSock);
            WSACleanup();
            system("pause");
            return 1;
        }

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
    system("pause");
    return 0;
}