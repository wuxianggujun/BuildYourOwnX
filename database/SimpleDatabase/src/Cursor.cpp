#include "Cursor.hpp"
#include "BTree.hpp"

Cursor::Cursor(Table *table, uint32_t page_num, uint32_t cell_num): table(table), page_num(page_num),
                                                                    cell_num(cell_num) {
    void *root_node = table->getPager().getPage(table->getRootPageNum());
    uint32_t num_cells = *leaf_node_num_cells(root_node);
    end_of_table = (num_cells == 0);
}

Cursor::~Cursor() {
}

void *Cursor::value() {
    void *page = table->getPager().getPage(page_num);
    return leaf_node_value(page, cell_num);
}

void Cursor::advance() {
    void* node = table->getPager().getPage(page_num);

    cell_num += 1;
    if (cell_num >= (*leaf_node_num_cells(node))) {
        end_of_table = true;
    }
}

bool Cursor::isEndOfTable() const {
    return end_of_table;
}

void Cursor::setCellNum(uint32_t cell_num) {
    this->cell_num = cell_num;
}

uint32_t Cursor::getPageNum() const {
    return page_num;
}

uint32_t Cursor::getCellNum() const {
    return cell_num;
}
