# pablitto-client

![alt text](https://github.com/antromeo/pablitto-client/blob/master/logo/pablitto.png)


pablitto-client is an library of MQTT v3.1.1 (http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html)

The library was tested on Debian 10 buster

## how to use it


1) Install cmake:
    ```
     sudo apt install cmake
    ```
2) git clone:

    ```
    git clone https://github.com/antromeo/pablitto-client.git
    ```
     
3) Build 
    ```
    cd pablitto-client
    mkdir cmake-debug-files
    cd cmake-debug-files
    cmake ..
    make 
    ```
    
  4) publitto_pub, publitto_sub are examples that exchange strings as payloads. libpablitto.a is static library.


## how to use pablitto_pub

open a terminal in cmake-debug-files and type:

  ```
   ./pablitto_pub --help
  ```


## how to use pablitto_sub

open a terminal in cmake-debug-files and type:

  ```
   ./pablitto_sub --help
  ```
  

  
  ## how to use libpablitto.a
  
  supposing to use gcc and put the files in include and the libpablitto.a file in the same directory, open a terminal and type:
  
  ```
   g++ your_app.cpp -o -L . -lpablitto
  ```
  
  in your_app include  ``#include "pablittoMQTT.hh" `` and use API, visit the doc
  
  
  
  Playlist YouTube: https://www.youtube.com/watch?v=shWXoqBa2OY&list=PLcJaq9O6tuqb-dEidWH7ExByMqrzLxRYN
