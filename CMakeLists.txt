cmake_minimum_required(VERSION 3.13)
project(pablitto_sub)
project(pablitto_pub)

set(CMAKE_CXX_STANDARD 11)

#se usi thread necessariamente devi mettere find_package e $CMAKE_THREAD_LIBS_INIT, sarà perché dipendono dal SO, boh
find_package(Threads)
#include directories serve a trovare gli header files
include_directories(./include)

#se vuoi includere tutta la cartella (i file .cpp), si deve fare un CMakeLists.txt per ogni directory
ADD_LIBRARY(pablitto lib/SocketTCP.cpp lib/pablittoMQTT.cpp lib/control_packet.cpp lib/connect_packet.cpp lib/connack_packet.cpp lib/publish_packet.cpp lib/subscribe_packet.cpp
        lib/suback_packet.cpp lib/unsubscribe_packet.cpp lib/unsuback_packet.cpp lib/puback_packet.cpp)
#target_link_libraries(LibsModule -L /libs/network/)

add_executable(pablitto_pub pablitto_pub.cpp)
add_executable(pablitto_sub pablitto_sub.cpp)
target_link_libraries(pablitto_pub pablitto ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(pablitto_sub pablitto ${CMAKE_THREAD_LIBS_INIT})
