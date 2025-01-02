cmake_minimum_required(VERSION 3.30)
project(BuildYourOwnX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(emulator/LinuxCntr500)
add_subdirectory(database/SimpleDatabase)