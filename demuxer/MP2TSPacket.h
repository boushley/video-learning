//
// Created on 3/18/20.
//

#ifndef VIDEO_LEARNING_MP2TSPACKET_H
#define VIDEO_LEARNING_MP2TSPACKET_H

#import <vector>

struct MP2TSAdaptationField_t {
    bool discontinuityIndicator;
    bool randomAccessIndicator;
    bool elementaryStreamPriorityIndicator;
    bool PcrFlag;
    bool OpcrFlag;
    bool splicingPointFlag;
    bool transportPrivateDataFlag;
    bool adaptationFieldExtensionFlag;
};

struct MP2TSProgramEntries_t {
    uint16_t programNumber;
    uint16_t pid;
};

struct MP2TSProgramSpecificInformation_t {
    uint8_t tableId;
    uint16_t transportStreamId;
    uint8_t versionNumber;
    bool currentNextIndicator;
    std::vector<MP2TSProgramEntries_t> programEntries;
    uint32_t crc;
};

class MP2TSPacket {
public:
    static const int size;
    static const uint8_t syncByte;

    std::vector<uint8_t>& rawBytes;
    bool transportErrorIndicator;
    bool payloadUnitStartIndicator;
    bool transportPriority;
    uint16_t PID;
    uint8_t transportScramblingControl;
    uint8_t adaptationFieldControl;
    uint8_t continuityCounter;
    std::unique_ptr<MP2TSAdaptationField_t> adaptationField;
    std::unique_ptr<MP2TSProgramSpecificInformation_t> psiData;

    MP2TSPacket(std::unique_ptr<std::vector<uint8_t>> bytes);

private:
    uint8_t readAdaptationField();
    void readProgramAssociationTable(uint8_t startPoint);
};


#endif //VIDEO_LEARNING_MP2TSPACKET_H
