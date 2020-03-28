cmake_minimum_required(VERSION 3.15)
project(video_learning)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE DEMUXER_SRC_FILES demuxer/*.cpp)

add_executable(demuxer ${DEMUXER_SRC_FILES})
