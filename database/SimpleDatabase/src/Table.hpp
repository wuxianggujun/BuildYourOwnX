#pragma once
#include <string>

class Pager;
class Cursor;

class Table {
public:
    explicit Table(const std::string &filename);

    ~Table();

    Cursor *tableStart();

    void close() const;

    Pager* getPager() const {
        return pager_;
    }
    
    friend class Cursor;
    friend class Pager;
    friend class InternalNode;
    friend class LeafNode;

private:
    Pager *pager_;
    uint32_t rootPageNum_;
};
