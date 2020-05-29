#include "../include/pablittoMQTT.hh"


PablittoMQTT::PablittoMQTT(string ipv4_broker, int port){
    this->socketTCP.set_ipv4(ipv4_broker);
    this->socketTCP.set_port(port);
}

PablittoMQTT::PablittoMQTT(int port){
    this->socketTCP.set_port(port);
}

PablittoMQTT::~PablittoMQTT(){
    if(reader.joinable()) reader.join();
    if(keep_alive_thread.joinable()) keep_alive_thread.join();
    socketTCP.closeTCP();
}

void PablittoMQTT::stop(){
    stop_thread=true;
    if(reader.joinable()) reader.join();
    if(keep_alive_thread.joinable()) keep_alive_thread.join();
    socketTCP.closeTCP();
}


template <typename T>
T PablittoMQTT::dequeue_packet(queue<T>& queue){
    mutex.lock();
    if(queue.size()!=0) {
        T packet=queue.front();
        queue.pop();
        mutex.unlock();
        return packet;
    }
    mutex.unlock();
    throw "Coda vuota";
}

std::pair <std::string, std::vector<byte_t>> PablittoMQTT::get_message(){
    std::pair <std::string, std::vector<byte_t>> pair;
    if(!queue_publish_packet.empty()){
        packets::PublishPacket publish=dequeue_packet(queue_publish_packet);
        pair=std::make_pair(publish.get_topic_name(), publish.get_payload());
    }
    return pair;
}



template <typename T>
int PablittoMQTT::enqueue_packet(queue<T>& queue, T msg){
    mutex.lock();
    queue.push(msg);
    mutex.unlock();
    return 1;
}





void PablittoMQTT::start(bool debug) {
        if (debug) this->debug=true;
        int code=socketTCP.connectionTCP();
        if (code==-1) throw "Error: Connection Failed";
        if (code==0) reader=std::thread(&PablittoMQTT::reader_routine, this);
}



void PablittoMQTT::reader_routine() {
    int num_byte_read;
    std::vector<byte_t> msg;
    while(true){
        num_byte_read=socketTCP.readByteTCP(msg);
        if(num_byte_read>0){
            analyze_packet(msg, debug);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if(this->stop_thread){
            std::clog<<"LOG: Stop thread Reader"<<std::endl;
            return;
        }
    }
}


void PablittoMQTT::keep_alive_routine(){
    int result;
    int count=0;
    packets::FixedHeader fixed_header;
    fixed_header.set_message_type(PINGREQ);
    fixed_header.set_dup(0x00);
    fixed_header.set_qos(0x00);
    fixed_header.set_retain(0x00);
    packets::PingreqPacket pr(fixed_header);

    std::vector<byte_t> msg=pr.boxing();
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(seconds/10));
        count++;
        if(count==5) {
            count=0;
            if(debug) std::clog<<"PINGREQ: Sending"<<std::endl;

            result=socketTCP.sendByteTCP(&msg[0], msg.size());
            if (result==-1) {
                if (debug) std::clog<<"Connection is down"<<msg.size()<<std::endl;
            }
        }

        if(this->stop_thread){
            std::clog<<"LOG: Stop thread HeartBeat"<<std::endl;
            return;
        }
    }
}

void PablittoMQTT::analyze_packet(std::vector<byte_t> msg, bool debug){
    int message_type=msg[0]>>4;

    packets::FixedHeader fixed_header;

    switch (message_type) {
        case CONNACK:{
            packets::ConnackPacket connack_packet(fixed_header);
            connack_packet.unboxing(msg);
            enqueue_packet(queue_connack_packet, connack_packet);
            if(debug) std::clog<<"CONNACK: Received"<<std::endl;

            break;
        }
        case PUBLISH:{
            if(debug) std::clog<<"PUBLISH: Received"<<std::endl;

            packets::PublishPacket publish_packet(fixed_header);
            publish_packet.unboxing(msg);
            enqueue_packet(queue_publish_packet, publish_packet);
            break;
        }
        case PUBACK:{
            if(debug) std::clog<<"PUBACK: Received"<< std::endl;

            packets::PubackPacket puback_packet(fixed_header);
            puback_packet.unboxing(msg);
            enqueue_packet(queue_puback_packet, puback_packet);
            break;
        }
        case SUBACK:{
            if(debug)std::clog<<"SUBACK: Received"<<std::endl;

            packets::SubackPacket suback_packet(fixed_header);
            suback_packet.unboxing(msg);
            enqueue_packet(queue_suback_packet, suback_packet);
            break;
        }
        case PINGRESP:{
            if(debug) std::clog << "PINGRESP: Received" << std::endl;;
            break;
        }
        default:
            break;
    }
}



