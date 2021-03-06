//
// Created on 3/18/20.
//

#ifndef VIDEO_LEARNING_MP2TSPACKET_H
#define VIDEO_LEARNING_MP2TSPACKET_H

#import <vector>
#include "MP2TSStreamTypes.h"

namespace MP2TS {

    struct AdaptationField_t {
        bool discontinuityIndicator;
        bool randomAccessIndicator;
        bool elementaryStreamPriorityIndicator;
        bool PcrFlag;
        bool OpcrFlag;
        bool splicingPointFlag;
        bool transportPrivateDataFlag;
        bool adaptationFieldExtensionFlag;
    };

    struct ProgramAssociationEntry_t {
        uint16_t programNumber;
        uint16_t pid;
    };

    struct ProgramMapEntry_t {
        StreamType streamType;
        uint16_t elementaryPid;
        std::vector<uint8_t> descriptor;
    };

    struct ProgramSpecificInformation_t {
        uint8_t tableId;
        uint16_t transportStreamId;
        uint16_t sectionLength;

        // Shared stuff
        uint8_t versionNumber;
        bool currentNextIndicator;

        // Program Association Table
        std::vector<ProgramAssociationEntry_t> programEntries;
        uint32_t crc;

        // Program Map Table
        uint16_t programNumber;
        uint16_t pcrPid;
        uint16_t programInfoLength;
        std::vector<uint8_t> descriptor;
        std::vector<std::shared_ptr<ProgramMapEntry_t>> programMapEntries;
    };

    class Packet {
    public:
        static const int size;
        static const uint8_t syncByte;

        std::vector<uint8_t> rawBytes;
        bool transportErrorIndicator;
        bool payloadUnitStartIndicator;
        bool transportPriority;
        uint16_t PID;
        uint8_t transportScramblingControl;
        uint8_t adaptationFieldControl;
        uint8_t continuityCounter;
        std::unique_ptr<AdaptationField_t> adaptationField;
        std::unique_ptr<ProgramSpecificInformation_t> psiData;

        Packet(std::unique_ptr<std::vector<uint8_t>> bytes);
        void readProgramMap();

    private:
        uint8_t dataStartPoint;
        uint8_t readAdaptationField();
        void readProgramAssociationTable();
        void readPsiHeader();
    };
}


#endif //VIDEO_LEARNING_MP2TSPACKET_H
