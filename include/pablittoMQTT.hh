#ifndef __MQTT__HH
#define __MQTT__HH



#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <string>
#include <chrono>
#include <sstream>
#include <ctime>

#include "SocketTCP.hh"
#include "control_packet.hh"
#include "connect_packet.hh"
#include "connack_packet.hh"
#include "publish_packet.hh"
#include "fixed_header.hh"
#include "subscribe_packet.hh"
#include "unsubscribe_packet.hh"
#include "suback_packet.hh"
#include "pingreq_packet.hh"
#include "pingresp_packet.hh"
#include "puback_packet.hh"
#include "disconnect_packet.hh"
#include "unsuback_packet.hh"


using namespace std;

class PablittoMQTT{
private:
    SocketTCP socketTCP;
    std::vector<string> topics;

    queue<packets::ConnackPacket> queue_connack_packet;
    queue<packets::PubackPacket> queue_puback_packet;
    queue<packets::SubackPacket> queue_suback_packet;
    queue<packets::PublishPacket> queue_publish_packet;
    queue<packets::UnsubackPacket> queue_unsuback_packet;

    std::mutex mutex;
    std::thread reader;
    std::thread keep_alive_thread;
    bool stop_thread=false;
    int seconds;
    bool debug=false;


    template <typename T> int enqueue_packet(queue<T>& queue, T msg);
    template <typename T> T dequeue_packet(queue<T>& queue);
    void reader_routine();
    void keep_alive_routine();


    std::vector<std::string> split_string(const std::string str);


public:
    PablittoMQTT(string ipv4_broker, int port);
    PablittoMQTT(int port);
    ~PablittoMQTT();

    void start(bool debug=false);
    void stop();


    inline void set_stop_thread(bool value){
        this->stop_thread=value;
    }
    void analyze_packet(std::vector<byte_t> msg, bool debug=false);

    byte_t connectMQTT(std::string client_id, uint16_t keep_alive);
    void connectMQTT(std::string client_id, std::string username,vector<byte_t> password);
    void connectMQTT(std::string client_id, std::string username,vector<byte_t> password, bool clean_session);
    void connectMQTT(std::string client_id, std::string username,vector<byte_t> password, bool clean_session, uint16_t keep_alive);
    void connectMQTT(std::string client_id, std::string username,vector<byte_t> password, bool clean_session, uint16_t keep_alive, bool will_flag, vector<byte_t> will_message, byte_t will_qos=0x00, bool will_retain=false);

    uint16_t publish(std::string topic_name, std::vector<byte_t> message, bool retain, byte_t qos=0x00);
    uint16_t subscribe(std::string topic_name, byte_t qos);
    uint16_t subscribe(std::vector<string> topic_name_vector, std::vector<byte_t> qos_vector);
    uint16_t unsubscribe(std::string topic_name);
    uint16_t unsubscribe(std::vector<string> topic_name_vector);
    bool is_valid_topic(const std::string str);
    void disconnect();

    std::pair <std::string, std::vector<byte_t>> get_message();

};



#endif