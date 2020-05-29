#ifndef __PUBLISH_PACKET_HH
#define __PUBLISH_PACKET_HH

#include <string>
#include <vector>
#include "utilities.hh"
#include "control_packet.hh"

namespace packets {
    class PublishPacket : public ControlPacket {
    private:
        std::string topic_name;
        uint16_t message_id;
        std::vector<byte_t> payload;

    public:
        PublishPacket(FixedHeader fh);


        PublishPacket(FixedHeader fh, uint16_t message_id, std::vector<byte_t> payload);
        PublishPacket(FixedHeader fh, std::string topic_name, std::vector<byte_t> payload);

        std::vector<byte_t> boxing();

        void unboxing(std::vector<byte_t> &msg);

        void info();

        inline uint16_t get_message_id(){
            return message_id;
        }
        inline int get_qos(){
            return fixed_header.get_qos();
        }
        inline std::string get_topic_name(){
            return topic_name;
        }

        inline std::vector<byte_t> get_payload(){
            return payload;
        }

    };
}
#endif
