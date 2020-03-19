cmake_minimum_required(VERSION 3.15)
project(video_learning)

set(CMAKE_CXX_STANDARD 17)

add_executable(demuxer demuxer/demux.cpp demuxer/MP2TSFile.cpp demuxer/MP2TSFile.h demuxer/MP2TSPacket.cpp demuxer/MP2TSPacket.h)
