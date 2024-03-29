#include <iostream>
#include "../include/unsuback_packet.hh"



packets::UnsubackPacket::UnsubackPacket(FixedHeader fh) {
    this->fixed_header=fh;
};



packets::UnsubackPacket::UnsubackPacket(FixedHeader fh, uint16_t message_id) {
    this->fixed_header=fh;
    this->message_id=message_id;
};

std::vector<byte_t> packets::UnsubackPacket::boxing() {
    std::vector<byte_t> packet_bytes;
    std::vector<byte_t> body_bytes;
    std::vector<byte_t> bytes;

    uint8_t message_id_msb=message_id>>8;
    uint8_t message_id_lsb=message_id;
    body_bytes.push_back(message_id_msb);
    body_bytes.push_back(message_id_lsb);

    fixed_header.set_remaining_length(body_bytes.size()); //2
    packet_bytes=fixed_header.boxing();

    packet_bytes.insert(packet_bytes.end(), body_bytes.begin(), body_bytes.end());
    return packet_bytes;
}


void packets::UnsubackPacket::unboxing(std::vector<byte_t>& msg) {
    std::vector<byte_t> qos_vector;

    fixed_header.unboxing(msg);

    uint8_t message_id_msb=decode_byte(msg);
    uint8_t message_id_lsb=decode_byte(msg);
    message_id=(message_id_msb<<8)|message_id_lsb;

}

void packets::UnsubackPacket::info(){
    std::cout<<"----INFO_UNSUBACK----"<<std::endl;
    std::cout<<"message_id="<<std::bitset<16>(message_id)<<std::endl;
    std::cout<<"--------"<<std::endl;
}