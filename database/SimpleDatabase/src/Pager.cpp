#include "Pager.hpp"

#include <iostream>
#include <cstring>
#include <cerrno>

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "Common.hpp"

Pager::Pager(const std::string &fileName): fileDescriptor(-1), fileLength(0), numPages(0) {
    fileDescriptor = _open(fileName.c_str(),_O_RDWR | _O_CREAT, _S_IWRITE | _S_IREAD);
    if (fileDescriptor == -1) {
        std::cerr << "Cannot open file" << std::endl;
        exit(EXIT_FAILURE);
    }
    fileLength = _lseek(fileDescriptor, 0, SEEK_END);
    updateNumPages();

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pages[i] = nullptr;
    }
    std::cerr << "Pager created. fileDescriptor: " << fileDescriptor << ", fileLength: " << fileLength << ", numPages: " << numPages << std::endl;
}

Pager::~Pager() {
    std::cerr << "Pager destructor called." << std::endl;
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        if (pages[i]) {
            free(pages[i]);
            pages[i] = nullptr;
        }
    }

    int result = _close(fileDescriptor);
    if (result == -1) {
        std::cerr << "Error closing db file." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void *Pager::getPage(uint32_t pageNum) {
    std::cerr << "Pager::getPage called for page: " << pageNum << std::endl;
    if (pageNum > TABLE_MAX_PAGES) {
        std::cerr << "Tried to fetch page number out of bounds. " << pageNum << " > " << TABLE_MAX_PAGES << std::endl;
        exit(EXIT_FAILURE);
    }

    allocatePage(pageNum);
    
    // 如果是新分配的页面（之前没有从文件加载过），不需要读取文件
    if (pageNum < numPages && _lseek(fileDescriptor, pageNum * PAGE_SIZE, SEEK_SET) != -1) {
        int bytesRead = _read(fileDescriptor, pages[pageNum], PAGE_SIZE);
        if (bytesRead == -1) {
            std::cerr << "Error reading file: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    std::cerr << "Pager::getPage returning page: " << pageNum << std::endl;
    return pages[pageNum];
}

void Pager::flush() {
    std::cerr << "Pager::flush (full) called." << std::endl;
    for (uint32_t i = 0; i < numPages; i++) {
        if (pages[i] == nullptr) {
            continue;
        }
        flush(i, PAGE_SIZE);
    }
    std::cerr << "Pager::flush (full) finished." << std::endl;
}

void Pager::flush(uint32_t pageNum, uint32_t size) {
    std::cerr << "Pager::flush (page) called for page: " << pageNum << ", size: " << size << std::endl;

    if (pages[pageNum] == nullptr) {
        std::cerr << "Tried to flush null page" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (fileDescriptor == -1) {
        std::cerr << "Invalid file descriptor." << std::endl;
        exit(EXIT_FAILURE);
    }

    _lseek(fileDescriptor, pageNum * PAGE_SIZE, SEEK_SET);

    // 写入整个页面或页面的实际大小
    uint32_t pageSizeToWrite = PAGE_SIZE;
    // 如果是最后一页，且未满，则只写入实际的数据大小
    if (pageNum == numPages - 1 && fileLength % PAGE_SIZE != 0) {
        pageSizeToWrite = fileLength % PAGE_SIZE;
    }

    // 检查是否超出 fileLength
    if (pageNum * PAGE_SIZE + pageSizeToWrite > fileLength) {
        std::cerr << "Error: Attempting to write beyond file length." << std::endl;
        std::cerr << "Page Number: " << pageNum << std::endl;
        std::cerr << "File Length: " << fileLength << std::endl;
        std::cerr << "Num Pages: " << numPages << std::endl;
        std::cerr << "Page Size To Write: " << pageSizeToWrite << std::endl;
        exit(EXIT_FAILURE);
    }

    int bytesWritten = _write(fileDescriptor, pages[pageNum], pageSizeToWrite);

    if (bytesWritten == -1) {
        std::cerr << "Error writing file: " << errno << " at page " << pageNum << std::endl;
        perror("write");
        exit(EXIT_FAILURE);
    }

    free(pages[pageNum]);
    pages[pageNum] = nullptr;
    std::cerr << "Pager::flush (page) finished for page: " << pageNum << std::endl;
}

uint32_t Pager::getFileLength() const {
    return fileLength;
}

void *Pager::getNewPage() {
    std::cerr << "Pager::getNewPage called." << std::endl;
    if (numPages >= TABLE_MAX_PAGES) {
        std::cerr << "Error: Maximum number of pages reached." << std::endl;
        exit(EXIT_FAILURE);
    }
    // 分配新页面
    void *newPage = malloc(PAGE_SIZE);
    if (newPage == nullptr) {
        std::cerr << "Error: Memory allocation failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    // 更新文件长度和页数
    fileLength += PAGE_SIZE;
    updateNumPages();

    pages[numPages - 1] = newPage;
    std::cerr << "Pager::getNewPage allocated new page at index: " << numPages - 1 << ", new fileLength: " << fileLength << ", new numPages: " << numPages << std::endl;
    return newPage;
}

void Pager::updateNumPages() {
    numPages = (fileLength + PAGE_SIZE - 1) / PAGE_SIZE;
    std::cerr << "Pager::updateNumPages called. numPages updated to: " << numPages << std::endl;
}

void Pager::allocatePage(uint32_t pageNum) {
    std::cerr << "Pager::allocatePage called for page: " << pageNum << std::endl;
    if (pages[pageNum] == nullptr) {
        // 更新 fileLength 和 numPages
        if (pageNum >= numPages) {
            fileLength = (pageNum + 1) * PAGE_SIZE; // 直接计算 fileLength
            updateNumPages();
            std::cerr << "Pager::allocatePage updated fileLength to: " << fileLength << ", numPages to: " << numPages << std::endl;
        }
        void* page = malloc(PAGE_SIZE);
        if (page == nullptr) {
            std::cerr << "Error: Memory allocation failed." << std::endl;
            exit(EXIT_FAILURE);
        }
        pages[pageNum] = page;
        std::cerr << "Pager::allocatePage allocated page at index: " << pageNum << std::endl;
    }
}
