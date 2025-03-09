cmake_minimum_required(VERSION 3.20)
project(Redis)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 定义源文件列表
set(SOURCES
        src/server.cpp
        src/thread_pool.cpp
        src/heap.cpp
        src/zset.cpp
        src/avl.cpp
        src/hashtable.cpp
)

# 定义头文件列表
set(HEADERS
    src/thread_pool.h
    src/list.h
    src/zset.h
    src/hashtable.h
    src/heap.h
    src/avl.h
    src/common.h
)

# 添加客户端和服务端可执行文件
add_executable(Redis-server src/server.cpp)
add_executable(Redis-client src/client.cpp)

# 将其他源文件添加为库
add_library(redis_lib ${SOURCES})

# 链接库到可执行文件
target_link_libraries(Redis-server PRIVATE redis_lib)
target_link_libraries(Redis-client PRIVATE redis_lib)

# 包含头文件目录
target_include_directories(Redis-server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(Redis-client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)