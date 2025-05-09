#include "Table.hpp"

#include "Node.hpp"
#include "Pager.hpp"
#include "Cursor.hpp"
#include "LeafNode.hpp"
#include "InternalNode.hpp"

Table::Table(const std::string &filename): pager_(new Pager(filename)), rootPageNum_(0) {
    if (pager_->numPages_ == 0) {
        // 新数据库文件。将页面 0 初始化为叶节点。
        Node *rootNode = pager_->getPage(0);
        rootNode->initialize(NodeType::LEAF);
        rootNode->getHeader()->setRoot(true);
    }
}

Table::~Table() {
    delete pager_;
}

Cursor *Table::tableStart() {
    Cursor *cursor = new Cursor(this, 0);
    Node *rootNode = pager_->getPage(rootPageNum_);

    if (rootNode->getHeader()->getType() == NodeType::LEAF) {
        LeafNode *leafNode = dynamic_cast<LeafNode *>(rootNode);
        cursor->setCellNum(0);
        cursor->setEndOfTable(leafNode->getHeader()->getNumCells() == 0);
    } else {
        auto *internal_node = dynamic_cast<InternalNode *>(rootNode);
        uint32_t next_page_num = internal_node->getChildPageNum(pager_, 0);
        Node *next_node = pager_->getPage(next_page_num);
        while (next_node->getHeader()->getType() == NodeType::INTERNAL) {
            internal_node = dynamic_cast<InternalNode *>(next_node);
            next_page_num = internal_node->getChildPageNum(pager_, 0);
            next_node = pager_->getPage(next_page_num);
        }
        cursor->setPageNUm(next_page_num);
        cursor->setCellNum(0);
        auto *leaf_node = dynamic_cast<LeafNode *>(next_node);
        cursor->setEndOfTable(leaf_node->getHeader()->getNumCells() == 0);
    }
    return cursor;
}

void Table::close() const {
    pager_->file_.flush();

    for (uint32_t i = 0; i < pager_->numPages_; i++) {
        if (pager_->pages_[i] != nullptr) {
            pager_->flushPage(i);
        }
    }
}
