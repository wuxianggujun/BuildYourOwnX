cmake_minimum_required(VERSION 3.25)
project(BuildYourOwnX)

set(CMAKE_CXX_STANDARD 20)

#add_subdirectory(game/Cavestory)
#add_subdirectory(mark-sweep)
#add_subdirectory(database/Redis)
#add_subdirectory(emulator/LinuxCntr500)
#add_subdirectory(database/SimpleDatabase)
add_subdirectory(grokking_algorithms)
#[[
add_compile_options(-fsanitize=address)
add_compile_options(-fno-omit-frame-pointer)
add_link_options(-fsanitize=address)
add_link_options(-fno-omit-frame-pointer)]]
