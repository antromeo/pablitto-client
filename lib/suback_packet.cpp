#include <iostream>
#include "../include/suback_packet.hh"


packets::SubackPacket::SubackPacket(FixedHeader fh) {
    this->fixed_header=fh;
};



packets::SubackPacket::SubackPacket(FixedHeader fh, uint16_t message_id, std::vector<byte_t> return_code_vector) {
    this->fixed_header=fh;
    this->message_id=message_id;
    this->return_code_vector=return_code_vector;
};

std::vector<byte_t> packets::SubackPacket::boxing() {
    std::vector<byte_t> packet_bytes;
    std::vector<byte_t> body_bytes;
    std::vector<byte_t> bytes;

    uint8_t message_id_msb=message_id>>8;
    uint8_t message_id_lsb=message_id;
    body_bytes.push_back(message_id_msb);
    body_bytes.push_back(message_id_lsb);
    for (byte_t b:return_code_vector){
        body_bytes.push_back(b);
    }
    fixed_header.set_remaining_length(body_bytes.size());
    packet_bytes=fixed_header.boxing();

    packet_bytes.insert(packet_bytes.end(), body_bytes.begin(), body_bytes.end());
    return packet_bytes;
}


void packets::SubackPacket::unboxing(std::vector<byte_t>& msg) {
    std::vector<byte_t> qos_vector;
    byte_t qos;

    fixed_header.unboxing(msg);

    uint8_t message_id_msb=decode_byte(msg);
    uint8_t message_id_lsb=decode_byte(msg);
    message_id=(message_id_msb<<8)|message_id_lsb;

    int payload_len=fixed_header.get_remaining_len()-2; //2 bytes for message_id_msb and message_id_lsb
    while(payload_len>0){
        qos=decode_byte(msg);
        qos_vector.push_back(qos);
        payload_len-=1; //2 for string msb and lsb, 1 for qos
    }
    return_code_vector=qos_vector;
}

void packets::SubackPacket::info(){
    std::cout<<"----INFO_SUBACK----"<<std::endl;
    std::cout<<"message_id="<<std::bitset<16>(message_id)<<std::endl;
    for (byte_t b:return_code_vector){
        std::cout<<"qos received="<<b<<std::endl;
    }
    std::cout<<"--------"<<std::endl;
}