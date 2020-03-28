//
// Created on 3/18/20.
//

#import <string>
#import "MP2TSStreamTypes.h"

// A good list of the stream types
// https://en.wikipedia.org/wiki/Program-specific_information#Elementary_stream_types
namespace MP2TS {
    std::string streamTypeToString(StreamType t) {
        switch(t) {
            case mpeg1Video:
                return "MPEG 1 Video";
            case mpeg2Video:
                return "MPEG 2 Video";
            case mpeg1Audio:
                return "MPEG 1 Audio";
            case mpeg2Audio:
                return "MPEG 2 Audio";
            case aacAudio:
                return "AAC Audio";
            case dsmCcDeferredAssociationTag:
                return "Digital Storage Media Command and Control Deferred Association Tag";
            case h264Video:
                return "AVC / h.264 Video";
            case h265Video:
                return "HEVC / h.265 Video";
        }
    }
}
