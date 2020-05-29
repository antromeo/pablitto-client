#ifndef __PINGRESP_PACKET_HH
#define __PINGRESP_PACKET_HH

namespace packets {
    class PingrespPacket : public ControlPacket {

    public:
        PingrespPacket(FixedHeader fh){this->fixed_header=fh;};

        std::vector<byte_t> boxing(){
            std::vector<byte_t> packet_bytes;

            fixed_header.set_remaining_length(0); //2
            packet_bytes=fixed_header.boxing();

            return packet_bytes;
        };

        void unboxing(std::vector<byte_t> &msg){
            fixed_header.unboxing(msg);
        };

        void info(){};

    };
}



#endif