byte_t PablittoMQTT::connectMQTT(std::string client_id, uint16_t keep_alive){
    packets::FixedHeader fixed_header;
    fixed_header.set_message_type(CONNECT);
    fixed_header.set_qos(AT_MOST_ONCE);
    fixed_header.set_retain(0x00);
    fixed_header.set_dup(0x00);
    packets::ConnectPacket connect_packet(fixed_header, client_id, keep_alive);

    std::vector<byte_t> msg=connect_packet.boxing();

    try{
        if(debug) std::clog<<"CONNECT: Sending"<<std::endl;

        socketTCP.sendByteTCP(&msg[0], msg.size());
    } catch(const char * msg){
        std::clog<<msg<<std::endl;
    }

    int attemps=MAX_ATTEMPTS;
    do{
        if(queue_connack_packet.empty()){
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        } else{
            packets::ConnackPacket connack_packet=dequeue_packet(queue_connack_packet);
            if (connack_packet.get_return_code()==0x00){
                this->seconds=int(connect_packet.get_keep_alive());
                keep_alive_thread=std::thread(&PablittoMQTT::keep_alive_routine, this);
            }
            return connack_packet.get_return_code();
        }
        attemps--;
    } while(attemps>0);
    return NOTHING_RESPONSE;
}

byte_t PablittoMQTT::connectMQTT(std::string client_id, std::string username,vector<byte_t> password, uint16_t keep_alive){
    packets::FixedHeader fixed_header;
    fixed_header.set_message_type(CONNECT);
    fixed_header.set_qos(AT_MOST_ONCE);
    fixed_header.set_retain(0x00);
    fixed_header.set_dup(0x00);
    packets::ConnectPacket connect_packet(fixed_header, client_id, username, password, keep_alive);

    std::vector<byte_t> msg=connect_packet.boxing();

    try{
        if(debug) std::clog<<"CONNECT: Sending"<<std::endl;

        socketTCP.sendByteTCP(&msg[0], msg.size());
    } catch(const char * msg){
        std::clog<<msg<<std::endl;
    }

    int attemps=MAX_ATTEMPTS;
    do{
        if(queue_connack_packet.empty()){
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        } else{
            packets::ConnackPacket connack_packet=dequeue_packet(queue_connack_packet);
            if (connack_packet.get_return_code()==0x00){
                this->seconds=int(connect_packet.get_keep_alive());
                keep_alive_thread=std::thread(&PablittoMQTT::keep_alive_routine, this);
            }
            return connack_packet.get_return_code();
        }
        attemps--;
    } while(attemps>0);
    return NOTHING_RESPONSE;
}


byte_t PablittoMQTT::connectMQTT(std::string client_id, std::string username, vector<byte_t> password, std::string will_topic, bool will_flag,
        vector<byte_t> will_message,  bool will_retain, uint16_t keep_alive, byte_t will_qos) {
    packets::FixedHeader fixed_header;
    fixed_header.set_message_type(CONNECT);
    fixed_header.set_qos(AT_MOST_ONCE);
    fixed_header.set_retain(0x00);
    fixed_header.set_dup(0x00);

    packets::ConnectPacket connect_packet(fixed_header,  client_id, username, password,  will_topic,  will_message, keep_alive);

    std::vector<byte_t> msg=connect_packet.boxing();

    try{
        if(debug) std::clog<<"CONNECT: Sending"<<std::endl;

        socketTCP.sendByteTCP(&msg[0], msg.size());
    } catch(const char * msg){
        std::clog<<msg<<std::endl;
    }

    int attemps=MAX_ATTEMPTS;
    do{
        if(queue_connack_packet.empty()){
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        } else{
            packets::ConnackPacket connack_packet=dequeue_packet(queue_connack_packet);
            if (connack_packet.get_return_code()==0x00){
                this->seconds=int(connect_packet.get_keep_alive());
                keep_alive_thread=std::thread(&PablittoMQTT::keep_alive_routine, this);
            }
            return connack_packet.get_return_code();
        }
        attemps--;
    } while(attemps>0);
    return NOTHING_RESPONSE;
}

