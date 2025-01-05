#pragma once

#include "InternalNodeHeader.hpp"
#include "LeafNodeHeader.hpp"
#include "NodeHeader.hpp"

class Node {
public:
    explicit Node(NodeHeader *header) : header_(header) {
    }

    virtual ~Node() { delete header_; }

    NodeHeader *getHeader() { return header_; }
    [[nodiscard]] const NodeHeader *getHeader() const { return header_; }

    void initialize(const NodeType type) {
        if (type == NodeType::LEAF) {
            header_ = new LeafNodeHeader();
        } else {
            header_ = new InternalNodeHeader();
        }
    }

private:
    NodeHeader *header_;
};
