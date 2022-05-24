#include <sys/socket.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

using namespace std;

#define MAXSIZE 1024
// #define Srv_Port 9999


// struct Head{
//     uint32_t len;//4bytes
//     unsigned char ver[4];//1byte
//     unsigned char cmd[4];//1byte
//     unsigned char body[64];//1byte
// };


int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        cout << "Input ./TcpClient IP" << endl;
        return -1;
    }
    int ret;
    int port = 9999;
    char rec[MAXSIZE];
    char send[MAXSIZE];
    // struct Head Send_Data;
    // memcpy(Send_Data.len,"",sizeof(""));
    // memcpy(Send_Data.ver, "0x01", sizeof("0x01"));
    // memcpy(Send_Data.cmd, "0x01", sizeof("0x01"));
    // uint16_t Srv_Port = stoi(argv[2]);
    struct sockaddr_in Srv_Addr;
    socklen_t Srv_Addr_len = sizeof(Srv_Addr);
    memset(&Srv_Addr, 0, Srv_Addr_len);
    Srv_Addr.sin_family = AF_INET;
    // Srv_Addr.sin_port = htons(stoi(argv[2]));
    Srv_Addr.sin_port = htons(port);
    Srv_Addr.sin_addr.s_addr = inet_addr(argv[1]);
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd == -1)
    {
        cout << "socket error :" << errno << endl;
    }
    else
    {
        cout << "socket succ" << endl;
    }

    ret = connect(sockfd, (struct sockaddr*)&Srv_Addr, Srv_Addr_len);
    if(ret == -1)
    {
        cout << "connect error: " << errno << endl;
    }
    else
    {
        cout << "connect succ" << endl;
    }
    
    while(1)
    {
        memset(rec, 0, sizeof(rec));
        memset(send, 0, sizeof(send));
        cout << "send to server: " << endl;
        fgets(send, MAXSIZE, stdin);
        if(send[0] == 'q')
        {
            cout << "connection closed" << endl;
            close(sockfd);
            return 0;
        }
        // ret = sendto(sockfd, send, sizeof(send), 0, (struct sockaddr*)&Srv_Addr, Srv_Addr_len);
        ret = write(sockfd, send, sizeof(send));
        if(ret == -1)
        {
            cout << "sendto error: " << errno << endl;
        }
        else
        {
            cout << "send data: " << send  << endl;
        }
        // ret = recvfrom(sockfd, rec, 0, MAXSIZE, 0, NULL, NULL);
        ret = read(sockfd, rec, MAXSIZE);
        if(ret == -1)
        {
            cout << "recvfrom error: " << errno << endl;
        }
        else
        {
            cout << "recieve from server: " << rec << endl;
        }
    }
    close(sockfd);
    return 0;
}
