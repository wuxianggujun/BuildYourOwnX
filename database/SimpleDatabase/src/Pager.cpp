#include "Pager.hpp"
#include "InternalNode.hpp"
#include "LeafNode.hpp"
#include "LeafNodeHeader.hpp"
#include "InternalNodeHeader.hpp"
#include "Row.hpp"
#include "Cursor.hpp"

// Pager
Pager::Pager(const std::string &filename) : fileLength_(0), numPages_(0),
                                            file_(filename,
                                                  std::ios::in | std::ios::out | std::ios::binary | std::ios::app) {
    if (!file_.is_open()) {
        // 创建新文件
        file_.open(filename, std::ios::out | std::ios::binary);
        if (!file_.is_open()) {
            throw std::runtime_error("Unable open file: " + std::string(strerror(errno)));
        }
        file_.close();

        file_.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!file_.is_open()) {
            throw std::runtime_error("Unable to open file: " + std::string(strerror(errno)));
        }
    }
    file_.seekg(0, std::ios::end);
    fileLength_ = file_.tellg();
    numPages_ = fileLength_ / PAGE_SIZE;
    if (fileLength_ % PAGE_SIZE != 0) {
        throw std::runtime_error("Db file is not a whole number of pages. Corrupt file.");
    }
    pages_.resize(TABLE_MAX_PAGES, nullptr);
}

Pager::~Pager() {
    for (Node *page: pages_) {
        delete page;
    }
    file_.close();
}

Node *Pager::getPage(uint32_t pageNum) {
    if (pageNum > TABLE_MAX_PAGES) {
        throw std::runtime_error("Tried to fetch page number out of bounds.");
    }

    if (pages_[pageNum] == nullptr) {
        char *raw_page = new char[PAGE_SIZE];

        if (pageNum < numPages_) {
            file_.seekg(pageNum * PAGE_SIZE, std::ios::beg);
            if (!file_.read(raw_page, PAGE_SIZE)) {
                if (!file_.eof()) {
                    delete[] raw_page;
                    throw std::runtime_error("Error reading file: " + std::string(strerror(errno)));
                }
            }
        } else if (pageNum == numPages_) {
            numPages_++;
        } else {
            delete[] raw_page;
            throw std::runtime_error("Cannot allocate non-sequential pages");
        }

        uint8_t nodeTypeByte = *reinterpret_cast<uint8_t *>(raw_page);
        NodeType nodeType = static_cast<NodeType>(nodeTypeByte);

        Node *node;
        if (nodeType == NodeType::INTERNAL) {
            node = new InternalNode();
        } else if (nodeType == NodeType::LEAF) {
            node = new LeafNode();
        } else {
            node = new Node(new NodeHeader(NodeType::LEAF));
            node->initialize(NodeType::LEAF);
        }

        char *headerStart = raw_page;

        if (node->getHeader()->getType() == NodeType::LEAF) {
            auto *header = dynamic_cast<LeafNodeHeader *>(node->getHeader());
            header->setType(NodeType::LEAF);
            header->setRoot(*reinterpret_cast<bool *>(headerStart + sizeof(uint8_t)));
            header->setParentPageNum(*reinterpret_cast<uint32_t *>(headerStart + sizeof(uint8_t) + sizeof(bool)));
            header->setNumCells(
                *reinterpret_cast<uint32_t *>(headerStart + sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t)));

            header->setNextLeafPageNum(
                *reinterpret_cast<uint32_t *>(headerStart + sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t) + sizeof(
                                                  uint32_t)));
        } else if (node->getHeader()->getType() == NodeType::INTERNAL) {
            auto *header = dynamic_cast<InternalNodeHeader *>(node->getHeader());
            header->setType(NodeType::INTERNAL);
            header->setRoot(*reinterpret_cast<bool *>(headerStart + sizeof(uint8_t)));
            header->setParentPageNum(*reinterpret_cast<uint32_t *>(headerStart + sizeof(uint8_t) + sizeof(bool)));
            header->setNumKeys(
                *reinterpret_cast<uint32_t *>(headerStart + sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t)));
            header->setRightChildPageNum(
                *reinterpret_cast<uint32_t *>(headerStart + sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t) + sizeof(
                                                  uint32_t)));
        }
        pages_[pageNum] = node;
        delete[] raw_page;
    }
    return pages_[pageNum];
}

