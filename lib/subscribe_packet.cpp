#include <iostream>
#include <random>
#include "../include/subscribe_packet.hh"


packets::SubscribePacket::SubscribePacket(FixedHeader fh) {
    this->fixed_header=fh;
};



packets::SubscribePacket::SubscribePacket(FixedHeader fh,  std::vector<std::string> topics, std::vector<byte_t> qos_vector) {
    this->fixed_header=fh;

    static uint16_t id_static=1;
    message_id=(id_static++)%65536;

    this->topics=topics;
    this->qos_vector=qos_vector;
};

std::vector<byte_t> packets::SubscribePacket::boxing() {
    std::vector<byte_t> packet_bytes;
    std::vector<byte_t> body_bytes;
    std::vector<byte_t> bytes;

    uint8_t message_id_msb=message_id>>8;
    uint8_t message_id_lsb=message_id;
    body_bytes.push_back(message_id_msb);
    body_bytes.push_back(message_id_lsb);
    int k=0;
    for (std::string str:topics){
        bytes=encode_string(str);
        for (byte_t b:bytes){
            body_bytes.push_back(b);
        }
        body_bytes.push_back(qos_vector[k]);
        k++;
    }

    fixed_header.set_remaining_length(body_bytes.size()); //2
    packet_bytes=fixed_header.boxing();

    packet_bytes.insert(packet_bytes.end(), body_bytes.begin(), body_bytes.end());
    return packet_bytes;
}


void packets::SubscribePacket::unboxing(std::vector<byte_t>& msg) {
    std::string topic;
    byte_t qos;

    fixed_header.unboxing(msg);

    uint8_t message_id_msb=decode_byte(msg);
    uint8_t message_id_lsb=decode_byte(msg);
    message_id=(message_id_msb<<8)|message_id_lsb;
    int payload_len=fixed_header.get_remaining_len()-2; //2 bytes for message_id_msb and message_id_lsb
    while(payload_len>0){
               topic=decode_string(msg);
               topics.push_back(topic);
               qos=decode_byte(msg);
               qos_vector.push_back(qos);
               payload_len-=topic.size()+2+1; //2 for string msb and lsb, 1 for qos
    }

}

void packets::SubscribePacket::info(){
    std::cout<<"----INFO_SUBSCRIBE----"<<std::endl;
    std::cout<<"message_id="<<std::bitset<16>(message_id)<<std::endl;
    int k=0;
    for (std::string str:topics){
        std::cout<<"topic="<<str<<std::endl;
        std::cout<<"qos="<<qos_vector[k]<<std::endl;
        k++;
    }
     std::cout<<"--------"<<std::endl;
}
