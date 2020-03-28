//
// Created on 3/17/20.
//

#include <iostream>
#include "MP2TSFile.h"

namespace MP2TS {

    File::File(std::string filePath): path(filePath), hasPidMap(false) {
        inFile.open(path, std::ios::binary);
        inFile.unsetf(std::ios::skipws);
        programMapPid = INT_MAX;
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

        std::unique_ptr<Packet> packet = std::make_unique<Packet>(std::move(packetData));

        if (packet->psiData != nullptr && packet->psiData->tableId == 0) {
            auto entries = packet->psiData->programEntries;
            for (auto ptr = entries.begin(); ptr < entries.end(); ptr++) {
                if (ptr->programNumber != 0) {
                    programMapPid = ptr->pid;
                    break;
                }
            }
        } else if (packet->PID == programMapPid) {
            packet->readProgramMap();

            std::cout << "Read Program Map ";
            for (auto pm : packet->psiData->programMapEntries) {
                pidTypes.insert(std::make_pair(pm->elementaryPid, pm->streamType));
                std::cout << " PID:" << pm->elementaryPid << "='" << streamTypeToString(pm->streamType) << "' ";
            }
            std::cout << std::endl;

            hasPidMap = true;
        } else if (hasPidMap) {
            StreamType type = pidTypes[packet->PID];
            std::cout << "Parsed packet for"
                << " PID: " << packet->PID
                << " Type: '" << streamTypeToString(type) << "'"
                << std::endl;
        }

        return packet;
    }

}
