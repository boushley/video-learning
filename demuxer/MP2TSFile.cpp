//
// Created on 3/17/20.
//

#include "MP2TSFile.h"

namespace MP2TS {

    File::File(std::string filePath): path(filePath) {
        inFile.open(path, std::ios::binary);
        inFile.unsetf(std::ios::skipws);
    }

    bool File::isOpen() {
        return inFile.is_open();
    }

    int File::size() {
        auto currentPosition = inFile.tellg();

        inFile.seekg(0, std::ios::end);
        auto size = inFile.tellg();

        inFile.seekg(currentPosition);

        return size;
    }

    bool File::hasMore() {
        return inFile.peek() == Packet::syncByte;
    }

    std::string File::getPath() {
        return path;
    }

    std::unique_ptr<Packet> File::parsePacket() {
        std::unique_ptr<std::vector<uint8_t>> packetData = std::make_unique<std::vector<uint8_t>>(Packet::size);
        inFile.read(reinterpret_cast<char*>(packetData->data()), Packet::size);

        if (inFile.eof()) {
            return nullptr;
        }

        return std::make_unique<Packet>(std::move(packetData));
    }

}
