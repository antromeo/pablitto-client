#ifndef __FIXED_HEADER__HH
#define __FIXED_HEADER__HH

#include <vector>
#include <bitset>

#include "utilities.hh"

namespace packets{

    class FixedHeader{
    private:
        byte_t message_type;
        bool dup;
        byte_t qos;
        bool retain;
        int remaining_length;

    public:
        FixedHeader(){
            this->message_type=0x01;
            this->dup=false;
            this->qos=0x00;
            this->retain=false;
            this->remaining_length=0;
        }


        FixedHeader(byte_t message_type, bool dup, byte_t qos, bool retain){
            this->message_type=message_type;
            this->dup=dup;
            this->qos=qos;
            this->retain=retain;
        }
        FixedHeader(std::vector<byte_t> msg){
            this->remaining_length=decode_remaining_len(msg);
        }

        FixedHeader(const FixedHeader& fh){
           message_type=fh.message_type;
           dup=fh.dup;
           qos=fh.qos;
           retain=fh.retain;
           remaining_length=fh.remaining_length;
        }

        ~FixedHeader(){};

        void set_remaining_length(int len){
            remaining_length=len;
        }

        void to_String(){
            std::cout<<"message type="<<int(message_type)<<std::endl;
            std::cout<<"dup="<<dup<<std::endl;
            std::cout<<"qos="<<std::bitset<8>(qos)<<std::endl;
            std::cout<<"retain="<<retain<<std::endl;
            std::cout<<"remaining len="<<remaining_length<<std::endl;
        }
        std::vector<byte_t> encode_remaining_len(int remaining_length){
            std::vector<byte_t> enc_len;
            unsigned char byte;
            while(true) {
                byte = remaining_length % 128;
                remaining_length /= 128;
                if (remaining_length > 0) byte |= 0x80;
                enc_len.push_back(byte);
                if (remaining_length==0) break;
            }
            return enc_len;
        }




        int decode_remaining_len(std::vector<byte_t> msg){
            uint32_t multiplier=0x00;
            uint32_t rem_len=0x00;
            int indice=1;
            unsigned char cifra;
            while(multiplier<27){
                cifra=msg[indice]&127;
                rem_len|=uint32_t(cifra)<<multiplier;
                if (((msg[indice]&128)>>7)==0) break;
                indice++;
                multiplier+=7;
            }
            return int(rem_len);

        }

        std::vector<byte_t> boxing(){
            std::vector<byte_t> fh_bytes;
            std::vector<byte_t> remaining_byte;
            byte_t byte;
            byte=message_type<<4|uint8_t(dup)<<3|qos<<1|uint8_t(retain);
            fh_bytes.push_back(byte);
            remaining_byte=encode_remaining_len(remaining_length);

            for (byte_t element:remaining_byte){
                fh_bytes.push_back(element);
            }
            return fh_bytes;
        }


        void unboxing(std::vector<byte_t>& msg){
            std::vector<byte_t> variable_header;
            message_type=msg[0]>>4;
            dup=(msg[0]>>3)&0x01;
            qos=(msg[0>>1])&0x03;
            retain=msg[0]&0x01;
            remaining_length=decode_remaining_len(msg);
            if (remaining_length<127) msg.erase(msg.begin(), msg.begin()+2); //erase byte always exist + 1 byte rem_len dimension
            if (remaining_length>=128 && remaining_length<=16383) msg.erase(msg.begin(), msg.begin()+3); //erase 3 byte, byte always exist + 2 byte rem_len dimension

        }

        std::vector<byte_t> get_variable_header(std::vector<byte_t> msg){
            std::vector<byte_t> variable_header;
            remaining_length=decode_remaining_len(msg);
            int fixed_header_len=msg.size()-remaining_length;
            std::cout<<"fixed_header_len="<<fixed_header_len<<std::endl;
            variable_header.insert(variable_header.begin(), msg.begin()+fixed_header_len, msg.end());
            return variable_header;
        }


        inline int get_message_type(){
            return this->message_type;

        }

        inline int get_qos(){
            return qos;
        }

        inline int get_remaining_len(){

            return remaining_length;
        }

        inline bool get_retain(){
            return this->retain;
        }

        inline void set_message_type(byte_t message_type){
            this->message_type=message_type;
        }
        inline void set_qos(byte_t qos){
            this->qos=qos;
        }
        inline void set_retain(bool retain){
            this->retain=retain;
        }

        inline void set_dup(byte_t dup){
            this->dup=dup;
        }

    };


}
#endif