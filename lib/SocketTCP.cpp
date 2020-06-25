#include "../include/SocketTCP.hh"


SocketTCP::SocketTCP(){
    this->ipv4="127.0.0.1";
    this->port=1883;
    this->buffer=new unsigned char[MAX_LEN];
    this->server_address.sin_family = AF_INET; //Ipv4

}


SocketTCP::SocketTCP(std::string ipv4, int port){
this->ipv4=ipv4;
this->port=port;
this->buffer=new unsigned char[MAX_LEN];
this->server_address.sin_family = AF_INET; //Ipv4

}

SocketTCP::~SocketTCP(){ //close connection TCP
    close(this->socket_descriptor);
    delete(buffer);
}

void SocketTCP::closeTCP(){ //close connection TCP
    close(this->socket_descriptor);
}

int SocketTCP::connectionTCP(){
    mutex.lock();
    //setting ip
    int len=ipv4.length();
    char char_ipv4[len+1];
    strcpy(char_ipv4, ipv4.c_str());
    //setting port
    this->server_address.sin_port = htons(port);
    int return_code=-1;

    if ((this->socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        mutex.unlock();
        return -1;
    }

    if(inet_pton(AF_INET, char_ipv4, &server_address.sin_addr)<=0){
        mutex.unlock();
        return -1;
    }

    if (return_code=connect(this->socket_descriptor, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        mutex.unlock();
        return -1;
    }
    mutex.unlock();
    return return_code;
}


ssize_t SocketTCP::sendByteTCP(unsigned char * message, size_t len){
    int error_code;
    socklen_t * error_code_size=reinterpret_cast<socklen_t *>(sizeof(error_code));
    getsockopt(socket_descriptor, SOL_SOCKET, SO_ERROR, &error_code, error_code_size);
    if (error_code!=-1){
        ssize_t num_byte_read=send(socket_descriptor, message, len, 0);
        if (num_byte_read==-1) return -1;
        return num_byte_read;
    }
    return -1;
}

ssize_t SocketTCP::readByteTCP(std::vector<unsigned char>& data){
    mutex.lock();
    ssize_t num_byte_read=0;
    unsigned char * buffer=new unsigned char[MAX_LEN];
    int remaining_len;

    int error_code;
    socklen_t * error_code_size=reinterpret_cast<socklen_t *>(sizeof(error_code));
    getsockopt(socket_descriptor, SOL_SOCKET, SO_ERROR, &error_code, error_code_size);
    if (error_code!=-1){


        num_byte_read=read(this->socket_descriptor, &buffer[0], MAX_LEN);
        if (num_byte_read==-1){
            mutex.unlock();
            delete(buffer);
            return -1;
        }
        else if(num_byte_read>0){
            remaining_len=buffer[1];
            data.resize(2+remaining_len);
            std::copy(buffer, buffer+remaining_len+2, data.begin());
        }
        mutex.unlock();
        delete(buffer);
        return num_byte_read;

    }
    delete(buffer);
    return -1;
}