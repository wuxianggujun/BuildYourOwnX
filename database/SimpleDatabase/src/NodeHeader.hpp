#pragma once

#include <cstdint>

#include "Common.hpp"

class NodeHeader {
public:
    explicit NodeHeader(const NodeType type): type_(type), isRoot_(false) {
    }

    [[nodiscard]] NodeType getType() const { return type_; }
    void setType(const NodeType type) { type_ = type; }

    [[nodiscard]] bool isRoot() const { return isRoot_; }
    void setRoot(const bool isRoot) { isRoot_ = isRoot; }
    [[nodiscard]] uint32_t getParentPageNum() const { return parentPageNum; }
    void setParentPageNum(const uint32_t parentPageNum) { this->parentPageNum = parentPageNum; }

    virtual ~NodeHeader() = default;

private:
    NodeType type_{};
    bool isRoot_;
    uint32_t parentPageNum = INVALID_PAGE_NUM;
};