uint16_t PablittoMQTT::publish(std::string topic_name, std::vector<byte_t> payload, bool retain,  byte_t qos) {
    packets::FixedHeader fixed_header;
    fixed_header.set_message_type(PUBLISH);
    fixed_header.set_qos(qos);
    fixed_header.set_retain(retain);

    fixed_header.set_dup(0x00);

    packets::PublishPacket publish_packet(fixed_header, topic_name, payload);
    std::vector<byte_t> msg=publish_packet.boxing();


    try{
        if(debug) std::clog<<"PUBLISH: Sending" << std::endl;

         socketTCP.sendByteTCP(&msg[0], msg.size());
    } catch(const char * msg){
        std::clog<<msg<<std::endl;
    }

    if(qos==AT_LEAST_ONCE){
        int attemps=MAX_ATTEMPTS;
        do{
            if(queue_puback_packet.empty()){
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            } else{
                packets::PubackPacket puback_packet=dequeue_packet(queue_puback_packet);
                if (puback_packet.get_message_id()==publish_packet.get_message_id()){
                    return publish_packet.get_message_id();
                }

            }
            attemps--;
        } while(attemps>0);

        return NOTHING_RESPONSE;
    }

    return publish_packet.get_message_id();

}


uint16_t PablittoMQTT::subscribe(std::vector<string> topic_name_vector, std::vector<byte_t> qos_vector){
    packets::FixedHeader fixed_header;
    fixed_header.set_message_type(SUBSCRIBE);
    fixed_header.set_qos(AT_LEAST_ONCE);
    fixed_header.set_dup(0x00);
    fixed_header.set_retain(0x00);

    packets::SubscribePacket subscribe_packet(fixed_header, topic_name_vector, qos_vector);
    std::vector<byte_t> msg=subscribe_packet.boxing();

    try{
        if(debug) std::clog<<"SUBSCRIBE: Sending"<<std::endl;
        socketTCP.sendByteTCP(&msg[0], msg.size());
    } catch(const char * msg){
        std::clog<<msg<<std::endl;
    }


    int attemps=MAX_ATTEMPTS;
    do{
        if(queue_suback_packet.empty()){
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        } else{
            packets::SubackPacket suback_packet=dequeue_packet(queue_suback_packet);
            if (subscribe_packet.get_message_id()==suback_packet.get_message_id()){
                for (std::string topic_name:topic_name_vector){ topics.push_back(topic_name);}
            }
            return suback_packet.get_message_id();
        }
        attemps--;
    } while(attemps>0);
    return NOTHING_RESPONSE;

}


uint16_t PablittoMQTT::subscribe(std::string topic_name, byte_t qos){
    packets::FixedHeader fixed_header;
    fixed_header.set_message_type(SUBSCRIBE);
    fixed_header.set_qos(AT_LEAST_ONCE);
    fixed_header.set_dup(0x00);
    fixed_header.set_retain(0x00);

    std::vector<string> topic_name_vector;
    std::vector<byte_t> qos_vector;
    topic_name_vector.push_back(topic_name);
    qos_vector.push_back(qos);

    packets::SubscribePacket subscribe_packet(fixed_header, topic_name_vector, qos_vector);
    std::vector<byte_t> msg=subscribe_packet.boxing();

    try{
        if(debug) std::clog<<"SUBSCRIBE: Sending"<<std::endl;
        socketTCP.sendByteTCP(&msg[0], msg.size());
    } catch(const char * msg){
        std::clog<<msg<<std::endl;
    }


    int attemps=MAX_ATTEMPTS;
    do{
        if(queue_suback_packet.empty()){
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        } else{
            packets::SubackPacket suback_packet=dequeue_packet(queue_suback_packet);
            if (subscribe_packet.get_message_id()==suback_packet.get_message_id()){
                for (std::string topic_name:topic_name_vector){ topics.push_back(topic_name);}
            }
            return suback_packet.get_message_id();
        }
        attemps--;
    } while(attemps>0);
    return NOTHING_RESPONSE;

}

