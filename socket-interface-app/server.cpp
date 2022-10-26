#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#pragma comment (lib, "Ws2_32.lib")
using namespace std;
#define SRV_PORT 1234 //server port
#define BUF_SIZE 64 //buffer size
int main(){
    char buff[1024];
    if (WSAStartup(0x0202,(WSADATA *) &buff[0]))
    {
        //Error type for case of not correct loading of library assets
        cout << "Error WSAStartup\n" << WSAGetLastError();
        return -1;
    }
    SOCKET s, s_new;
    int from_len;
    char buf[BUF_SIZE] = { 0 };
    sockaddr_in sin, from_sin;
    s = socket(AF_INET, SOCK_STREAM, 0);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(SRV_PORT);
    bind(s, (sockaddr *)&sin, sizeof(sin));
    string msg, msg1;
    listen(s, 10);
    while(1){
        from_len = sizeof(from_sin);
        s_new=accept(s, (sockaddr*)&from_sin,&from_len);
        cout << "new connected client! " << endl;
        msg = "ti kto takoy kuda prishel";
        while (1){
            send(s_new, (char *)&msg[0],msg.size(),0);
            from_len = recv(s_new, (char *)buf, BUF_SIZE,0);
            buf[from_len]=0;
            msg1=(string)buf;
            cout << msg1 << endl;
            if (msg1=="Bye") break;
            getline(cin, msg);
        }
        cout << "client is lost";
        closesocket(s_new);
    }
    return 0;
}