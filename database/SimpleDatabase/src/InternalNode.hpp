#pragma once

#include "Node.hpp"
#include "Pager.hpp"

class InternalNode : public Node {
public:

    InternalNode::InternalNode(Pager* pager, uint32_t pageNum) : Node(new InternalNodeHeader()), pager_(pager), pageNum_(pageNum) {}

    [[nodiscard]] InternalNodeHeader *getHeader() override {
        return dynamic_cast<InternalNodeHeader *>(Node::getHeader());
    }
    
    uint32_t getChildPageNum(Pager *pager, uint32_t index);

    void setChildPageNum(uint32_t index, uint32_t childPageNum);
    
    uint32_t getKey(uint32_t index);
    void setKey(uint32_t index, uint32_t key);
    
    uint32_t findChildIndex(uint32_t key);

    ~InternalNode() = default;
private:
    Pager* pager_;
    uint32_t pageNum_;

public:
    static constexpr uint32_t INTERNAL_NODE_NUM_KEYS_SIZE = sizeof(uint32_t);
    static constexpr uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE = sizeof(uint32_t);
    static constexpr uint32_t INTERNAL_NODE_KEY_SIZE = sizeof(uint32_t);
    static constexpr uint32_t INTERNAL_NODE_CHILD_SIZE = sizeof(uint32_t);
    static constexpr uint32_t INTERNAL_NODE_CELL_SIZE = INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE;
    static constexpr uint32_t INTERNAL_NODE_MAX_KEYS = 3;

    static constexpr uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
    static constexpr uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
    static constexpr uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
    static constexpr uint32_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

    static constexpr uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET = COMMON_NODE_HEADER_SIZE;
    static constexpr uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET =
            INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE;
    static constexpr uint32_t INTERNAL_NODE_HEADER_SIZE =
            COMMON_NODE_HEADER_SIZE + INTERNAL_NODE_NUM_KEYS_SIZE + INTERNAL_NODE_RIGHT_CHILD_SIZE;

    static constexpr uint32_t INTERNAL_NODE_CELL_OFFSET = INTERNAL_NODE_HEADER_SIZE;

};