uint16_t PablittoMQTT::unsubscribe(std::string topic_name){
    packets::FixedHeader fixed_header;
    fixed_header.set_message_type(UNSUBSCRIBE);
    fixed_header.set_qos(AT_LEAST_ONCE);
    fixed_header.set_dup(0x00);
    fixed_header.set_retain(0x00);

    std::vector<string> topic_name_vector;
    topic_name_vector.push_back(topic_name);


    packets::UnsubscribePacket unsubscribe_packet(fixed_header, topic_name_vector);
    std::vector<byte_t> msg=unsubscribe_packet.boxing();


    try{
        if(debug) std::clog<<"UNSUBSCRIBE: Sending"<<std::endl;

        socketTCP.sendByteTCP(&msg[0], msg.size());
    } catch(const char * msg){
        std::clog<<msg<<std::endl;
    }


    int attemps=MAX_ATTEMPTS;
    do{
        if(queue_suback_packet.empty()){
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        } else{
            packets::UnsubackPacket unsuback_packet=dequeue_packet(queue_unsuback_packet);
            if (unsubscribe_packet.get_message_id()==unsuback_packet.get_message_id()){
                for (std::string topic_name:topic_name_vector){ topics.push_back(topic_name);}
            }
            return unsuback_packet.get_message_id();
        }
        attemps--;
    } while(attemps>0);
    return NOTHING_RESPONSE;

}


uint16_t PablittoMQTT::unsubscribe(std::vector<string> topic_name_vector){
    packets::FixedHeader fixed_header;
    fixed_header.set_message_type(UNSUBSCRIBE);
    fixed_header.set_qos(AT_LEAST_ONCE);
    fixed_header.set_dup(0x00);
    fixed_header.set_retain(0x00);

    packets::UnsubscribePacket unsubscribe_packet(fixed_header, topic_name_vector);
    std::vector<byte_t> msg=unsubscribe_packet.boxing();

    try{
        if(debug) std::clog<<"UNSUBSCRIBE: Sending"<<std::endl;
        socketTCP.sendByteTCP(&msg[0], msg.size());
    } catch(const char * msg){
        std::clog<<msg<<std::endl;
    }

    int attemps=MAX_ATTEMPTS;
    do{
        if(queue_suback_packet.empty()){
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        } else{
            packets::UnsubackPacket unsuback_packet=dequeue_packet(queue_unsuback_packet);
            if (unsubscribe_packet.get_message_id()==unsuback_packet.get_message_id()){
                for (std::string topic_name:topic_name_vector){ topics.push_back(topic_name);}
            }
            return unsuback_packet.get_message_id();
        }
        attemps--;
    } while(attemps>0);
    return NOTHING_RESPONSE;
}





void PablittoMQTT::disconnect() {
    packets::FixedHeader fixed_header;
    fixed_header.set_message_type(DISCONNECT);
    fixed_header.set_qos(AT_MOST_ONCE);
    fixed_header.set_retain(0x00);
    fixed_header.set_dup(0x00);

    packets::DisconnectPacket disconnect_packet(fixed_header);
    std::vector<byte_t> msg=disconnect_packet.boxing();

    try{
        if(debug) std::clog<<"DISCONNECT: Sending"<<std::endl;

        socketTCP.sendByteTCP(&msg[0], msg.size());
    } catch(const char * msg){
        std::clog<<msg<<std::endl;
    }


}


std::vector<std::string> PablittoMQTT::split_string(const std::string str){
    std::vector<std::string> str_vector;
    std::string s;
    istringstream istream(str);
    while(getline(istream, s, '/')){
        str_vector.push_back(s);
    }
    return str_vector;
}


bool PablittoMQTT::is_valid_topic(const std::string str) {
    std::vector<std::string> members=split_string(str);
    for (std::string member:members){
        if ((member.find("+")!=std::string::npos)||(member.find("#")!=std::string::npos)) {
            throw "Error: wildcard must be absent in an publish";
        }
    }
    return true;
}

