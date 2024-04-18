cmake_minimum_required(VERSION 3.21)
project(ClientServer)

set(Boost_INCLUDE_DIRS "/opt/homebrew/Cellar/boost")

find_package(Boost 1.40 COMPONENTS system REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(Server Server.cpp)
add_executable(Client Client.cpp)