#ifndef __PABLITTO_PUBACK_PACKET_HH
#define __PABLITTO_PUBACK_PACKET_HH

#include <iostream>
#include <string>
#include <vector>
#include "utilities.hh"
#include "control_packet.hh"


namespace packets {
    class PubackPacket : public ControlPacket {
    private:
        uint16_t message_id;

    public:
        PubackPacket(FixedHeader fh);


        PubackPacket(FixedHeader fh, uint16_t message_id);

        std::vector<byte_t> boxing();

        void unboxing(std::vector<byte_t> &msg);

        void info();

        uint16_t get_message_id(){return message_id;};

    };
}


#endif
