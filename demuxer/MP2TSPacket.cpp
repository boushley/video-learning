//
// Created on 3/18/20.
//

#include "MP2TSPacket.h"
#include <iostream>
#include <bitset>

namespace MP2TS {

    const int Packet::size = 188;

    const uint8_t Packet::syncByte = static_cast<uint8_t>(0b0100'0111);

    Packet::Packet(std::unique_ptr<std::vector<uint8_t>> bytes) : rawBytes(*bytes) {
        if (rawBytes[0] != Packet::syncByte) {
            std::cout << "Sync Byte Doesn't Match!! const: " << std::bitset<8>(Packet::syncByte)
                    << " data: " << std::bitset<8>(rawBytes[0])
                    << std::endl;
            exit(3);
        }

        transportErrorIndicator = rawBytes[1] & 0b1000'0000;
        payloadUnitStartIndicator = rawBytes[1] & 0b0100'0000;
        transportPriority = rawBytes[1] & 0b0010'0000;
        PID = ((rawBytes[1] & 0b0001'1111) << 8) | rawBytes[2];

        transportScramblingControl = (rawBytes[3] & 0b1100'0000) >> 6;
        adaptationFieldControl = (rawBytes[3] & 0b11'0000) >> 4;
        continuityCounter = (rawBytes[3] & 0b1111);

        bool hasAdaptationField = adaptationFieldControl & 0b10;
        bool hasPayload = adaptationFieldControl & 0b1;

        dataStartPoint = 4;
        if (hasAdaptationField) {
            dataStartPoint = readAdaptationField();
        } else {
            adaptationField = nullptr;
        }

        uint8_t pointerField = 0;
        if (payloadUnitStartIndicator) {
            uint8_t pointerField = rawBytes[dataStartPoint];
            dataStartPoint = dataStartPoint + 1 + pointerField;
        }

        if (hasPayload && PID == 0) {
            readProgramAssociationTable();
        }

        // std::cout << "Transport Error? " << transportErrorIndicator
        //           << " Payload Start? " << payloadUnitStartIndicator
        //           << " Priority? " << transportPriority
        //           << " PID: " << PID
        //           << " scrambling? " << int(transportScramblingControl)
        //           << " adaptation control? " << int(adaptationFieldControl)
        //           << " continuity counter: " << int(continuityCounter)
        //           << " has adaptation? " << hasAdaptationField
        //           << " hasPayload? " << hasPayload
        //           << " data start: " << int(dataStartPoint)
        //           << " pointerField: " << (int) pointerField
        //           << std::endl;
    }

    void Packet::readProgramMap() {
        readPsiHeader();

        psiData->programNumber = (rawBytes[dataStartPoint + 3] << 8) | rawBytes[dataStartPoint + 4];

        // reserved: rawBytes[dataStartPoint + 5] & 0b1100'0000
        psiData->versionNumber = rawBytes[dataStartPoint + 5] & 0b0011'1110;
        psiData->currentNextIndicator = rawBytes[dataStartPoint + 5] & 0b0000'0001;

        uint8_t sectionNumber = rawBytes[dataStartPoint + 6];
        uint8_t lastSectionNumber = rawBytes[dataStartPoint + 7];

        // reserved: rawBytes[dataStartPoint + 8] & 0b1110'0000
        psiData->pcrPid = ((rawBytes[dataStartPoint + 8] & 0b0001'1111) << 8) | rawBytes[dataStartPoint + 9];

        // reserved: rawBytes[dataStartPoint + 10] & 0b1111'0000
        psiData->programInfoLength = ((rawBytes[dataStartPoint + 10] & 0b0000'1111) << 8) | rawBytes[dataStartPoint + 11];
        psiData->descriptor = std::vector(rawBytes.begin() + dataStartPoint + 12, rawBytes.begin() + dataStartPoint + 12 + psiData->programInfoLength);
        std::string descriptor(psiData->descriptor.begin(), psiData->descriptor.end());

        int current = dataStartPoint + 12 + psiData->programInfoLength;
        int sectionEnd = dataStartPoint + 2 + psiData->sectionLength;
        while ((sectionEnd - current) > 4) {
            std::shared_ptr<ProgramMapEntry_t> programMap = std::make_shared<ProgramMapEntry_t>();
            programMap->streamType = (StreamType)rawBytes[current++];
            // reserved: rawBytes[current] & 0b1110'0000
            programMap->elementaryPid = ((rawBytes[current] & 0b0001'1111) << 8) | rawBytes[current+1];
            current += 2;

            // reserved: rawBytes[current] & 0b1111'0000
            uint16_t esInfoLength = ((rawBytes[current] & 0b0000'1111) << 8) | rawBytes[current + 1];
            current += 2;

            programMap->descriptor = std::vector(rawBytes.begin() + current, rawBytes.begin() + current + esInfoLength);
            current += esInfoLength;

            psiData->programMapEntries.push_back(programMap);
        }

        psiData->crc = (rawBytes[sectionEnd - 3] << 24) |
                       (rawBytes[sectionEnd - 2] << 16) |
                       (rawBytes[sectionEnd - 1] << 8) |
                       rawBytes[sectionEnd];
    }

    void Packet::readProgramAssociationTable() {
        readPsiHeader();

        psiData->transportStreamId = (rawBytes[dataStartPoint + 3] << 8) | rawBytes[dataStartPoint + 4];
        // reserved: rawBytes[dataStartPoint + 5] & 0b1100'0000
        psiData->versionNumber = rawBytes[dataStartPoint + 5] & 0b0011'1110;
        psiData->currentNextIndicator = rawBytes[dataStartPoint + 5] & 0b0000'0001;

        uint8_t sectionNumber = rawBytes[dataStartPoint + 6];
        uint8_t lastSectionNumber = rawBytes[dataStartPoint + 7];

        psiData->crc = (rawBytes[dataStartPoint + 12] << 24) |
                       (rawBytes[dataStartPoint + 13] << 16) |
                       (rawBytes[dataStartPoint + 14] << 8) |
                       rawBytes[dataStartPoint + 15];

        // std::cout << "Program Association Table Table ID: " << (int) psiData->tableId << " sectionLength: " << psiData->sectionLength
        //           << " transportStreamId: " <<
        //           psiData->transportStreamId << " versionNumber: " << (int) psiData->versionNumber
        //           << " currentNextIndicator: " << psiData->currentNextIndicator <<
        //           " sectionNumber: " << (int) sectionNumber << " lastSectionNumber: " << (int) lastSectionNumber
        //           << " crc: " << psiData->crc << std::endl;

        for (int i = sectionNumber; i <= lastSectionNumber; i++) {
            ProgramAssociationEntry_t programEntry;
            programEntry.programNumber = (rawBytes[dataStartPoint + 8] << 8) | rawBytes[dataStartPoint + 9];
            // reserved: rawBytes[dataStartPoint + 10] & 0b1110'0000
            programEntry.pid = ((rawBytes[dataStartPoint + 10] & 0b0001'1111) << 8) | rawBytes[dataStartPoint + 11];
            psiData->programEntries.push_back(programEntry);
            // std::cout << "ProgramNumber: " << psiData->programEntries[0].programNumber
            //         << " PID: " << programEntry.pid
            //         << std::endl;
        }
    }

    void Packet::readPsiHeader() {
        psiData = std::make_unique<ProgramSpecificInformation_t>();
        psiData->tableId = rawBytes[dataStartPoint];
        bool sectionSyntaxIndicator = rawBytes[dataStartPoint + 1] & 0b1000'0000;
        if (!sectionSyntaxIndicator) {
            std::cout << "Section syntax indicator should have been 1... but it wasn't" << std::endl;
        }
        if (rawBytes[dataStartPoint + 1] & 0b0100'0000) {
            std::cout << "Byte that should be zero isn't!!!!!!!!!!!" << std::endl;
        }
        // reserved: rawBytes[dataStartPoint + 1] & 0b0011'0000
        psiData->sectionLength = ((rawBytes[dataStartPoint + 1] & 0b0000'1111) << 8) | rawBytes[dataStartPoint + 2];
    }

    uint8_t Packet::readAdaptationField() {
        uint8_t adaptationLength = rawBytes[4];

        if (adaptationLength == 0) {
            // If we have a 0 length adaptation field then PES data starts at 5
            return 5;
        }

        adaptationField = std::make_unique<AdaptationField_t>();

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
}
