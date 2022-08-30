#include <iostream>
#include <random>
#include "../include/publish_packet.hh"


packets::PublishPacket::PublishPacket(FixedHeader fh) {
    this->fixed_header=fh;
};



packets::PublishPacket::PublishPacket(FixedHeader fh, uint16_t message_id, std::vector<byte_t> payload) {
    this->fixed_header=fh;
    this->message_id=message_id;
    this->payload=payload;
};

packets::PublishPacket::PublishPacket(FixedHeader fh, std::string topic_name, std::vector<byte_t> payload) {
    this->fixed_header=fh;
    this->topic_name=topic_name;

    static uint16_t id_static=1;
    message_id=(id_static++)%65536;

    this->payload=payload;

};

std::vector<byte_t> packets::PublishPacket::boxing() {
    std::vector<byte_t> packet_bytes;
    std::vector<byte_t> body_bytes;
    std::vector<byte_t> bytes;

    bytes=encode_string(topic_name);
    for (byte_t b:bytes) body_bytes.push_back(b);

    if (fixed_header.get_qos()>0){
        uint8_t message_id_msb=message_id>>8;
        uint8_t message_id_lsb=message_id;
        body_bytes.push_back(message_id_msb);
        body_bytes.push_back(message_id_lsb);
    }

    fixed_header.set_remaining_length(body_bytes.size()+payload.size());
    packet_bytes=fixed_header.boxing();

    for (byte_t b:payload) body_bytes.push_back(b);
    packet_bytes.insert(packet_bytes.end(), body_bytes.begin(), body_bytes.end());

    return packet_bytes;
}


void packets::PublishPacket::unboxing(std::vector<byte_t>& msg) {
    fixed_header.unboxing(msg);
    int payload_len=fixed_header.get_remaining_len();
    topic_name=decode_string(msg);
    // NOT USED IN CLIENT
    /*if (fixed_header.get_qos()>0){ //se è >0 vuol dire che c'è MessageID
        uint8_t message_id_msb=decode_byte(msg);
        uint8_t message_id_lsb=decode_byte(msg);
        message_id=(message_id_msb<<8)|message_id_lsb;
        payload_len-=topic_name.size()+4; //+4, +2 length lsb and msb of topic name, +2 lsb and msb of message_id
    } else*/{
        payload_len-=topic_name.size()+2; //+2,  lsb and msb of topic_name
    }
    if (payload_len<0) std::cout<<"Errore, gestisci eccezione"<<std::endl;
    payload.clear();
    payload.reserve(payload_len);
    for (byte_t b:msg) payload.push_back(b);

}

void packets::PublishPacket::info(){
    std::cout<<"---- INFO_PUBLISH ----"<<std::endl;
    std::cout<<"topic_name="<<topic_name<<std::endl;
    std::cout<<"message_id="<<std::bitset<16>(message_id)<<std::endl;
    std::cout<<"qos="<<fixed_header.get_qos()<<std::endl;
    int k=0; 
    for (byte_t b:payload) {
        std::cout<<"elemento numero="<<k<<" byte="<<std::bitset<8>(b)<<std::endl;
        k++;
    }
    std::cout<<"--------"<<std::endl;

}

