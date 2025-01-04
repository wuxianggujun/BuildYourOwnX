#pragma once

#include "Pager.hpp"
#include "Row.hpp"
#include "Common.hpp"

class Table {
public:
    explicit Table(const std::string& filename);
    ~Table();

    void close();
    bool isFull() const;
    void insert(const Row& row);
    void select();

private:
    Pager pager;
    uint32_t numRows;
};