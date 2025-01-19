#pragma once

#include "InternalNodeHeader.hpp"
#include "LeafNodeHeader.hpp"
#include "NodeHeader.hpp"
#include <iostream>

class Node {
public:
    explicit Node(NodeHeader *header) : header_(header) {
        // Ensure header_ is properly initialized here.
        if (header_ == nullptr) {
            std::cerr << "Error: Node constructor called with null header!" << std::endl;
        }
    }

    virtual ~Node() { delete header_; }

    virtual NodeHeader *getHeader() { return header_; }
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
