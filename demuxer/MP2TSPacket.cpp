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

    uint8_t dataStartPoint = 4;
    if (hasAdaptationField) {
        dataStartPoint = readAdaptationField();
    } else {
        adaptationField = nullptr;
    }

    if (hasPayload && PID == 0) {
        readProgramAssociationTable(dataStartPoint);
    }

    std::cout << "Transport Error? " << transportErrorIndicator << " Payload Start? " << payloadUnitStartIndicator <<
        " Priority? " << transportPriority << " PID: " << PID << " scrambling? " << int(transportScramblingControl) <<
        " adaptation control? " << int(adaptationFieldControl) << " continuity counter: " << int(continuityCounter) <<
        " has adaptation? " << hasAdaptationField << " hasPayload? " << hasPayload << " data start: " << int(dataStartPoint) <<
        std::endl;
}

void MP2TSPacket::readProgramAssociationTable(uint8_t startPoint) {
    std::cout << "Table ID: " << int(rawBytes[startPoint]) << std::endl;

}

uint8_t MP2TSPacket::readAdaptationField() {
    uint8_t adaptationLength = rawBytes[4];

    if (adaptationLength == 0) {
        // If we have a 0 length adaptation field then PES data starts at 5
        return 5;
    }

    adaptationField = std::make_unique<MP2TSAdaptationField_t>();

    adaptationField->discontinuityIndicator = rawBytes[5] & 0b1000'0000;
    adaptationField->randomAccessIndicator = rawBytes[5] & 0b0100'0000;
    adaptationField->elementaryStreamPriorityIndicator = rawBytes[5] & 0b0010'0000;
    adaptationField->PcrFlag = rawBytes[5] & 0b0001'0000;
    adaptationField->OpcrFlag = rawBytes[5] & 0b0000'1000;
    adaptationField->splicingPointFlag = rawBytes[5] & 0b0000'0100;
    adaptationField->transportPrivateDataFlag = rawBytes[5] & 0b0000'0010;
    adaptationField->adaptationFieldExtensionFlag = rawBytes[5] & 0b0000'0001;

    // Adaptation field has plenty more fields, see 2.4.3.4

    return 5 + adaptationLength;
}
