cmake_minimum_required(VERSION 3.10)
project(ColorTransfer)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(gflags REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

message(status "OpenCV VERSION: ${OpenCV_VERSION}")
message(status "       libraries: ${OpenCV_LIBS}")

add_executable(ColorTransfer main.cpp error_code.h log.h struct_define.h 
utils.h utils.cpp color_space.h color_space.cpp
color_transfer.h color_transfer.cpp 
contrast.h contrast.cpp)

target_link_libraries(ColorTransfer ${OpenCV_LIBS} gflags)

add_definitions(-std=c++11)