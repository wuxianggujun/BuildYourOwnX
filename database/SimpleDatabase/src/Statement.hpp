#pragma once

#include "Common.hpp"
#include "Row.hpp"

class Statement {
public:
    StatementType type;
    Row rowToInsert;

    explicit Statement(StatementType type) : type(type) {
    }
};