void Pager::flushPage(uint32_t pageNum) {
     if (pageNum >= pages_.size() || pages_[pageNum] == nullptr) {
        throw std::runtime_error("Tried to flush invalid page");
    }
    
    Node* node = pages_[pageNum];
    char* raw_page = new char[PAGE_SIZE];
    std::memset(raw_page, 0, PAGE_SIZE); 

    // Serialize header
    char* header_start = raw_page;
    if (node->getHeader()->getType() == NodeType::LEAF) {
        auto* header = dynamic_cast<LeafNodeHeader*>(node->getHeader());
        *reinterpret_cast<uint8_t *>(header_start) = static_cast<uint8_t>(header->getType());
        *reinterpret_cast<bool *>(header_start + sizeof(uint8_t)) = header->isRoot();
        *reinterpret_cast<uint32_t *>(header_start + sizeof(uint8_t) + sizeof(bool)) = header->getParentPageNum();
        *reinterpret_cast<uint32_t *>(header_start + sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t)) = header->getNumCells();
        *reinterpret_cast<uint32_t *>(header_start + sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t) + sizeof(uint32_t)) = header->getNextLeafPageNum();
    } else if (node->getHeader()->getType() == NodeType::INTERNAL) {
        auto* header = dynamic_cast<InternalNodeHeader*>(node->getHeader());
        *reinterpret_cast<uint8_t *>(header_start) = static_cast<uint8_t>(header->getType());
        *reinterpret_cast<bool *>(header_start + sizeof(uint8_t)) = header->isRoot();
        *reinterpret_cast<uint32_t *>(header_start + sizeof(uint8_t) + sizeof(bool)) = header->getParentPageNum();
        *reinterpret_cast<uint32_t *>(header_start + sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t)) = header->getNumKeys();
        *reinterpret_cast<uint32_t *>(header_start + sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t) + sizeof(uint32_t)) = header->getRightChildPageNum();
    }

    // Serialize body (for leaf nodes)
    if (node->getHeader()->getType() == NodeType::LEAF) {
        auto* leaf_node = dynamic_cast<LeafNode*>(node);
        for (uint32_t i = 0; i < leaf_node->getHeader()->getNumCells(); i++) {
            char* cell_start = raw_page + LeafNode::LEAF_NODE_HEADER_SIZE + i * LeafNode::LEAF_NODE_CELL_SIZE;
            *reinterpret_cast<uint32_t *>(cell_start) = leaf_node->getKey(i);
            Row* row = leaf_node->getValue(this,i);
            
            char* row_start = cell_start + LeafNode::LEAF_NODE_KEY_SIZE;
            *reinterpret_cast<uint32_t *>(row_start) = row->getId();
            std::strncpy(row_start + sizeof(uint32_t), row->getUserName().c_str(), COLUMN_USERNAME_SIZE + 1);
            std::strncpy(row_start + sizeof(uint32_t) + COLUMN_USERNAME_SIZE + 1, row->getEmail().c_str(), COLUMN_EMAIL_SIZE + 1);
        }
    } else if (node->getHeader()->getType() == NodeType::INTERNAL){
        auto* internal_node = dynamic_cast<InternalNode*>(node);
        for (uint32_t i = 0; i < internal_node->getHeader()->getNumKeys(); i++) {
            char* cell_start = raw_page + InternalNode::INTERNAL_NODE_CELL_OFFSET + i * InternalNode::INTERNAL_NODE_CELL_SIZE;
            *reinterpret_cast<uint32_t *>(cell_start) = internal_node->getChildPageNum(this,i);
            *reinterpret_cast<uint32_t *>(cell_start + InternalNode::INTERNAL_NODE_CHILD_SIZE) = internal_node->getKey(i);
        }
    }

    file_.seekp(pageNum * PAGE_SIZE, std::ios::beg);
    if (!file_.write(raw_page, PAGE_SIZE)) {
        delete[] raw_page;
        throw std::runtime_error("Error writing to file: " + std::string(strerror(errno)));
    }

    delete[] raw_page;
}

uint32_t Pager::getUnusedPageNum() const {
    return numPages_;
}

uint32_t Pager::getNodeMaxKey(Node *node) {
    if (node->getHeader()->getType() == NodeType::LEAF) {
        LeafNode* leaf_node = static_cast<LeafNode*>(node);
        return leaf_node->getKey(leaf_node->getHeader()->getNumCells() - 1);
    }
    InternalNode* internal_node = static_cast<InternalNode*>(node);
    Node* right_child = getPage(internal_node->getHeader()->getRightChildPageNum());
    return getNodeMaxKey(right_child);
}

