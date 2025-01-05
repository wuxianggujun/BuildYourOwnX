#pragma once

#include "NodeHeader.hpp"

class LeafNodeHeader : public NodeHeader {
public:
    LeafNodeHeader(): NodeHeader(NodeType::LEAF), numCells_(0), nextLeafPageNum_(0) {
    }

    [[nodiscard]] uint32_t getNumCells() const { return numCells_; }
    void setNumCells(const uint32_t numCells) { numCells_ = numCells; }
    [[nodiscard]] uint32_t getNextLeafPageNum() const { return nextLeafPageNum_; }
    void setNextLeafPageNum(const uint32_t nextLeafPageNum) { nextLeafPageNum_ = nextLeafPageNum; }

private:
    uint32_t numCells_;
    uint32_t nextLeafPageNum_;
};
