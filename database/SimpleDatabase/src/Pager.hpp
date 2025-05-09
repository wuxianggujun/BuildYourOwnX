#pragma once
#include <fstream>
#include <vector>

#include "Common.hpp"

class Node;
class Cursor;
class Row;

class Pager {
public:
    explicit Pager(const std::string &filename);

    ~Pager();

    Node *getPage(uint32_t pageNum);

    char* getPageData(uint32_t pageNum);

    void flushPage(uint32_t pageNum);

    [[nodiscard]] uint32_t getUnusedPageNum() const;

    uint32_t getNodeMaxKey(Node *node);

    void createNewRoot(uint32_t rightChildPageNum);

    void internalNodeInsert(uint32_t parentPageNum, uint32_t childPageNum);

    void internalNodeSplitAndInsert(uint32_t parentPageNum, uint32_t childPageNum);

    void leafNodeSplitAndInsert(Cursor *cursor, uint32_t key, Row *value);

    void updateInternalNodeKey(uint32_t nodePageNum, uint32_t oldKey, uint32_t newKey);

    friend class Table;
    friend class Cursor;
    friend class InternalNode;
    friend class LeafNode;

private:
    std::fstream file_;
    uint32_t fileLength_;
    uint32_t numPages_;
    std::vector<Node *> pages_;
    char* page_data_[TABLE_MAX_PAGES];
};
