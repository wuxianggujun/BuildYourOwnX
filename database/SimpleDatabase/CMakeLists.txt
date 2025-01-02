cmake_minimum_required(VERSION 3.20)
project(SimpleDatabase)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_FILES 
        src/InputBuffer.hpp
        src/InputBuffer.cpp
        src/main.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)