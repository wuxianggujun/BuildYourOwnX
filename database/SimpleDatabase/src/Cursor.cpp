#include "Cursor.hpp"
#include "Node.hpp"
#include "LeafNode.hpp"
#include "InternalNode.hpp"
#include "Pager.hpp"
#include "Table.hpp"

Row *Cursor::getValue() const {
    Node *node = table_->pager_->getPage(pageNUm_);
    if (node->getHeader()->getType() == NodeType::LEAF) {
        auto *leaf_node = dynamic_cast<LeafNode *>(node);
        return leaf_node->getValue(table_->pager_, cellNum_);
    }
    return nullptr;
}

void Cursor::advance() {
    Node *node = table_->pager_->getPage(pageNUm_);

    if (node->getHeader()->getType() == NodeType::LEAF) {
        auto *leaf_node = dynamic_cast<LeafNode *>(node);
        cellNum_ += 1;
        if (cellNum_ >= leaf_node->getHeader()->getNumCells()) {
            uint32_t next_page_num = leaf_node->getHeader()->getNextLeafPageNum();
            if (next_page_num == 0) {
                endOfTable_ = true;
            } else {
                pageNUm_ = next_page_num;
                cellNum_ = 0;
            }
        }
    }
}

void Cursor::find(uint32_t key) {
    uint32_t root_page_num = table_->rootPageNum_;
    Node *root_node = table_->pager_->getPage(root_page_num);

    if (root_node->getHeader()->getType() == NodeType::LEAF) {
        LeafNode *leaf_node = dynamic_cast<LeafNode *>(root_node);
        pageNUm_ = root_page_num;
        cellNum_ = leaf_node->findCellIndex(key);
        endOfTable_ = false;
    } else {
        InternalNode *internal_node = dynamic_cast<InternalNode *>(root_node);
        uint32_t child_index = internal_node->findChildIndex(key);
        uint32_t child_page_num = internal_node->getChildPageNum(table_->pager_, child_index);
        Node *child_node = table_->pager_->getPage(child_page_num);

        while (child_node->getHeader()->getType() == NodeType::INTERNAL) {
            internal_node = dynamic_cast<InternalNode *>(child_node);
            child_index = internal_node->findChildIndex(key);
            child_page_num = internal_node->getChildPageNum(table_->pager_, child_index);
            child_node = table_->pager_->getPage(child_page_num);
        }

        LeafNode *leaf_node = dynamic_cast<LeafNode *>(child_node);
        pageNUm_ = child_page_num;
        cellNum_ = leaf_node->findCellIndex(key);
        endOfTable_ = false;
    }
}

void Cursor::setCellNum(uint32_t num) {
    this->cellNum_ = num;
}

void Cursor::setEndOfTable(bool endOfTable) {
    this->endOfTable_ = endOfTable;
}

void Cursor::setPageNUm(uint32_t pageNum) {
    this->pageNUm_ = pageNum;
}
