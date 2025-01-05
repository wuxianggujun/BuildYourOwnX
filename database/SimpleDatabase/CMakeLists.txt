cmake_minimum_required(VERSION 3.20)
# vcpkg configuration
set(CMAKE_TOOLCHAIN_FILE "D:/Programs/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(SimpleDatabase)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "D:/Programs/vcpkg/installed/x64-windows")

find_package(GTest CONFIG REQUIRED)

set(SRC_FILES
        src/Common.hpp
        src/Row.hpp
        src/Statement.hpp
        src/NodeHeader.hpp
        src/InternalNodeHeader.hpp
        src/LeafNodeHeader.hpp
        src/Node.hpp
        src/InternalNode.hpp
        src/LeafNode.hpp
        src/Cursor.hpp
        src/Pager.hpp
        src/Pager.cpp
        src/Table.hpp
        src/Table.cpp
        src/InputBuffer.hpp
        src/InputBuffer.cpp
        src/main.cpp
        src/Cursor.cpp
        src/InternalNode.cpp
        src/LeafNode.cpp
)
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)
# 链接 Google Test
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_TOOLCHAIN_FILE="D:/Programs/vcpkg/scripts/buildsystems/vcpkg.cmake")

#add_subdirectory(test)