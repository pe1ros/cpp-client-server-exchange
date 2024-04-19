CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(ClienServerEcn)

set(Boost_INCLUDE_DIR "/opt/homebrew/Cellar/boost")

FIND_PACKAGE(Boost 1.80 COMPONENTS system REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

SET(CMAKE_CXX_STANDARD 17)

set(SERVER_SOURCES
    Server.cpp
    json.hpp
    Common.hpp
    Exchange.hpp
)

set(CLIENT_SOURCES
    Client.cpp
    json.hpp
    Common.hpp
    Exchange.hpp
)

add_executable(Server ${SERVER_SOURCES})
TARGET_LINK_LIBRARIES(Server PRIVATE Threads::Threads ${Boost_LIBRARIES})

add_executable(Client ${CLIENT_SOURCES})
TARGET_LINK_LIBRARIES(Client PRIVATE Threads::Threads ${Boost_LIBRARIES})