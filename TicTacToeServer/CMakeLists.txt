cmake_minimum_required(VERSION 3.15)
project(ws LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOOST_ROOT "/opt/homebrew/opt/boost")
include_directories(${BOOST_ROOT}/include)
find_package(nlohmann_json REQUIRED)

add_executable(ws 
    ./src/server.cpp
    ./src/Session.cpp 
    ./src/SessionHandler.cpp )

target_link_libraries(ws PRIVATE pthread nlohmann_json::nlohmann_json)
