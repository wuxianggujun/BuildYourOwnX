#include "Table.hpp"

Table::Table(const std::string &filename): pager(filename), numRows(0) {
    numRows = pager.getFileLength() / ROW_SIZE;
}

Table::~Table() {
}

void Table::close() {
    pager.flush();
}

bool Table::isFull() const {
    return numRows >= TABLE_MAX_ROWS;
}

void Table::insert(const Row &row) {
    void *destination = pager.getPage(numRows / ROWS_PER_PAGE);
    row.serialize(static_cast<char *>(destination) + (numRows % ROWS_PER_PAGE) * ROW_SIZE);
    numRows++;
}

void Table::select() {
    Row row;
    for (uint32_t i = 0; i < numRows; i++) {
        void* source = pager.getPage(i / ROWS_PER_PAGE);
        row.deserialize(static_cast<char*>(source) + (i % ROWS_PER_PAGE) * ROW_SIZE);
        row.print();
    }
}