void Pager::createNewRoot(uint32_t rightChildPageNum) {
    Node* root = getPage(0);
    Node* right_child = getPage(rightChildPageNum);
    uint32_t left_childPageNum = getUnusedPageNum();
    Node* left_child = getPage(left_childPageNum);

    if (root->getHeader()->getType() == NodeType::INTERNAL) {
        left_child->initialize(NodeType::INTERNAL);
    } else {
        left_child->initialize(NodeType::LEAF);
    }
  
    std::memcpy(reinterpret_cast<char*>(left_child), reinterpret_cast<char*>(root), PAGE_SIZE);
    left_child->getHeader()->setRoot(false);

    if (left_child->getHeader()->getType() == NodeType::INTERNAL) {
        InternalNode* internal_left_child = static_cast<InternalNode*>(left_child);
        for (int i = 0; i < internal_left_child->getHeader()->getNumKeys(); i++) {
            Node* child = getPage(internal_left_child->getChildPageNum(this,i));
            child->getHeader()->setParentPageNum(left_childPageNum);
        }
        Node* right_most_child = getPage(internal_left_child->getHeader()->getRightChildPageNum());
        right_most_child->getHeader()->setParentPageNum(left_childPageNum);
    }

    root->initialize(NodeType::INTERNAL);
    InternalNode* internal_root = static_cast<InternalNode*>(root);
    internal_root->getHeader()->setRoot(true);
    internal_root->getHeader()->setNumKeys(1);
    internal_root->setChildPageNum(0, left_childPageNum);
    uint32_t left_child_max_key = getNodeMaxKey(left_child);
    internal_root->setKey(0, left_child_max_key);
    internal_root->getHeader()->setRightChildPageNum(rightChildPageNum);
    left_child->getHeader()->setParentPageNum(0);
    right_child->getHeader()->setParentPageNum(0);
}

void Pager::internalNodeInsert(uint32_t parentPageNum, uint32_t childPageNum) {
    Node* parent = getPage(parentPageNum);
    InternalNode* internal_parent = static_cast<InternalNode*>(parent);
    Node* child = getPage(childPageNum);
    uint32_t child_max_key = getNodeMaxKey(child);
    uint32_t index = internal_parent->findChildIndex(child_max_key);

    uint32_t original_num_keys = internal_parent->getHeader()->getNumKeys();

    if (original_num_keys >= InternalNode::INTERNAL_NODE_MAX_KEYS) {
        internalNodeSplitAndInsert(parentPageNum, childPageNum);
        return;
    }

    uint32_t right_childPageNum = internal_parent->getHeader()->getRightChildPageNum();

    if (right_childPageNum == INVALID_PAGE_NUM) {
        internal_parent->getHeader()->setRightChildPageNum(childPageNum);
        return;
    }

    Node* right_child = getPage(right_childPageNum);

    internal_parent->getHeader()->setNumKeys(original_num_keys + 1);

    if (child_max_key > getNodeMaxKey(right_child)) {
        internal_parent->setChildPageNum(original_num_keys, right_childPageNum);
        internal_parent->setKey(original_num_keys, getNodeMaxKey(right_child));
        internal_parent->getHeader()->setRightChildPageNum(childPageNum);
    } else {
        for (uint32_t i = original_num_keys; i > index; i--) {
            internal_parent->setChildPageNum(i, internal_parent->getChildPageNum(this, i - 1));
            internal_parent->setKey(i, internal_parent->getKey(i-1));
        }
        internal_parent->setChildPageNum(index, childPageNum);
        internal_parent->setKey(index, child_max_key);
    }
}

