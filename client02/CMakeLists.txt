cmake_minimum_required(VERSION 3.28)
project(client02)

set(CMAKE_CXX_STANDARD 20)

add_executable(client02 main.cpp)

target_link_libraries(client02 ws2_32)