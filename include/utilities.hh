#ifndef  __UTILITIES__HH
#define  __UTILITIES__HH


#define AT_MOST_ONCE 0x00
#define AT_LEAST_ONCE 0x01

#define MAX_ATTEMPTS 5
#define MAX_LEN 2048

#define CONNECT 1
#define CONNACK 2
#define PUBLISH 3
#define PUBACK  4
#define PUBREC  5
#define PUBREL  6
#define PUBCOMP 7
#define SUBSCRIBE 8
#define SUBACK 9
#define UNSUBSCRIBE 10
#define UNSUBACK 11
#define PINGREQ 12
#define PINGRESP 13
#define DISCONNECT 14

#define CONNECTION_ACCEPTED 0x00
#define UNACCEPTABLE_PROTOCOL_VERSION 0x01
#define IDENTIFIER_REJECTED 0x02
#define SERVER_UNAVAILABLE 0x03
#define BAD_USERNAME_PASSWORD 0x04
#define NOT_AUTHORIZED 0x05
#define NOTHING_RESPONSE 0xFF


//#include <cstdint>
//#include <vector>

typedef unsigned char byte_t;



#endif