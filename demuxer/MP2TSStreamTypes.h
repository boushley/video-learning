//
// Created on 3/18/20.
//

#ifndef VIDEO_LEARNING_MP2TSSTREAMTYPES_H
#define VIDEO_LEARNING_MP2TSSTREAMTYPES_H

#import <string>

// A good list of the stream types
// https://en.wikipedia.org/wiki/Program-specific_information#Elementary_stream_types
namespace MP2TS {
    enum StreamType {
        mpeg1Video = 0x01,
        mpeg2Video = 0x02,
        mpeg1Audio = 0x03,
        mpeg2Audio = 0x04,
        aacAudio = 0x0F,
        dsmCcDeferredAssociationTag = 0x15,
        h264Video = 0x1B,
        h265Video = 0x24,
    };

    std::string streamTypeToString(StreamType t);
}


#endif //VIDEO_LEARNING_MP2TSSTREAMTYPES_H
