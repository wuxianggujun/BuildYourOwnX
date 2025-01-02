#include "InputBuffer.hpp"

#include <iostream>
#include <string>

InputBuffer::InputBuffer(): buffer(nullptr), buffer_length(0), input_length(0) {
}

InputBuffer::~InputBuffer() {
    delete[] buffer;
    buffer = nullptr;
    buffer_length = 0;
    input_length = 0;
}

const char *InputBuffer::getBuffer() const {
    return buffer;
}

size_t InputBuffer::getBufferLength() const {
    return buffer_length;
}

int InputBuffer::getInputLength() const {
    return input_length;
}

void InputBuffer::setBufferLength(size_t buffer_length) {
    this->buffer_length = buffer_length;
}

void InputBuffer::setInputLength(int input_length) {
    this->input_length = input_length;
}

void InputBuffer::readInput() {
    std::string input_line;
    if (!std::getline(std::cin, input_line)) {
        std::cerr << "Error to read input." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!input_line.empty() && input_line.back() == '\n') {
        input_line.pop_back();
    }
    if (!input_line.empty() && input_line.back() == '\r') {
        input_line.pop_back();
    }

    input_length = input_line.length();

    // 重新分配Buffer大小
    delete[] buffer;
    buffer_length = input_length + 1;
    buffer = new char[buffer_length];

    strcpy(buffer, input_line.c_str());
}
