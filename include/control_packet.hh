#ifndef __CONTROL_PACKET__HH
#define __CONTROL_PACKET__HH

#include <string>
#include "utilities.hh"
#include "fixed_header.hh"


namespace packets{

    class ControlPacket{
    protected:
        FixedHeader fixed_header;

    public:

        virtual std::vector<byte_t> boxing()=0;
        virtual void unboxing(std::vector<byte_t>& variable_header)=0;
        virtual void info()=0;


        std::vector<byte_t> encode_bytes(std::vector<byte_t> bytes);
        std::vector<byte_t> decode_bytes(std::vector<byte_t>& bytes);
        byte_t decode_byte(std::vector<byte_t>& bytes);

        inline int get_message_type(){
            return this->fixed_header.get_message_type();
        }

        inline std::vector<byte_t> encode_string(std::string str){
            std::vector<byte_t> str_bytes(str.begin(), str.end());
            return encode_bytes(str_bytes);
        };

        inline std::string decode_string(std::vector<byte_t>& str_bytes_with_len){
            std::vector<byte_t> str_bytes=decode_bytes(str_bytes_with_len);
            return std::string(str_bytes.begin(), str_bytes.end());
        };

    };

}
#endif