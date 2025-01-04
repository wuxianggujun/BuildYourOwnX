#pragma once

#include <cstdint>
#include <string>

#include "Common.hpp"

class Pager {
public:
    explicit Pager(const std::string &fileName);

    ~Pager();

    void *getPage(uint32_t pageNum);

    void flush(uint32_t pageNum, uint32_t size);

    void flush();

    uint32_t getFileLength() const;

    void* getNewPage();

private:
    int fileDescriptor;
    uint32_t fileLength;
    void *pages[TABLE_MAX_PAGES];
    uint32_t numPages;
    
    // 新增：内部更新 numPages 的方法
    void updateNumPages();
    void allocatePage(uint32_t pageNum); // 新增：专门用于分配新页面的函数
};
