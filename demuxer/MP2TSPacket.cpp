//
// Created on 3/18/20.
//

#include "MP2TSPacket.h"
#include <iostream>

const int MP2TSPacket::size = 188;

MP2TSPacket::MP2TSPacket(std::unique_ptr<std::vector<std::byte>> bytes) {
    std::cout << "Got bytes!!!" << std::endl;
}