void Pager::internalNodeSplitAndInsert(uint32_t parentPageNum, uint32_t childPageNum) {
     uint32_t old_page_num = parentPageNum;
    Node* old_node_base = getPage(parentPageNum);
    InternalNode* old_node = static_cast<InternalNode*>(old_node_base);
    uint32_t old_max = getNodeMaxKey(old_node);

    Node* child_base = getPage(childPageNum);
    uint32_t child_max = getNodeMaxKey(child_base);

    uint32_t new_page_num = getUnusedPageNum();

    bool splitting_root = old_node->getHeader()->isRoot();

    Node* parent_base;
    Node* new_node_base;
    if (splitting_root) {
        createNewRoot(new_page_num);
        parent_base = getPage(0);

        old_page_num = static_cast<InternalNode*>(parent_base)->getChildPageNum(this,0);
        old_node_base = getPage(old_page_num);
        old_node = static_cast<InternalNode*>(old_node_base);
    } else {
        parent_base = getPage(old_node->getHeader()->getParentPageNum());
        new_node_base = getPage(new_page_num);
        new_node_base->initialize(NodeType::INTERNAL);
        new_node_base->getHeader()->setParentPageNum(old_node->getHeader()->getParentPageNum());
    }
    InternalNode* new_node = static_cast<InternalNode*>(new_node_base);

    uint32_t* old_num_keys = &old_node->getHeader()->getNumKeys();

    uint32_t cur_page_num = old_node->getHeader()->getRightChildPageNum();
    Node* cur = getPage(cur_page_num);

    internalNodeInsert(new_page_num, cur_page_num);
    cur->getHeader()->setParentPageNum(new_page_num);
    old_node->getHeader()->setRightChildPageNum(INVALID_PAGE_NUM);

    for (int i = InternalNode::INTERNAL_NODE_MAX_KEYS - 1; i > InternalNode::INTERNAL_NODE_MAX_KEYS / 2; i--) {
        cur_page_num = old_node->getChildPageNum(this,i);
        cur = getPage(cur_page_num);

        internalNodeInsert(new_page_num, cur_page_num);
        cur->getHeader()->setParentPageNum(new_page_num);

        (*old_num_keys)--;
    }

    old_node->getHeader()->setRightChildPageNum(old_node->getChildPageNum(this,*old_num_keys - 1));
    (*old_num_keys)--;

    uint32_t max_after_split = getNodeMaxKey(old_node);

    uint32_t destination_page_num = child_max < max_after_split ? old_page_num : new_page_num;

    internalNodeInsert(destination_page_num, childPageNum);
    getPage(childPageNum)->getHeader()->setParentPageNum(destination_page_num);

    updateInternalNodeKey(parent_base->getHeader()->getParentPageNum(), old_max, getNodeMaxKey(old_node));

    if (!splitting_root) {
        internalNodeInsert(old_node->getHeader()->getParentPageNum(),new_page_num);
    }
}

void Pager::leafNodeSplitAndInsert(Cursor *cursor, uint32_t key, Row *value) {
      Node* old_node_base = getPage(cursor->getPageNUm());
    LeafNode* old_node = static_cast<LeafNode*>(old_node_base);
    uint32_t old_max = getNodeMaxKey(old_node);
    uint32_t new_page_num = getUnusedPageNum();
    Node* new_node_base = getPage(new_page_num);
    new_node_base->initialize(NodeType::LEAF);
    LeafNode* new_node = static_cast<LeafNode*>(new_node_base);

    new_node->getHeader()->setParentPageNum(old_node->getHeader()->getParentPageNum());
    new_node->getHeader()->setNextLeafPageNum(old_node->getHeader()->getNextLeafPageNum());
    old_node->getHeader()->setNextLeafPageNum(new_page_num);

    const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT = (LeafNode::LEAF_NODE_MAX_CELLS + 1) / 2;
    const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT = (LeafNode::LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_LEFT_SPLIT_COUNT;

    for (int32_t i = LeafNode::LEAF_NODE_MAX_CELLS; i >= 0; i--) {
        LeafNode* destination_node;
        if (i >= LEAF_NODE_LEFT_SPLIT_COUNT) {
            destination_node = new_node;
        } else {
            destination_node = old_node;
        }
        uint32_t index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;

        if (i == cursor->getCellNum()) {
            destination_node->setKey(index_within_node, key);
            destination_node->setValue(index_within_node, *value);
        } else if (i > cursor->getCellNum()) {
            
            uint32_t dest_key = old_node->getKey(i - 1);
            Row* dest_value = old_node->getValue(this, i - 1);
            destination_node->setKey(index_within_node, dest_key);
            destination_node->setValue(index_within_node, *dest_value);
            delete dest_value;
        } else {
            uint32_t dest_key = old_node->getKey(i);
            Row* dest_value = old_node->getValue(this, i);
            destination_node->setKey(index_within_node, dest_key);
            destination_node->setValue(index_within_node, *dest_value);
            delete dest_value;
        }
    }

    old_node->getHeader()->setNumCells(LEAF_NODE_LEFT_SPLIT_COUNT);
    new_node->getHeader()->setNumCells(LEAF_NODE_RIGHT_SPLIT_COUNT);

    if (old_node->getHeader()->isRoot()) {
        createNewRoot(new_page_num);
    } else {
        uint32_t parent_page_num = old_node->getHeader()->getParentPageNum();
        uint32_t new_max = getNodeMaxKey(old_node);
        updateInternalNodeKey(parent_page_num, old_max, new_max);
        internalNodeInsert(parent_page_num, new_page_num);
    }
}

void Pager::updateInternalNodeKey(uint32_t nodePageNum, uint32_t oldKey, uint32_t newKey) {
    Node* node = getPage(nodePageNum);
    InternalNode* internal_node = static_cast<InternalNode*>(node);
    uint32_t old_child_index = internal_node->findChildIndex(oldKey);
    internal_node->setKey(old_child_index, newKey);
}
