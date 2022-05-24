#include <sys/socket.h>
#include <iostream>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>

using namespace std;

#define MAXSIZE 1024

// struct Head{
//     uint32_t len;//4bytes
//     unsigned char ver[2];//1byte
//     unsigned char cmd[2];//1byte
//     unsigned char body[64];//1byte
// }Head;


int main(int argc, char *argv[])
{
    int ret;
    int port = 9999;
    struct sockaddr_in Srv_Addr;
    struct sockaddr_in Peer_Addr;
    socklen_t Srv_Addr_len = sizeof(Srv_Addr);
    socklen_t Peer_Addr_len = sizeof(Peer_Addr);
    memset(&Srv_Addr, 0, Srv_Addr_len);
    memset(&Peer_Addr, 0, Peer_Addr_len);
    Srv_Addr.sin_family = AF_INET;
    Srv_Addr.sin_port = htons(port);
    Srv_Addr.sin_addr.s_addr = htonl(INADDR_ANY);
    struct epoll_event ev, event[MAXSIZE];
    // if(argc != 2)
    // {
    //     cout << "Input ./TcpServer IP " << endl;
    //     return -1;
    // }
    int  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    cout << "sockfd: " << sockfd << endl;
    int opt = 1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    ret = bind(sockfd, (struct sockaddr*)&Srv_Addr, Srv_Addr_len);
    if(ret == -1)
    {
        cout << "bind error: " << errno << endl;
    }
    else
    {
        cout << "bind succ: " << endl;
    }

    ret = listen(sockfd, MAXSIZE);
    if(ret == -1)
    {
        cout << "listen error: " << errno << endl;
    }
    else
    {
        cout << "listen succ" << endl;
    }
    int epfd = epoll_create(MAXSIZE);
    if(epfd == -1)
    {
        cout << "epoll_create error: " << errno << endl;
    }
    else
    {
        cout << "epol_create succ" << endl;
    }

    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
    if(ret == -1)
    {
        cout << "epoll_ctl error: " << errno << endl;
    }

    while(1)
    {
        int Count = epoll_wait(epfd, event, MAXSIZE, -1);
        for(int i = 0;i < Count;i++)
        {
            char recv[MAXSIZE];
            char send[MAXSIZE];
            int curfd = event[i].data.fd;
            if(curfd == sockfd)
            {
                int newfd = accept(curfd, (struct sockaddr*)&Peer_Addr, &Peer_Addr_len);
                cout << "newfd: " << newfd << endl;
                ev.events = EPOLLIN;
                ev.data.fd = newfd;
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, newfd, &ev);
                if(ret == -1)
                {
                    cout << "epoll_ctl error: " << errno << endl;
                }
            }
            else if(event[i].events & EPOLLIN)
            {
                memset(recv, 0, MAXSIZE);
                // ret = recvfrom(curfd, recv, MAXSIZE, 0, (struct sockaddr*)&Peer_Addr, &Peer_Addr_len);
                ret = read(curfd, recv, MAXSIZE);
                if(ret <= 0)
                {
                    // cout << "recvfrom error: " << errno << endl;
                    cout << "client disconnect " << endl;
                    struct epoll_event event_del;
                    event_del.data.fd = curfd;
                    event_del.events = 0;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, &event_del);
                }
                else
                {
                    cout << "recieve from client: " << recv << endl;
                    ev.events = EPOLLOUT;
                    ev.data.fd = curfd;
                    epoll_ctl(epfd, EPOLL_CTL_MOD, curfd, &ev);
                }
            }
            else if(event[i].events & EPOLLOUT)
            // else if((curfd != sockfd) & EPOLLOUT)
            {
                // memset(send, 0, MAXSIZE);
                // ret = sendto(curfd, recv, sizeof(recv), 0, (struct sockaddr*)&Peer_Addr, Peer_Addr_len);
                ret = write(curfd, recv, sizeof(recv));
                if(ret <= 0)
                {
                    struct epoll_event event_del;
                    cout << "sendto error: " << errno << endl;
                    event_del.data.fd = curfd;
                    event_del.events = 0;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, &event_del);
                }
                // else if(ret > 0)
                else
                {
                    cout << "sendto client: " << recv << endl;
                    ev.events = EPOLLIN;
                    ev.data.fd = curfd;
                    epoll_ctl(epfd, EPOLL_CTL_MOD, curfd, &ev);
                }
                // else
                // {
                //     continue;
                // }
            }
            else
            {
                cout << "epoll error: " << endl;
                epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, &event[i]);
            }
        }
    }
}