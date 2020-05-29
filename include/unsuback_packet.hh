#ifndef __UNSUBACK_HH
#define __UNSUBACK_HH


#include <string>
#include <vector>
#include "utilities.hh"
#include "control_packet.hh"

namespace packets {
    class UnsubackPacket : public ControlPacket {
    private:
        uint16_t message_id;

    public:
        UnsubackPacket(FixedHeader fh);

        UnsubackPacket(FixedHeader fh, uint16_t message_id);

        std::vector<byte_t> boxing();

        void unboxing(std::vector<byte_t> &msg);

        void info();

        uint16_t get_message_id(){return message_id;}

    };
}

#endif
