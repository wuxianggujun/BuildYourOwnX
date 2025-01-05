#pragma once

#include "Node.hpp"
#include "Pager.hpp"

class Row;

class LeafNode : public Node {
public:
    LeafNode() : Node(new LeafNodeHeader()) {}

    LeafNodeHeader* getHeader() { return dynamic_cast<LeafNodeHeader*>(Node::getHeader()); }
    [[nodiscard]] const LeafNodeHeader* getHeader() const { return dynamic_cast<const LeafNodeHeader*>(Node::getHeader()); }

    // Methods for accessing keys and values
    [[nodiscard]] uint32_t getKey(uint32_t cell_num) ;
    Row* getValue(Pager* pager, uint32_t cell_num);
    void setKey(uint32_t cell_num, uint32_t key);
    void setValue(uint32_t cell_num, const Row& value);
    [[nodiscard]] uint32_t findCellIndex(uint32_t key);

    void insertAt(uint32_t cell_num, uint32_t key, const Row& value);

public:
    static constexpr uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
    static constexpr uint32_t LEAF_NODE_NEXT_LEAF_SIZE = sizeof(uint32_t);
    static constexpr uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
    static constexpr uint32_t LEAF_NODE_VALUE_SIZE =
        sizeof(uint32_t) + COLUMN_USERNAME_SIZE + 1 + COLUMN_EMAIL_SIZE + 1; 
    static constexpr uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
    
    static constexpr uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
    static constexpr uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
    static constexpr uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
    static constexpr uint32_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

    static constexpr uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
    static constexpr uint32_t LEAF_NODE_NEXT_LEAF_OFFSET = LEAF_NODE_NUM_CELLS_OFFSET + LEAF_NODE_NUM_CELLS_SIZE;
    static constexpr uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE + LEAF_NODE_NEXT_LEAF_SIZE;

    static constexpr uint32_t LEAF_NODE_CELL_OFFSET = LEAF_NODE_HEADER_SIZE;
    static constexpr uint32_t LEAF_NODE_MAX_CELLS = (PAGE_SIZE - LEAF_NODE_HEADER_SIZE) / LEAF_NODE_CELL_SIZE;
};
