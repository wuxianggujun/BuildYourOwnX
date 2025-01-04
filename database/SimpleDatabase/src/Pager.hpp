#pragma once

#include <cstdint>
#include <string>

#include "Common.hpp"

class Pager {
public:
    explicit Pager(const std::string &fileName);

    ~Pager();

    void *getPage(uint32_t pageNum);

    void flush();

    uint32_t getFileLength() const;

private:
    int fileDescriptor;
    uint32_t fileLength;
    void *pages[TABLE_MAX_PAGES];
    uint32_t numPages;
};
