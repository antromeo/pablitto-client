#include <iostream>
#include "../include/connect_packet.hh"

packets::ConnectPacket::ConnectPacket(FixedHeader fh) {
    this->fixed_header=fh;
};



packets::ConnectPacket::ConnectPacket(FixedHeader fh, string client_id, uint16_t keep_alive) {
    this->fixed_header=fh;
    this->protocol_name="MQTT";
    this->protocol_version=0x04;
    this->clean_session=false;
    this->will_flag=false;
    this->will_qos=0x00;
    this->will_retain=false;
    this->username_flag=false;
    this->password_flag=false;
    this->reserved_bit=false;
    this->keep_alive=keep_alive;
    this->client_id=client_id;
};

packets::ConnectPacket::ConnectPacket(FixedHeader fh, string client_id, string username, vector<byte_t> password, uint16_t keep_alive) {
    this->fixed_header=fh;
    this->protocol_name="MQTT";
    this->protocol_version=0x04;
    this->clean_session=false;
    this->will_flag=false;
    this->will_qos=0x00;
    this->will_retain=false;
    this->username_flag=true;
    this->password_flag=true;
    this->reserved_bit=false;
    this->keep_alive=keep_alive;
    this->client_id=client_id;
    this->username=username;
    this->password=password;
}

packets::ConnectPacket::ConnectPacket(FixedHeader fh, string client_id, string username, vector<byte_t> password, std::string will_topic, std::vector<byte_t> will_message, uint16_t keep_alive) {
    this->fixed_header=fh;
    this->protocol_name="MQTT";
    this->protocol_version=0x04;
    this->clean_session=false;
    this->will_flag=true;
    this->will_qos=0x00;
    this->will_retain=false;
    this->username_flag=true;
    this->password_flag=true;
    this->reserved_bit=false;
    this->keep_alive=keep_alive;
    this->client_id=client_id;
    this->username=username;
    this->password=password;
    this->will_topic=will_topic;
    this->will_message=will_message;
    this->will_retain=false;
}


std::vector<byte_t> packets::ConnectPacket::boxing() {
    std::vector<byte_t> packet_bytes;
    std::vector<byte_t> body_bytes;
    std::vector<byte_t> bytes;
    byte_t flag_bits;


    std:vector<byte_t> str_bytes=encode_string(protocol_name);
    for (byte_t byte:str_bytes) body_bytes.push_back(byte);
    body_bytes.push_back(protocol_version);
    flag_bits=username_flag<<7|password_flag<<6|will_retain<<5|will_qos<<3|will_flag<<2|clean_session<<1;
    body_bytes.push_back(flag_bits);
    uint8_t keep_alive_msb=keep_alive>>8;
    uint8_t keep_alive_lsb=keep_alive;
    body_bytes.push_back(keep_alive_msb);
    body_bytes.push_back(keep_alive_lsb);
    str_bytes=encode_string(client_id);
    for (byte_t byte:str_bytes) body_bytes.push_back(byte);
    if(will_flag) {
        str_bytes=encode_string(will_topic);
        for (byte_t byte:str_bytes) body_bytes.push_back(byte);
        bytes=encode_bytes(will_message);
        for (byte_t byte:bytes) body_bytes.push_back(byte);
    }
    if (username_flag){
        str_bytes=encode_string(username);
        for (byte_t byte:str_bytes) body_bytes.push_back(byte);
    }
    if (password_flag){
        bytes=encode_bytes(password);
        for (byte_t byte: bytes) body_bytes.push_back(byte);
    }
    fixed_header.set_remaining_length(body_bytes.size());
    packet_bytes=fixed_header.boxing();

    packet_bytes.insert(packet_bytes.end(), body_bytes.begin(), body_bytes.end());

    return packet_bytes;

}

void packets::ConnectPacket::unboxing(std::vector<byte_t>& msg){
    fixed_header.unboxing(msg);

    byte_t options;
    protocol_name=decode_string(msg);
    protocol_version=decode_byte(msg);
    options=decode_byte(msg);
    reserved_bit=1&options;
    clean_session=1&(options>>1);
    will_flag=1&(options>>2);
    will_qos=3&(options>>3);
    will_retain=1&(options>>5);
    password_flag=1&(options>>6);
    username_flag=1&(options>>7);
    uint8_t ka_msb=decode_byte(msg);
    uint8_t ka_lsb=decode_byte(msg);
    keep_alive=(ka_msb<<8)|ka_lsb;
    client_id=decode_string(msg);
    if (will_flag) {
        will_topic=decode_string(msg);
        will_message=decode_bytes(msg);
    }
    if (username_flag) username=decode_string(msg);
    if (password_flag) {
        password=decode_bytes(msg);
    }

}



void packets::ConnectPacket::info(){
    std::cout<<"STAMPA FIXED HEADER"<<std::endl;
    fixed_header.to_String();
    std::cout<<"protocol_name="<<protocol_name<<std::endl;
    std::cout<<"protocol_version="<<std::bitset<8>(protocol_version)<<std::endl;
    std::cout<<"clean_session="<<clean_session<<std::endl;
    std::cout<<"will_flag="<<will_flag<<std::endl;
    std::cout<<"will_qos="<<bitset<8>(will_qos)<<std::endl;
    std::cout<<"will retain="<<will_retain<<std::endl;
    std::cout<<"username_flag="<<username_flag<<std::endl;
    std::cout<<"password_flag="<<password_flag<<std::endl;
    std::cout<<"reserverd_bit="<<reserved_bit<<std::endl;
    std::cout<<"keep_alive="<<bitset<16>(keep_alive)<<std::endl;
    std::cout<<"client_id="<<client_id<<std::endl;
    std::cout<<"will_topic="<<will_topic<<std::endl;
    for (byte_t element:will_message)
        std::cout<<"(vector) will_message="<<bitset<8>(element)<<std::endl;
    std::cout<<"username="<<username<<std::endl;
    for (byte_t element:password)
        std::cout<<"(vector) password="<<bitset<8>(element)<<std::endl;

}