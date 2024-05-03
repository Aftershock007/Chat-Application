cmake_minimum_required(VERSION 3.28)
project(client)

set(CMAKE_CXX_STANDARD 20)

add_executable(client01 main.cpp)

target_link_libraries(client01 ws2_32)