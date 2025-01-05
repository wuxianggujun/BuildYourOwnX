#pragma once

#include <string>

class InputBuffer {
public:
    InputBuffer();

    void readInput();

    [[nodiscard]] std::string getBuffer() const;

private:
    std::string buffer_;
    size_t inputLength_;
};
