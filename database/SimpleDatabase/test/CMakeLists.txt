# 测试程序
set(TEST_SRC_FILES
        Compiler.hpp
        Compiler.cpp
        test.cpp
        
)

add_executable(CompilerTest ${TEST_SRC_FILES})
message(STATUS "TEST_SRC_FILES: ${TEST_SRC_FILES}")
target_include_directories(CompilerTest PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/test)
message(STATUS "TEST_INCLUDE_DIRS: ${PROJECT_SOURCE_DIR}/src")
target_link_libraries(CompilerTest PRIVATE GTest::gtest_main)

# 启用测试
enable_testing()
add_test(NAME CompilerTest COMMAND CompilerTest)