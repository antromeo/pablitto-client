#include <iostream>
#include <regex>
#include "include/pablittoMQTT.hh"
#include "include/utilities.hh"
#include <signal.h>

template<typename T> std::string toString(const T& t);
void signal_callback_handler(int signum);
void show_help_sub();
int analyze_arguments_sub(std::vector<string> args, std::string& client_id, std::string& broker_address, int& port, std::string& topic, int& qos, bool& debug, int& keep_alive);
int target(byte_t return_code,  std::string client_id, std::vector<byte_t> str_bytes, std::string topic, int qos, PablittoMQTT& mqtt_client);



int main(int argc, char* argv[]){
    std::string broker_address="127.0.0.1";
    int port=1883;
    std::string topic;
    std::string msg;
    int qos=0;
    int keep_alive=60;
    bool debug=false;

    time_t current_time;
    current_time=time(NULL);
    std::string client_id="pablittosub-id"+toString(current_time);


    std::vector<string> args(argv+1, argv+argc);


    try {
        analyze_arguments_sub(args, client_id, broker_address, port, topic,  qos, debug, keep_alive);
    } catch (const char * msg){
        std::cerr<<msg<<std::endl;
        return -1;
    }
    PablittoMQTT mqtt_client(broker_address, port);



    try {
        mqtt_client.start(debug);
        byte_t return_code=mqtt_client.connectMQTT(client_id, keep_alive);


        std::vector<byte_t> str_bytes(msg.begin(), msg.end());
        target(return_code, client_id, str_bytes,  topic, qos,  mqtt_client);

    } catch(const char* msg){
        std::cerr<<msg<<std::endl;
        return -1;
    }

    return 0;
}



int target(byte_t return_code, std::string client_id, std::vector<byte_t> str_bytes, std::string topic, int qos, PablittoMQTT& mqtt_client){
    switch(return_code){
        case CONNECTION_ACCEPTED:
            std::cout<<"Client "<<client_id<<std::endl;
            signal(SIGINT, signal_callback_handler);
            mqtt_client.subscribe(topic, qos);
            while(true){
                std::pair<std::string, std::vector<byte_t>> message=mqtt_client.get_message();
                std::string topic=message.first;
                std::vector<byte_t> payload=message.second;
                if (!topic.empty()){
                    std::cout<<"Topic="<<topic<<std::endl;
                    std::string str_bytes(payload.begin(), payload.end());
                    std::cout<<"Message Received="<<str_bytes<<std::endl;
                }

            }
            return 1;
        case UNACCEPTABLE_PROTOCOL_VERSION:
            std::cout<<"unacceptable protocol version"<<std::endl;
            break;
        case SERVER_UNAVAILABLE:
            std::cout<<"server unavailable"<<std::endl;
            break;
        case BAD_USERNAME_PASSWORD:
            std::cout<<"bad username or password"<<std::endl;
            break;
        case NOTHING_RESPONSE:
            std::cout<<"Nothing response by broker"<<std::endl;
            break;
        case IDENTIFIER_REJECTED:
            std::cout<<"Identifier Rejected"<<std::endl;
            break;
        case NOT_AUTHORIZED:
            std::cout<<"Not Authorized"<<std::endl;
            break;
        default:
            std::cout<<"Reserved for future use"<<std::endl;
            break;
    }
    return -1;
}



void show_help_sub(){
    std::cout<<"PABLITTO"<<std::endl;
    std::cout<<"pablitto_sub help"<<std::endl;
    std::cout<<"[-p port] to select port number"<<std::endl;
    std::cout<<"[-h broker_address] to select ip address"<<std::endl;
    std::cout<<"[-t topic] to enter topic_name"<<std::endl;

    std::cout<<"[-q qos]"<<std::endl;
    std::cout<<"[-k keep alive] to enter time-interval of heartbeat"<<std::endl;
    std::cout<<"[-id client_id]"<<std::endl;

    std::cout<<"[-d debug]"<<std::endl;


}




int analyze_arguments_sub(std::vector<string> args, std::string& client_id, std::string& broker_address, int& port, std::string& topic, int& qos, bool& debug,  int& keep_alive){
     if (args.size()<=0) {
        throw "Use pablitto_sub --help to see usage";
    }
    if (args.back()=="-p" || args.back()=="-h" || args.back()=="-t" || args.back()=="-q" || args.back()=="-p" || args.back()=="-id" || args.back()=="-k") {
        throw "Unkown Arguments, Use pablitto_sub --help to see usage";
    }

    vector<string>::iterator element;
    for(element=args.begin(); element!=args.end(); element++){
        if(*element=="--help"){
            show_help_sub();
            throw "";
        }
        if(*element=="-p"){
            string str_port=*++element;
            if (regex_match(str_port, regex("[0-9]*"))){
                port=atoi(str_port.data());
            } else{
                throw "port is not a number";
            }

        }
        if(*element=="-h"){
            broker_address=*++element;
        }

        if(*element=="-t"){
            topic=*++element;
        }

        if(*element=="-q"){
            string str_qos=*++element;
            if (regex_match(str_qos, regex("[0-2]"))){
                qos=atoi(str_qos.data());
                if(qos==2){
                    throw "QoS 2 is not yet implemented";
                }
            } else{
                throw "Error: QoS is not valid";

            }

        }

        if(*element=="-id"){
            client_id=*++element;
        }

        if(*element=="-d"){
            debug=true;
        }


        if(*element=="-k"){
            string str_keep_alive=*++element;
            bool match=regex_match(str_keep_alive, regex("[0-9]*"));
            int tmp=atoi(str_keep_alive.data());
            if (match && (tmp>=0 && tmp<65536)){
                keep_alive=tmp;
            } else{
                throw "Error: keep_alive is not valid";
            }
        }
    }
    return 1;
}


template<typename T>
std::string toString(const T& t) {
    std::ostringstream oss;
    oss << t;
    return oss.str();
}


void signal_callback_handler(int signum) {
    std::clog<< "DISCONNECT: Sending" <<std::endl;
    exit(1);
}