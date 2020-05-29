#ifndef __SUBACK_PACKET_HH
#define __SUBACK_PACKET_HH

#include <string>
#include <vector>
#include "utilities.hh"
#include "control_packet.hh"

namespace packets {
    class SubackPacket : public ControlPacket {
    private:
        uint16_t message_id;
        std::vector<byte_t> return_code_vector;

    public:
        SubackPacket(FixedHeader fh);


        SubackPacket(FixedHeader fh, uint16_t message_id, std::vector<byte_t> return_code_vector);

        std::vector<byte_t> boxing();

        void unboxing(std::vector<byte_t> &msg);

        void info();

        uint16_t get_message_id(){return message_id;};

    };
}


#endif
