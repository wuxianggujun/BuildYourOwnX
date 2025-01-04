#include "BTree.hpp"
#include "Row.hpp"
#include "MetaCommand.hpp"

void print_constants() {
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

void print_leaf_node(void* node) {
    uint32_t num_cells = *leaf_node_num_cells(node);
    printf("leaf (size %d)\n", num_cells);
    for (uint32_t i = 0; i < num_cells; i++) {
        uint32_t key = *leaf_node_key(node, i);
        printf("  - %d : %d\n", i, key);
    }
}

uint32_t* leaf_node_num_cells(void* node) {
    return (uint32_t*)(static_cast<char*>(node) + LEAF_NODE_NUM_CELLS_OFFSET);
}

void* leaf_node_cell(void* node, uint32_t cell_num) {
    return static_cast<char*>(node) + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t* leaf_node_key(void* node, uint32_t cell_num) {
    return (uint32_t*)leaf_node_cell(node, cell_num);
}

void* leaf_node_value(void* node, uint32_t cell_num) {
    return static_cast<char*>(leaf_node_cell(node, cell_num)) + LEAF_NODE_KEY_SIZE;
}

void initialize_leaf_node(void* node) {
    *leaf_node_num_cells(node) = 0;
}
// BTree.cpp
void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value) {
    void* node = cursor->table->getPager().getPage(cursor->getPageNum());

    uint32_t num_cells = *leaf_node_num_cells(node);
    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        // Node full
        printf("Need to implement splitting a leaf node.\n");
        exit(EXIT_FAILURE);
    }

    if (cursor->getCellNum() < num_cells) {
        // Make room for new cell
        for (uint32_t i = num_cells; i > cursor->getCellNum(); i--) {
            memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1),
                   LEAF_NODE_CELL_SIZE);
        }
    }

    *(leaf_node_num_cells(node)) += 1;
    *(leaf_node_key(node, cursor->getCellNum())) = key;
    // 直接将 value 序列化到页面中
    value->serialize(leaf_node_value(node, cursor->getCellNum()));

    // 标记页面已修改，需要写回磁盘
    cursor->table->getPager().markPageDirty(cursor->getPageNum());
}