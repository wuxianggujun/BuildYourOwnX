#include "LeafNode.hpp"
#include "Row.hpp"
#include <stdexcept>

uint32_t LeafNode::getKey(uint32_t cell_num) {
    if (cell_num >= getHeader()->getNumCells()) {
        throw std::runtime_error("Cell index out of bounds");
    }
    char* node_raw = reinterpret_cast<char*>(this);
    return *reinterpret_cast<uint32_t *>(node_raw + LEAF_NODE_CELL_OFFSET + cell_num * LEAF_NODE_CELL_SIZE);
}

Row * LeafNode::getValue(Pager *pager, uint32_t cell_num) {
    if (cell_num >= getHeader()->getNumCells()) {
        throw std::runtime_error("Cell index out of bounds");
    }
    char* node_raw = reinterpret_cast<char*>(this);
    char* cell_start = node_raw + LEAF_NODE_CELL_OFFSET + cell_num * LEAF_NODE_CELL_SIZE;
    char* value_start = cell_start + LEAF_NODE_KEY_SIZE;

    Row* row = new Row();
    row->setId(*(uint32_t*)value_start);
    row->setUsername(std::string(value_start + sizeof(uint32_t)));
    row->setEmail(std::string(value_start + sizeof(uint32_t) + COLUMN_USERNAME_SIZE + 1));
    return row;
}

void LeafNode::setKey(uint32_t cell_num, uint32_t key) {
    if (cell_num >= getHeader()->getNumCells()) {
        throw std::runtime_error("Cell index out of bounds");
    }
    char* node_raw = reinterpret_cast<char*>(this);
    *reinterpret_cast<uint32_t *>(node_raw + LEAF_NODE_CELL_OFFSET + cell_num * LEAF_NODE_CELL_SIZE) = key;
}

void LeafNode::setValue(uint32_t cell_num, const Row &value) {
    if (cell_num >= getHeader()->getNumCells()) {
        throw std::runtime_error("Cell index out of bounds");
    }
    char* node_raw = reinterpret_cast<char*>(this);
    char* cell_start = node_raw + LEAF_NODE_CELL_OFFSET + cell_num * LEAF_NODE_CELL_SIZE;
    char* value_start = cell_start + LEAF_NODE_KEY_SIZE;

    *(uint32_t*)value_start = value.getId();
    std::strncpy(value_start + sizeof(uint32_t), value.getUserName().c_str(), COLUMN_USERNAME_SIZE + 1);
    std::strncpy(value_start + sizeof(uint32_t) + COLUMN_USERNAME_SIZE + 1, value.getEmail().c_str(), COLUMN_EMAIL_SIZE + 1);
}

uint32_t LeafNode::findCellIndex(uint32_t key) {
    uint32_t num_cells = getHeader()->getNumCells();
    uint32_t min_index = 0;
    uint32_t one_past_max_index = num_cells;

    while (one_past_max_index != min_index) {
        uint32_t index = (min_index + one_past_max_index) / 2;
        uint32_t key_at_index = getKey(index);
        if (key == key_at_index) {
            return index;
        }
        if (key < key_at_index) {
            one_past_max_index = index;
        } else {
            min_index = index + 1;
        }
    }

    return min_index;
}

void LeafNode::insertAt(uint32_t cell_num, uint32_t key, const Row &value) {
    uint32_t num_cells = getHeader()->getNumCells();

    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        throw std::runtime_error("Leaf node is full");
    }

    if (cell_num < num_cells) {
        // Make room for new cell
        for (uint32_t i = num_cells; i > cell_num; i--) {
            char* node_raw = reinterpret_cast<char*>(this);
            char* dest_cell = node_raw + LEAF_NODE_CELL_OFFSET + i * LEAF_NODE_CELL_SIZE;
            char* src_cell = node_raw + LEAF_NODE_CELL_OFFSET + (i - 1) * LEAF_NODE_CELL_SIZE;
            std::memmove(dest_cell, src_cell, LEAF_NODE_CELL_SIZE);
        }
    }

    setKey(cell_num, key);
    setValue(cell_num, value);
    getHeader()->setNumCells(num_cells + 1);
}
