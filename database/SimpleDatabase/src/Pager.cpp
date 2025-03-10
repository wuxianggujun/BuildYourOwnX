#include "Pager.hpp"

#include <iostream>

#include "Node.hpp"
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
    }else
    {
        file_.seekg(0, std::ios::end);
        fileLength_ = file_.tellg();
        file_.seekg(0, std::ios::beg);
     
        if (fileLength_ % PAGE_SIZE != 0) {
            file_.close();
            throw std::runtime_error("Db file is not a whole number of pages. Corrupt file.");
        }
        numPages_ = fileLength_ / PAGE_SIZE;
    }

    pages_.resize(TABLE_MAX_PAGES, nullptr);

    // 初始化 pages_ 和 page_data_ 数组
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        page_data_[i] = nullptr;
    }
}

Pager::~Pager() {
    // 关闭文件
    if (file_.is_open()) {
        file_.close();
    }

    // 释放 pages_ 和 page_data_ 数组中分配的内存
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        if (pages_[i] != nullptr) {
            delete pages_[i];
            pages_[i] = nullptr;
        }
        if (page_data_[i] != nullptr) {
            delete[] page_data_[i];
            page_data_[i] = nullptr;
        }
    }
}

Node* Pager::getPage(uint32_t pageNum) {
    // 检查页码是否越界
    if (pageNum > TABLE_MAX_PAGES) {
        throw std::runtime_error("尝试获取超出范围的页码。");
    }

    // 如果该页尚未缓存，则进行加载
    if (pages_[pageNum] == nullptr) {
        // 分配一页的内存
        char* raw_page = new char[PAGE_SIZE];

        // 如果 pageNum 小于当前文件的页数，则从文件中读取
        if (pageNum < numPages_) {
            // 将文件指针移动到指定页的开头
            file_.seekg(pageNum * PAGE_SIZE, std::ios::beg);
            // 从文件中读取一页的数据
            if (!file_.read(raw_page, PAGE_SIZE)) {
                // 如果读取失败且不是文件末尾，则抛出异常
                if (!file_.eof()) {
                    delete[] raw_page;
                    throw std::runtime_error("读取文件错误: " + std::string(strerror(errno)));
                }
            }
        }
        else if (pageNum == numPages_) {
            // 如果 pageNum 等于当前文件的页数，则说明是新页，页数加一, 在这个地方不会立马分配内存
            numPages_++;
        }
        else {
            // 分配的内存一定要记得释放
            delete[] raw_page;
            // 如果 pageNum 大于当前文件的页数，则抛出异常，不允许创建不连续的页
            throw std::runtime_error("不能分配非顺序的页");
        }

        // 从 raw_page 中读取节点类型
        uint8_t nodeTypeByte = *reinterpret_cast<uint8_t*>(raw_page);
        NodeType nodeType = static_cast<NodeType>(nodeTypeByte);

        // 根据节点类型创建 Node 对象，并使用 try-catch 捕获可能的内存分配异常
        Node* node;
        try {
            if (nodeType == NodeType::INTERNAL) {
                node = new InternalNode(this, pageNum);
                std::cerr << "创建了一个 InternalNode, 地址: " << node << std::endl;
            }
            else if (nodeType == NodeType::LEAF) {
                node = new LeafNode(this, pageNum);
                std::cerr << "创建了一个 LeafNode, 地址: " << node << std::endl;
            }
            else {
                //对于新创建的数据库，第一页是空的，因此类型是 UNKNOWN，分配一个新节点
                node = new LeafNode(this, pageNum);
                node->getHeader()->setType(NodeType::LEAF);
                node->getHeader()->setRoot(true); // 设置为根节点
                dynamic_cast<LeafNodeHeader*>(node->getHeader())->setNumCells(0); // 初始化单元格数量为 0
                dynamic_cast<LeafNodeHeader*>(node->getHeader())->setNextLeafPageNum(0); // 初始化下一个叶子节点为 0
            }
        }
        catch (const std::bad_alloc& e) {
            std::cerr << "错误: 在 Pager::getPage 中内存分配失败: " << e.what() << std::endl;
            delete[] raw_page;
            // 适当处理错误，例如退出程序或返回 nullptr
            exit(EXIT_FAILURE); // 或者返回 nullptr;
        }

        // 从 raw_page 中读取头部信息
        char* headerStart = raw_page;

        // 根据节点类型，设置头部信息，这里只是进行头部信息的反序列化
        if (nodeType == NodeType::LEAF) {
            auto* header = dynamic_cast<LeafNodeHeader*>(node->getHeader());
            if (header != nullptr) {
                // 反序列化叶节点头部信息
                header->setType(NodeType::LEAF);
                header->setRoot(*reinterpret_cast<bool*>(headerStart + sizeof(uint8_t)));
                header->setParentPageNum(*reinterpret_cast<uint32_t*>(headerStart + sizeof(uint8_t) + sizeof(bool)));
                header->setNumCells(
                    *reinterpret_cast<uint32_t*>(headerStart + sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t)));
                header->setNextLeafPageNum(
                    *reinterpret_cast<uint32_t*>(headerStart + sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t) + sizeof(uint32_t)));
                // 调试信息：打印反序列化后的头部信息
                std::cerr << "反序列化 LeafNode 头部: " << std::endl;
                std::cerr << "  类型: " << (header->getType() == NodeType::LEAF ? "LEAF" : "UNKNOWN") << std::endl;
                std::cerr << "  是否为根: " << (header->isRoot() ? "是" : "否") << std::endl;
                std::cerr << "  父页码: " << header->getParentPageNum() << std::endl;
                std::cerr << "  单元格数量: " << header->getNumCells() << std::endl;
                std::cerr << "  下一个叶节点页码: " << header->getNextLeafPageNum() << std::endl;
            }
            else {
                std::cerr << "错误: 无法将头部转换为 LeafNodeHeader 类型." << std::endl;
                // 可以考虑抛出异常或进行其他错误处理
            }
        }
        else if (nodeType == NodeType::INTERNAL) {
            auto* header = dynamic_cast<InternalNodeHeader*>(node->getHeader());
            if (header != nullptr) {
                // 反序列化内部节点头部信息
                header->setType(NodeType::INTERNAL);
                header->setRoot(*reinterpret_cast<bool*>(headerStart + sizeof(uint8_t)));
                header->setParentPageNum(*reinterpret_cast<uint32_t*>(headerStart + sizeof(uint8_t) + sizeof(bool)));
                header->setNumKeys(
                    *reinterpret_cast<uint32_t*>(headerStart + sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t)));
                header->setRightChildPageNum(
                    *reinterpret_cast<uint32_t*>(headerStart + sizeof(uint8_t) + sizeof(bool) + sizeof(uint32_t) + sizeof(uint32_t)));
            }
            else {
                std::cerr << "错误: 无法将头部转换为 InternalNodeHeader 类型." << std::endl;
                // 可以考虑抛出异常或进行其他错误处理
            }
        }

        pages_[pageNum] = node;
        page_data_[pageNum] = raw_page; // 将 raw_page 存储在 page_data_ 中
        std::cerr << "将页 " << pageNum << " 存入缓存, 地址: " << pages_[pageNum] << std::endl;
        // 调试检查：确保节点头部不为空
        if (node->getHeader() == nullptr) {
            std::cerr << "错误: 节点头部为空!" << std::endl;
        }
    }

    // 返回缓存的页
    return pages_[pageNum];
}

char* Pager::getPageData(uint32_t pageNum)
{
    if (pageNum >= TABLE_MAX_PAGES) {
        std::cerr << "Error: Tried to access page data out of bounds. Page num: " << pageNum << std::endl;
        exit(EXIT_FAILURE);
    }
    return page_data_[pageNum];
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
