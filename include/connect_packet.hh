#ifndef __CONNECT__HH
#define __CONNECT__HH

#include <string>
#include <vector>
#include "utilities.hh"
#include "control_packet.hh"


namespace packets{

    using namespace std;

    class ConnectPacket: public ControlPacket{
    private:
        string protocol_name;
        byte_t protocol_version;
        bool clean_session;
        bool will_flag;
        byte_t will_qos;
        bool will_retain;
        bool username_flag;
        bool password_flag;
        bool reserved_bit;
        uint16_t keep_alive;

        string client_id;
        string will_topic;
        vector<byte_t> will_message;
        string username;
        vector<byte_t> password;

    public:
        ConnectPacket(FixedHeader fh);
        ConnectPacket(FixedHeader fh,  string client_id, uint16_t keep_alive=0x60);


        std::vector<byte_t> boxing();
        void unboxing(std::vector<byte_t>& msg);
        void info();

        inline uint16_t get_keep_alive(){return keep_alive;};



    };


}
#endif