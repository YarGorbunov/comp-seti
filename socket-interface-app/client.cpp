#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#pragma comment (lib, "Ws2_32.lib")
using namespace std;
#define SRV_HOST "localhost" //server name
#define SRV_PORT 1234 //server port
#define CLNT_PORT 1235 //client port
#define BUF_SIZE 64 //buffer size

int main()
{
    //interface realization
    char buff[1024];
    if (WSAStartup(0x0202,(WSADATA *) &buff[0]))
    {
        //Error type for case of not correct loading of library assets
        cout << "Error WSAStartup\n" << WSAGetLastError();
        return -1;
    }
    //client socket creation
    SOCKET s;
    int from_len;
    char buf[BUF_SIZE]={0};
    hostent * hp;
    sockaddr_in clnt_sin, srv_sin;
    //client socket creation
    s = socket(AF_INET, SOCK_STREAM, 0);
    //client parametr defenition
    clnt_sin.sin_family=AF_INET;
    clnt_sin.sin_addr.s_addr = 0;
    clnt_sin.sin_port = htons(CLNT_PORT);
    //binding socket with client adress
    bind(s, (sockaddr*)&clnt_sin, sizeof(clnt_sin));
    //definig=ng server parametrs
    hp = gethostbyname(SRV_HOST);
    srv_sin.sin_port = htons(SRV_PORT);
    srv_sin.sin_family = AF_INET;
    ((unsigned long *)&srv_sin.sin_addr)[0]=((unsigned long **)hp->h_addr_list)[0][0];
    //server connection
    connect(s,(sockaddr*)&srv_sin,sizeof(srv_sin));
    string mst;
    //dialog with server, while client won't say "Bye"
    do
    {
        //getting message from server
        from_len = recv(s,(char *)&buf, BUF_SIZE,0);
        buf[from_len]=0;
        cout << buf << endl;
        //sending message to server
        getline(cin, mst);
        int msg_size = mst.size();
        send (s, (char *)&mst[0], msg_size, 0);
    } while (mst!="Bye");
    //conncetion break
    cout << "exit" << endl;
    closesocket(s);
    return 0;  
}