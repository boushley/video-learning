//
// Created on 3/17/20.
//

#include "MP2TSFile.h"

MP2TSFile::MP2TSFile(std::string filePath): path(filePath) {
    inFile.open(path, std::ios::binary);
    inFile.unsetf(std::ios::skipws);
}

bool MP2TSFile::isOpen() {
    return inFile.is_open();
}

int MP2TSFile::size() {
    auto currentPosition = inFile.tellg();

    inFile.seekg(0, std::ios::end);
    auto size = inFile.tellg();

    inFile.seekg(currentPosition);

    return size;
}

std::string MP2TSFile::getPath() {
    return path;
}

std::unique_ptr<MP2TSPacket> MP2TSFile::parsePacket() {
    std::unique_ptr<std::vector<std::byte>> packetData = std::make_unique<std::vector<std::byte>>(MP2TSPacket::size);
    inFile.read(reinterpret_cast<char*>(packetData->data()), MP2TSPacket::size);

    return std::make_unique<MP2TSPacket>(std::move(packetData));
}
