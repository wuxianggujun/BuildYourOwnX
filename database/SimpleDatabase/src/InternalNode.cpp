#include "InternalNode.hpp"

#include <stdexcept>

uint32_t InternalNode::getChildPageNum(Pager *pager, uint32_t index) {
    if (index > getHeader()->getNumKeys()) {
        throw std::runtime_error("Tried to access child_num out of bounds");
    }
    if (index == getHeader()->getNumKeys()) {
        uint32_t right_child = getHeader()->getRightChildPageNum();
        if (right_child == INVALID_PAGE_NUM) {
            throw std::runtime_error("Tried to access invalid right child");
        }
        return right_child;
    }
    char* node_raw = reinterpret_cast<char*>(this);
    uint32_t child_page_num = *reinterpret_cast<uint32_t *>(node_raw + INTERNAL_NODE_CELL_OFFSET + index * INTERNAL_NODE_CELL_SIZE);
    if (child_page_num == INVALID_PAGE_NUM) {
        throw std::runtime_error("Tried to access invalid child");
    }
    return child_page_num;
}

void InternalNode::setChildPageNum(uint32_t index, uint32_t childPageNum) {
    if (index > getHeader()->getNumKeys()) {
        throw std::runtime_error("Tried to access child_num out of bounds");
    }
    if (index == getHeader()->getNumKeys()) {
        getHeader()->setRightChildPageNum(childPageNum);
    } else {
        char* node_raw = reinterpret_cast<char*>(this);
        *reinterpret_cast<uint32_t *>(node_raw + INTERNAL_NODE_CELL_OFFSET + index * INTERNAL_NODE_CELL_SIZE) = childPageNum;
    }
}

uint32_t InternalNode::getKey(uint32_t index) {
    char* node_raw = reinterpret_cast<char*>(this);
    return *reinterpret_cast<uint32_t *>(node_raw + INTERNAL_NODE_CELL_OFFSET + index * INTERNAL_NODE_CELL_SIZE +
                                         INTERNAL_NODE_CHILD_SIZE);
}

void InternalNode::setKey(uint32_t index, uint32_t key) {
    char* node_raw = reinterpret_cast<char*>(this);
    *reinterpret_cast<uint32_t *>(node_raw + INTERNAL_NODE_CELL_OFFSET + index * INTERNAL_NODE_CELL_SIZE +
                                  INTERNAL_NODE_CHILD_SIZE) = key;
}

uint32_t InternalNode::findChildIndex(uint32_t key) {
    uint32_t num_keys = getHeader()->getNumKeys();
    uint32_t min_index = 0;
    uint32_t max_index = num_keys;

    while (min_index != max_index) {
        uint32_t index = (min_index + max_index) / 2;
        uint32_t key_to_right = getKey(index);
        if (key_to_right >= key) {
            max_index = index;
        } else {
            min_index = index + 1;
        }
    }

    return min_index;
}
