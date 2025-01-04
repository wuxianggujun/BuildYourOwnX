#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include "Common.hpp"

class Row {
public:
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];

    void print() const;

    void serialize(void *destination) const;

    void deserialize(void *source);
};

// Declare the functions to calculate sizes and offsets
uint32_t calculateIdSize();
uint32_t calculateUsernameSize();
uint32_t calculateEmailSize();
uint32_t calculateIdOffset();
uint32_t calculateUsernameOffset();
uint32_t calculateEmailOffset();
uint32_t calculateRowSize();