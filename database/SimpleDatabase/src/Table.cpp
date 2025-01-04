#include "Table.hpp"
#include "Cursor.hpp"
#include "BTree.hpp"

Table::Table(const std::string &filename): pager(filename), root_page_num(0) {
    if (pager.getNumPages() == 0) {
        void *root_node = pager.getPage(0);
        initialize_leaf_node(root_node);
    }
    std::cerr << "Table created." << std::endl;
}

Table::~Table() {
    close();
}

void Table::close() {
    pager.flush();
    std::cerr << "Table closed." << std::endl;
}

bool Table::isFull() const {
    return pager.getNumPages() >= TABLE_MAX_ROWS;
}

void Table::insert(const Row &row) {
    std::cerr << "Table::insert called. " << std::endl;

    void *node = pager.getPage(root_page_num);
    if ((*leaf_node_num_cells(node) >= getLeafNodeMaxCells())) {
        std::cerr << "Error: Leaf node full." << std::endl;
        exit(EXIT_FAILURE);
    }

    auto *cursor = new Cursor(this, root_page_num, *leaf_node_num_cells(node));

    // 将 const Row& 转换为 Row*
    Row *rowPtr = const_cast<Row *>(&row);
    leaf_node_insert(cursor, row.id, rowPtr);
    delete cursor;
}

void Table::select() {
    std::cerr << "Table::select called." << std::endl;
    auto *cursor = new Cursor(const_cast<Table *>(this), root_page_num, 0);

    while (!cursor->isEndOfTable()) {
        Row row{};
        row.deserialize(cursor->value());
        row.print();
        cursor->advance();
    }

    delete cursor;
    std::cerr << "Table::select finished." << std::endl;
}

uint32_t Table::getRootPageNum() const {
    return root_page_num;
}

Pager &Table::getPager() {
    return pager;
}

Cursor *Table::table_start(Table *table) {
    auto *cursor = new Cursor(table, table->getRootPageNum(), 0);
    return cursor;
}

Cursor *Table::table_find(Table *table, uint32_t key) {
    // 简单的实现，假设所有数据都在根节点
    void *root_node = table->getPager().getPage(table->getRootPageNum());
    uint32_t num_cells = *leaf_node_num_cells(root_node);

    auto *cursor = new Cursor(table, table->getRootPageNum(), 0);

    // 简单的二分查找
    uint32_t min_index = 0;
    uint32_t max_index = num_cells;
    while (min_index != max_index) {
        uint32_t index = (min_index + max_index) / 2;
        uint32_t key_at_index = *leaf_node_key(root_node, index);
        if (key_at_index == key) {
            cursor->setCellNum(index);
            return cursor;
        }
        if (key_at_index < key) {
            min_index = index + 1;
        } else {
            max_index = index;
        }
    }

    cursor->setCellNum(min_index);
    return cursor;
}

uint32_t Table::getLeafNodeMaxCells() const {
    return LEAF_NODE_MAX_CELLS;
}
