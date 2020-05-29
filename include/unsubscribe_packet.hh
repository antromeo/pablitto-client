#ifndef __UNSUBSCRIBE_PACKET_HH
#define __UNSUBSCRIBE_PACKET_HH


#include <string>
#include <vector>
#include "utilities.hh"
#include "control_packet.hh"

namespace packets {
    class UnsubscribePacket : public ControlPacket {
    private:
        uint16_t message_id;
        std::vector<std::string> topics_vector;

    public:
        UnsubscribePacket(FixedHeader fh);

        UnsubscribePacket(FixedHeader fh, uint16_t message_id, std::vector<std::string> topics_vector);
        UnsubscribePacket(FixedHeader fh,  std::vector<std::string> topics_vector);

        std::vector<byte_t> boxing();

        void unboxing(std::vector<byte_t> &msg);

        void info();
        uint16_t get_message_id(){return message_id;}

    };
}
#endif
