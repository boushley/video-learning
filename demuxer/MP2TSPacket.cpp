//
// Created on 3/18/20.
//

#include "MP2TSPacket.h"
#include <iostream>
#include <bitset>

const int MP2TSPacket::size = 188;
const uint8_t MP2TSPacket::syncByte = static_cast<uint8_t>(0b0100'0111);

MP2TSPacket::MP2TSPacket(std::unique_ptr<std::vector<uint8_t>> bytes): rawBytes(*bytes) {
    if (rawBytes[0] != MP2TSPacket::syncByte) {
        std::cout << "Sync Byte Doesn't Match!! const: " << std::bitset<8>(MP2TSPacket::syncByte) << " data: " << std::bitset<8>(rawBytes[0]) << std::endl;
        exit(3);
    }

    transportErrorIndicator = rawBytes[1]   & 0b1000'0000;
    payloadUnitStartIndicator = rawBytes[1] & 0b0100'0000;
    transportPriority = rawBytes[1] & 0b0010'0000;
    PID = ((rawBytes[1] & 0b0001'1111) << 8) | rawBytes[2];

    transportScramblingControl = (rawBytes[3] & 0b1100'0000) >> 6;
    adaptationFieldControl = (rawBytes[3] & 0b11'0000) >> 4;
    continuityCounter = (rawBytes[3] & 0b1111);

    bool hasAdaptationField = adaptationFieldControl & 0b10;
    bool hasPayload = adaptationFieldControl & 0b1;

    std::cout << "Transport Error? " << transportErrorIndicator << " Payload Start? " << payloadUnitStartIndicator <<
        " Priority? " << transportPriority << " PID: " << PID << " scrambling? " << int(transportScramblingControl) <<
        " adaptation control? " << int(adaptationFieldControl) << " continuity counter: " << int(continuityCounter) <<
        std::endl;
}
