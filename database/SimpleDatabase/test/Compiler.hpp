#pragma once

#include <string>
#include "InputBuffer.hpp"
#include "main.cpp"

enum class CompileResult {
    COMPILE_SUCCESS,
    COMPILE_ERROR
};

class Compiler {
public:
    CompileResult compile(const InputBuffer& inputBuffer, Statement& statement);
};