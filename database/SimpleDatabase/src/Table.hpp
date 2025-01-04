#pragma once

#include "Pager.hpp"
#include "Row.hpp"
#include "Common.hpp"

class Cursor;

class Table {
public:
    explicit Table(const std::string& filename);
    ~Table();

    void close();
    bool isFull() const;
    void insert(const Row& row);
    void select();

    uint32_t getRootPageNum() const;
    
    friend class Cursor;
    Pager& getPager();

    Cursor* table_start(Table* table);
    Cursor* table_find(Table* table, uint32_t key);
    
private:
    Pager pager;
    uint32_t root_page_num;
    uint32_t getLeafNodeMaxCells() const;
};