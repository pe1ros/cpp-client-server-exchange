cmake_minimum_required(VERSION 3.21)
project(ClientServerEcn)

set(Boost_INCLUDE_DIR "/opt/homebrew/Cellar/boost")

find_package(Boost 1.80 COMPONENTS system REQUIRED)
find_package(Threads REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 17)

add_executable(Server Server.cpp exchange.h exchange.cpp json.hpp common.h)
target_link_libraries(Server PRIVATE Threads::Threads 
                                    ${Boost_LIBRARIES})

add_executable(Client Client.cpp exchange.h exchange.cpp json.hpp common.h)
target_link_libraries(Client PRIVATE Threads::Threads 
                                    ${Boost_LIBRARIES})
