cmake_minimum_required(VERSION 3.10)
project(AesTest)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 检查操作系统
if(NOT WIN32)
    message(FATAL_ERROR "This project is Windows-only due to WinCrypt API dependency")
endif()

# 设置UTF-8编码（Windows特有）
if(WIN32)
    # 为MSVC编译器添加 /utf-8 选项，确保源文件和执行字符集为UTF-8
    add_compile_options(/utf-8)
endif()

# 添加源文件
set(SOURCES
    src/main.cpp
    src/AesEncryptor.cpp
)

# 添加头文件目录
include_directories(src)

# 创建可执行文件
add_executable(AesTest ${SOURCES})

# Windows特定设置
if(WIN32)
    # 启用ATL支持
    target_compile_definitions(AesTest PRIVATE _ATL_CSTRING_EXPLICIT_CONSTRUCTORS)
    
    # 链接Windows加密API所需的库
    target_link_libraries(AesTest advapi32)
endif()
