#include <iostream>
#include <vector>
#include <thread>
#include <string>

#include "channel.hpp"



int main() {
    channel chan;
    
    std::thread producer([&](){
        for (int i=0; i<100; ++i) {
            std::string s = std::to_string(i);
            chan.write_to_chan(const_cast<char*>(s.c_str()), s.size());
        }
    });

    std::thread consumer([&](){
        for (int i=0; i<100; ++i) {
            std::string s = std::to_string(i);
            const char* bytes = chan.read_from_chan(s.size());
            std::cout << bytes << " ";
            delete bytes;
        }
        std::cout << std::endl;
    });

    producer.join();
    consumer.join();

    chan.close_chan();
}