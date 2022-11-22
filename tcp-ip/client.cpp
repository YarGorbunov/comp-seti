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
    string name;
    int marks[5];
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


    Student* mst;
    int* results;
    char buf[BUF_SIZE]={0};
    do
    {
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
        if (n<0) {
            break;
        } 
        mst=new Student[n];
        results=new int[n];
        for (int i=0;i<n;i++){
            cout << "Enter name, then 5 marks, mark can be integer between 2 and 5 and separated by space ";
            Student* s=new Student;
            cin >> s->name;
            for (int j=0;j<5;j++)
                cin >> s->marks[j];
            mst[i]=*s;
        }
        for (int i=0;i<n;i++){
            cout << mst[i].name << endl;
            packet_size = send(ClntSock, (char*)&mst[i].name, sizeof(mst[i].name), 0);
            for (int j=0;j< 5;j++)
                cout << mst[i].marks[j] << " ";
            packet_size = send(ClntSock, (char*)&mst[i].marks[0], 5*sizeof(int), 0);
        }

        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
            closesocket(ClntSock);
            WSACleanup();
            system("pause");
            return 1;
        }
        
        packet_size = recv(ClntSock, (char*)&results[0], n*sizeof(int), 0);
        if (packet_size == SOCKET_ERROR) {
            cout << "Can't recieve message from Server. Error # " << WSAGetLastError() << endl;
            closesocket(ClntSock);
            WSACleanup();
            system("pause");
            return 1;
        }
        cout << "Recieved message from server."<< endl;
        for (int i=0;i<n;i++){
            if (*(results + i)!=0)
                cout << mst[i].name << " has " << *(results + i) << " debts";
        }

    } while (true);

    cout << "exit" << endl;
    closesocket(ClntSock);
    WSACleanup();

    system("pause");
    return 0;
}