cmake_minimum_required(VERSION 3.16)
project(concurrent_cache)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-O2 -Wall -Wextra")

file(GLOB SERVER_SRC
    src/server/*.cpp
)

file(GLOB UTIL_SRC
    src/util/*.cpp
)

add_executable(concurrent-cache
    src/main.cpp

    src/server/TcpServer.cpp
    src/server/Worker.cpp
    src/server/Reactor.cpp
    src/server/Connection.cpp
    src/server/Parser.cpp

    src/util/Logger.cpp
)

target_include_directories(concurrent-cache PRIVATE src)