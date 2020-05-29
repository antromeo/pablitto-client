#include <iostream>
#include "../include/control_packet.hh"

std::vector<byte_t> packets::ControlPacket::encode_bytes(std::vector<byte_t> bytes){
    uint16_t len_message=bytes.size();
    uint8_t len_message_msb=len_message>>8;
    uint8_t len_message_lsb=len_message;
    bytes.insert(bytes.begin(), len_message_msb);
    bytes.insert(bytes.begin()+1, len_message_lsb);
    return bytes;
}


std::vector<byte_t> packets::ControlPacket::decode_bytes(std::vector<byte_t>& bytes){
    std::vector<byte_t> vec;
    uint8_t len_message_msb=bytes[0];
    uint8_t len_message_lsb=bytes[1];
    uint16_t len_message=((uint16_t) len_message_msb<<8)|len_message_lsb;

    vec.reserve(int(len_message));

    int i=0;
    while(i<int(len_message)) {
        vec.push_back(bytes[i + 2]);
        i++;
    }
    bytes.erase(bytes.begin(), bytes.begin()+len_message+2); //+2 for len_message + 2 byte, lsb e msb
    return vec;
}


byte_t packets::ControlPacket::decode_byte(std::vector<byte_t>& bytes){
    byte_t byte=bytes[0];
    bytes.erase(bytes.begin());
    return byte;
}
