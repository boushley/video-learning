//
// Created on 3/17/20.
//

#ifndef VIDEO_LEARNING_MP2TSFILE_H
#define VIDEO_LEARNING_MP2TSFILE_H


#include <string>
#include <fstream>
#include "MP2TSPacket.h"

namespace MP2TS {
    class File {
    private:
        std::string path;
        std::ifstream inFile;
        uint32_t programMapPid;

    public:
        File(std::string filePath);

        bool isOpen();

        int size();

        std::string getPath();

        std::unique_ptr<Packet> parsePacket();

        bool hasMore();
    };
}


#endif //VIDEO_LEARNING_MP2TSFILE_H
