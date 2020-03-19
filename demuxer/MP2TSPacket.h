//
// Created on 3/18/20.
//

#ifndef VIDEO_LEARNING_MP2TSPACKET_H
#define VIDEO_LEARNING_MP2TSPACKET_H

#import <vector>

class MP2TSPacket {
public:
    static const int size;
    MP2TSPacket(std::unique_ptr<std::vector<std::byte>> bytes);
};


#endif //VIDEO_LEARNING_MP2TSPACKET_H
