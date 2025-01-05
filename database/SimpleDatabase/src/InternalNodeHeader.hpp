#pragma once

#include "NodeHeader.hpp"

class InternalNodeHeader : public NodeHeader {
public:
    InternalNodeHeader(): NodeHeader(NodeType::INTERNAL), numKeys_(0), rightChildPageNum_(INVALID_PAGE_NUM) {
    }

    [[nodiscard]] uint32_t& getNumKeys() { return numKeys_; }
    void setNumKeys(const uint32_t numKeys) { numKeys_ = numKeys; }

    [[nodiscard]] uint32_t getRightChildPageNum() const { return rightChildPageNum_; }
    void setRightChildPageNum(const uint32_t rightChildPageNum) { rightChildPageNum_ = rightChildPageNum; }

private:
    uint32_t numKeys_;
    uint32_t rightChildPageNum_;
};
