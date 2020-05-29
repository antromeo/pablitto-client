#ifndef __SUBSCRIBE_PACKET_HH
#define __SUBSCRIBE_PACKET_HH

#include <string>
#include <vector>
#include "utilities.hh"
#include "control_packet.hh"

namespace packets {
    class SubscribePacket : public ControlPacket {
    private:
        uint16_t message_id;
        std::vector<std::string> topics;
        std::vector<byte_t> qos_vector;

    public:
        SubscribePacket(FixedHeader fh);

        SubscribePacket(FixedHeader fh, std::vector<std::string> topics, std::vector<byte_t> qos_vector);

        std::vector<byte_t> boxing();

        void unboxing(std::vector<byte_t> &msg);

        void info();

        inline uint16_t get_message_id(){return message_id;}

    };
}

#endif
