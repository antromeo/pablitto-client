#ifndef __SOCKET_TCP__HH
#define __SOCKET_TCP__HH



#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <bits/stdc++.h>
#include <mutex>
#include "utilities.hh"

class SocketTCP{
private:
    int socket_descriptor=0;
    struct sockaddr_in server_address;
    unsigned char * buffer;
    std::string ipv4;
    int port;
    std::mutex mutex;
public:
    SocketTCP();
    SocketTCP(std::string ipv4, int port);
    ~SocketTCP();
    inline void set_ipv4(std::string ipv4){
        this->ipv4=ipv4;
    }
    inline void set_port(int port){
        this->port=port;
    }
    inline void to_String(){
        std::cout<<"Socket, ip="<<this->ipv4<< " ,porta="<<this->port<<std::endl;
    }

    int connectionTCP();
    void closeTCP();
    ssize_t sendByteTCP(unsigned char * message, size_t len);
    ssize_t readByteTCP(std::vector<unsigned char>& data);
};



#endif