#include <iostream>
#include "../include/unsubscribe_packet.hh"




packets::UnsubscribePacket::UnsubscribePacket(FixedHeader fh) {
    this->fixed_header=fh;
};



packets::UnsubscribePacket::UnsubscribePacket(FixedHeader fh, uint16_t message_id, std::vector<std::string> topics_vector) {
    this->fixed_header=fh;
    this->message_id=message_id;
    this->topics_vector=topics_vector;
};


packets::UnsubscribePacket::UnsubscribePacket(FixedHeader fh,  std::vector<std::string> topics_vector) {
    this->fixed_header=fh;
    static uint16_t id_static=1;
    message_id=(id_static++)%65536;
    this->topics_vector=topics_vector;
};


std::vector<byte_t> packets::UnsubscribePacket::boxing() {
    std::vector<byte_t> packet_bytes;
    std::vector<byte_t> body_bytes;
    std::vector<byte_t> bytes;

    uint8_t message_id_msb=message_id>>8;
    uint8_t message_id_lsb=message_id;
    body_bytes.push_back(message_id_msb);
    body_bytes.push_back(message_id_lsb);
    for (std::string str:topics_vector){
        bytes=encode_string(str);
        for (byte_t b:bytes){
            body_bytes.push_back(b);
        }
    }

    fixed_header.set_remaining_length(body_bytes.size());
    packet_bytes=fixed_header.boxing();

    packet_bytes.insert(packet_bytes.end(), body_bytes.begin(), body_bytes.end());
    return packet_bytes;
}


void packets::UnsubscribePacket::unboxing(std::vector<byte_t>& msg) {
    std::string topic;

    fixed_header.unboxing(msg);

    uint8_t message_id_msb=decode_byte(msg);
    uint8_t message_id_lsb=decode_byte(msg);
    message_id=(message_id_msb<<8)|message_id_lsb;
    int payload_len=fixed_header.get_remaining_len()-2; //2 bytes for message_id_msb and message_id_lsb
    while(payload_len>0){
        topic=decode_string(msg);
        topics_vector.push_back(topic);
        payload_len-=topic.size()+2; //2 for string msb and lsb
    }

}

void packets::UnsubscribePacket::info(){
    std::cout<<"INFO_UNSUBSCRIBE="<<std::endl;
    std::cout<<"message_id="<<std::bitset<16>(message_id)<<std::endl;
    for (std::string str:topics_vector){
        std::cout<<"topic="<<str<<std::endl;
    }
}
