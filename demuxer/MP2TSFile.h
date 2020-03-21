//
// Created on 3/17/20.
//

#ifndef VIDEO_LEARNING_MP2TSFILE_H
#define VIDEO_LEARNING_MP2TSFILE_H


#include <string>
#include <fstream>
#include "MP2TSPacket.h"

class MP2TSFile {
private:
    std::string path;
    std::ifstream inFile;

public:
    MP2TSFile(std::string filePath);
    bool isOpen();
    int size();
    std::string getPath();

    std::unique_ptr<MP2TSPacket> parsePacket();

    bool hasMore();
};


#endif //VIDEO_LEARNING_MP2TSFILE_H
