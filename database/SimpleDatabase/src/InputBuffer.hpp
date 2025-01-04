#pragma once
#include <cstddef>

class InputBuffer {

public:
    InputBuffer();
    ~InputBuffer();
    // 使用 const char* 避免不必要的修改
    [[nodiscard]] const char* getBuffer() const;
    [[nodiscard]] size_t getBufferLength() const;
    [[nodiscard]] int getInputLength() const;
    void setBufferLength(size_t buffer_length);
    void setInputLength(int input_length);

    void readInput();
    void setBuffer(const char* buffer);
private:
    char* buffer;
    size_t buffer_length;
    int input_length;
};
