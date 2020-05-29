
#ifndef __DISCONNECT_PACKET_HH
#define __DISCONNECT_PACKET_HH

#include <string>
#include <vector>
#include "utilities.hh"
#include "control_packet.hh"

namespace packets {
    class DisconnectPacket : public ControlPacket {

    public:
        DisconnectPacket(FixedHeader fh){
            this->fixed_header=fh;
        };

        std::vector<byte_t> boxing(){
            std::vector<byte_t> packet_bytes;
            fixed_header.set_remaining_length(0); //2
            packet_bytes=fixed_header.boxing();
            return packet_bytes;
        };

        void unboxing(std::vector<byte_t> &msg){
            fixed_header.unboxing(msg);
        };

        void info(){
            std::cout<<"INFO_DISCONNECT"<<std::endl;
        };

    };
}



#endif