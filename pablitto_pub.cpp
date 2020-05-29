#include <iostream>
#include "include/pablittoMQTT.hh"
#include "include/utilities.hh"

template<typename T>std::string toString(const T& t);
int target(byte_t return_code, std::string client_id, std::vector<byte_t> str_bytes, std::string topic, int qos, const bool retain, PablittoMQTT& mqtt_client);
void show_help_pub();
int analyze_arguments_pub(std::vector<string> args, std::string& client_id, std::string& broker_address, int& port, std::string& topic, std::string& msg, int& qos, bool& debug, bool& retain, int& keep_alive);

int main(int argc, char* argv[]){
    std::string broker_address="127.0.0.1";
    int port=1883;
    std::string topic;
    std::string msg;
    int qos=0;
    int keep_alive=60;
    bool retain=false;
    bool debug=false;

    time_t current_time;
    current_time=time(NULL);

    std::string client_id="publittopub-id"+toString(current_time);

    std::vector<string> args(argv+1, argv+argc);


    try {
        analyze_arguments_pub(args, client_id, broker_address, port, topic, msg, qos, debug, retain,keep_alive);
    } catch (const char * msg){
        std::cerr<<msg<<std::endl;
        return -1;
    }

    PablittoMQTT mqtt_client(broker_address, port);
    try {

        mqtt_client.is_valid_topic(topic);
        mqtt_client.start(debug);
        byte_t return_code=mqtt_client.connectMQTT(client_id, keep_alive);

        std::vector<byte_t> str_bytes(msg.begin(), msg.end());
        target(return_code,  client_id, str_bytes,  topic, qos, retain,  mqtt_client);

    } catch(const char* msg){
        std::cerr<<msg<<std::endl;
        return -1;
    }


    return 1;
}




int target(byte_t return_code, std::string client_id, std::vector<byte_t> str_bytes,  std::string topic, int qos, const bool retain, PablittoMQTT& mqtt_client){
    switch(return_code){
        case CONNECTION_ACCEPTED:
            std::cout<<"Client "<<client_id<<std::endl;
            mqtt_client.publish(topic, str_bytes, retain, qos);
            mqtt_client.disconnect();
            exit(1);
            //mqtt_client.stop();
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


int analyze_arguments_pub(std::vector<string> args, std::string& client_id, std::string& broker_address, int& port, std::string& topic, std::string& msg, int& qos, bool& debug, bool& retain, int& keep_alive){
    retain=false;
    if (args.size()<=0) {
        throw "Use pablitto_pub --help to see usage";
    }
    if (args.back()=="-p" || args.back()=="-h" || args.back()=="-t" || args.back()=="-m" || args.back()=="-q" || args.back()=="-p" || args.back()=="-id" || args.back()=="-k") {
        throw "Unkown Arguments, Use pablitto_pub --help to see usage";
    };

    vector<string>::iterator element;
    for(element=args.begin(); element!=args.end(); element++){
        if(*element=="--help"){
            show_help_pub();
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
        if(*element=="-m"){
            msg=*++element;
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
        if(*element=="-r"){
            retain=true;
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



void show_help_pub(){
    std::cout<<"PUBLITTO"<<std::endl;
    std::cout<<"publitto_pub help"<<std::endl;
    std::cout<<"[-p port] to select port number"<<std::endl;
    std::cout<<"[-h broker_address] to select ip address"<<std::endl;
    std::cout<<"[-t topic] to enter topic_name"<<std::endl;
    std::cout<<"[-m msg] to enter message to send"<<std::endl;
    std::cout<<"[-q qos]"<<std::endl;
    std::cout<<"[-k keep alive] to enter time-interval of heartbeat"<<std::endl;
    std::cout<<"[-id client_id]"<<std::endl;
    std::cout<<"[-r retain]"<<std::endl;
    std::cout<<"[-d debug]"<<std::endl;

}


template<typename T>
std::string toString(const T& t) {
    std::ostringstream oss;
    oss << t;
    return oss.str();
}
