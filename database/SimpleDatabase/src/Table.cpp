#include "Table.hpp"

Table::Table(const std::string &filename): pager(filename), numRows(0) {
    numRows = pager.getFileLength() / ROW_SIZE;
    std::cerr << "Table created. numRows: " << numRows << std::endl;
}

Table::~Table() {
    close();
}

void Table::close() {
    pager.flush();
    std::cerr << "Table closed." << std::endl;
}

bool Table::isFull() const {
    return numRows >= TABLE_MAX_ROWS;
}

void Table::insert(const Row &row) {
    std::cerr << "Table::insert called. numRows: " << numRows << std::endl;
    // 计算行应该插入的页码
    uint32_t pageNum = numRows / ROWS_PER_PAGE;
    // 计算行在页面内的偏移量
    uint32_t rowOffset = numRows % ROWS_PER_PAGE;

    void *destination = static_cast<char *>(pager.getPage(pageNum)) + rowOffset * ROW_SIZE;

    row.serialize(destination);
    numRows++;
    std::cerr << "Table::insert inserted row at page: " << pageNum << ", offset: " << rowOffset << ", new numRows: " << numRows << std::endl;
}

void Table::select() {
    Row row;
    for (uint32_t i = 0; i < numRows; i++) {
        void *source = pager.getPage(i / ROWS_PER_PAGE);
        row.deserialize(static_cast<char *>(source) + (i % ROWS_PER_PAGE) * ROW_SIZE);
        row.print();
    }
}
