#pragma once
#include <cstdint>

class Table;
class Row;

class Cursor {
public:
    explicit Cursor(Table *table, uint32_t pageNum, uint32_t cellNum, bool endOfTable): table_(table),
        pageNUm_(pageNum), cellNum_(cellNum), endOfTable_(endOfTable) {
    }

    explicit Cursor(Table *table, uint32_t key): table_(table), pageNUm_(0), cellNum_(0), endOfTable_(false) {
        find(key);
    }

    [[nodiscard]] Row *getValue() const;

    void advance();

    void find(uint32_t key);

    void setCellNum(uint32_t num);

    void setEndOfTable(bool endOfTable);

    void setPageNUm(uint32_t pageNum);

    uint32_t getPageNUm() {
        return pageNUm_;
    }

    uint32_t getCellNum() {
        return cellNum_;
    }

    bool isEndOfTable() {
        return endOfTable_;
    }

private:
    Table *table_;
    uint32_t pageNUm_;
    uint32_t cellNum_;
    bool endOfTable_;
};
