#include "InputBuffer.hpp"

#include <iostream>

InputBuffer::InputBuffer(): buffer_(""), inputLength_(0) {
}

void InputBuffer::readInput() {
    std::getline(std::cin, buffer_);
    inputLength_ = buffer_.length();
}

std::string InputBuffer::getBuffer() const {
    return buffer_;
}
