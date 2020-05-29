
#ifndef __CONNACK_PACKET_HH
#define __CONNACK_PACKET_HH

#include <string>
#include <vector>
#include "utilities.hh"
#include "control_packet.hh"


namespace packets{
    class ConnackPacket: public ControlPacket{
    private:
        bool session_present;
        byte_t return_code;

    public:
        ConnackPacket(FixedHeader fh);
        ConnackPacket(FixedHeader fh, bool session_present, byte_t return_code);
        std::vector<byte_t> boxing();
        void unboxing(std::vector<byte_t>& msg);
        void info();

        inline byte_t get_return_code(){
            return this->return_code;
        }

    };
}




#endif
