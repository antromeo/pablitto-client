#include <iostream>
#include "../include/connack_packet.hh"

packets::ConnackPacket::ConnackPacket(FixedHeader fh) {
    this->fixed_header=fh;
};



packets::ConnackPacket::ConnackPacket(FixedHeader fh, bool session_present, byte_t return_code) {
    this->fixed_header=fh;
    this->session_present=session_present;
    this->return_code=return_code;
};

std::vector<byte_t> packets::ConnackPacket::boxing() {
    std::vector<byte_t> packet_bytes;
    std::vector<byte_t> body_bytes;
    std::vector<byte_t> bytes;

    body_bytes.push_back(session_present);
    body_bytes.push_back(return_code);

    fixed_header.set_remaining_length(body_bytes.size()); //2
    packet_bytes=fixed_header.boxing();

    packet_bytes.insert(packet_bytes.end(), body_bytes.begin(), body_bytes.end());
    return packet_bytes;

}


void packets::ConnackPacket::unboxing(std::vector<byte_t>& msg) {

    fixed_header.unboxing(msg);

    session_present=decode_byte(msg);

    return_code=decode_byte(msg);

}

void packets::ConnackPacket::info(){
    std::cout<<"----INFO_CONNACK----"<<std::endl;
    std::cout<<"session_present="<<session_present<<std::endl;
    std::cout<<"return_code="<<std::bitset<8>(return_code)<<std::endl; //0x00 is connection accepted
    std::cout<<"--------"<<std::endl;
}

