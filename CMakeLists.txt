cmake_minimum_required(VERSION 3.13)
project(pablitto_sub)
project(pablitto_pub)

set(CMAKE_CXX_STANDARD 11)


find_package(Threads)

#for header files
include_directories(./include)


ADD_LIBRARY(pablitto lib/SocketTCP.cpp lib/pablittoMQTT.cpp lib/control_packet.cpp lib/connect_packet.cpp lib/connack_packet.cpp lib/publish_packet.cpp lib/subscribe_packet.cpp
        lib/suback_packet.cpp lib/unsubscribe_packet.cpp lib/unsuback_packet.cpp lib/puback_packet.cpp)

add_executable(pablitto_pub pablitto_pub.cpp)
add_executable(pablitto_sub pablitto_sub.cpp)
target_link_libraries(pablitto_pub pablitto ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(pablitto_sub pablitto ${CMAKE_THREAD_LIBS_INIT})
