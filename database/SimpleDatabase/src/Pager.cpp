#include "Pager.hpp"

#include <iostream>
#include <cstring>
#include <cerrno>

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "BTree.hpp"
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
        pages_dirty[i] = false; // 初始化为 false
    }

    std::cerr << "Pager created. fileDescriptor: " << fileDescriptor << ", fileLength: " << fileLength << ", numPages: "
            << numPages << std::endl;
}

Pager::~Pager() {
    std::cerr << "Pager destructor called." << std::endl;
    for (auto &page: pages) {
        if (page) {
            free(page);
            page = nullptr;
        }
    }

    int result = _close(fileDescriptor);
    if (result == -1) {
        std::cerr << "Error closing db file." << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Pager.cpp
void *Pager::getPage(uint32_t page_num) {
    if (page_num > numPages) {
        std::cerr << "Tried to fetch page number out of bounds. " << page_num << " > " << numPages << std::endl;
        exit(EXIT_FAILURE);
    }

    if (pages[page_num] == nullptr) {
        allocatePage(page_num);
        // 如果是新页面，则初始化为叶节点
        if (page_num >= numPages) {
            std::cout << "Pager::updateNumPages called. numPages updated to: " << numPages + 1 << std::endl;
            numPages = page_num + 1;
            initialize_leaf_node(pages[page_num]); // 初始化为叶节点
        }
    }
    std::cout << "Pager::getPage returning page: " << page_num << std::endl;
    return pages[page_num];
}

//Pager.cpp
void Pager::flush() {
    if (fileDescriptor == -1) {
        std::cerr << "Attempt to flush a closed file." << std::endl;
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < numPages; i++) {
        if (pages[i] != nullptr && pages_dirty[i]) {
            // 只写回被修改的页面
            off_t offset = lseek(fileDescriptor, i * PAGE_SIZE, SEEK_SET);
            if (offset == -1) {
                std::cerr << "Error seeking: " << errno << std::endl;
                exit(EXIT_FAILURE);
            }

            int bytes_written = write(fileDescriptor, pages[i], PAGE_SIZE);
            if (bytes_written == -1) {
                std::cerr << "Error writing: " << errno << std::endl;
                exit(EXIT_FAILURE);
            }
            pages_dirty[i] = false; // 写回后，清除 dirty 标记
        }
    }

    int result = close(fileDescriptor);
    if (result == -1) {
        std::cerr << "Error closing db file." << std::endl;
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void *page = pages[i];
        if (page) {
            free(page);
            pages[i] = nullptr;
        }
    }
    std::cout << "Pager closed." << std::endl;
}

void Pager::flush(uint32_t pageNum, uint32_t size) {
    flushPage(pageNum);
}

uint32_t Pager::getFileLength() const {
    return fileLength;
}

// void *Pager::getNewPage() {
//     std::cerr << "Pager::getNewPage called." << std::endl;
//     if (numPages >= TABLE_MAX_PAGES) {
//         std::cerr << "Error: Maximum number of pages reached." << std::endl;
//         exit(EXIT_FAILURE);
//     }
//
//     _lseek(fileDescriptor,0,SEEK_END);
//     uint32_t newPageSize = PAGE_SIZE;
//     char emptyPage[newPageSize];
//     memset(emptyPage, 0, newPageSize);
//     
//     int bytesWritten = _write(fileDescriptor, emptyPage, newPageSize);
//
//     if (bytesWritten == -1) {
//         std::cerr << "Error appending new page to file: " << errno << " ("<<strerror(errno)<<")" << std::endl;
//         exit(EXIT_FAILURE);
//     }
//     
//     // 更新文件长度和页数
//     fileLength += PAGE_SIZE;
//     updateNumPages();
//
//     void* page = malloc(PAGE_SIZE);
//     pages[numPages - 1] = page;
//     
//     std::cerr << "Pager::getNewPage allocated new page at index: " << numPages - 1 << ", new fileLength: " << fileLength << ", new numPages: " << numPages << std::endl;
//     return pages[numPages - 1];
// }

void Pager::allocatePage(uint32_t pageNum) {
    std::cerr << "Pager::allocatePage called for page: " << pageNum << std::endl;
    if (pages[pageNum] == nullptr) {
        void *page = malloc(PAGE_SIZE);
        if (page == nullptr) {
            std::cerr << "Error: Memory allocation failed." << std::endl;
            exit(EXIT_FAILURE);
        }
        pages[pageNum] = page;

        // 如果是新分配的页面，则需要扩展文件
        if (pageNum >= numPages) {
            std::cerr << "Pager::allocatePage allocating new page at index: " << pageNum << std::endl;

            // 扩展文件
            if (extendFile(PAGE_SIZE) != 0) {
                std::cerr << "Error: Failed to extend file." << std::endl;
                free(page);
                pages[pageNum] = nullptr;
                exit(EXIT_FAILURE);
            }

            numPages = pageNum + 1;
            std::cerr << "Pager::allocatePage updated fileLength to: " << fileLength << ", numPages to: " << numPages <<
                    std::endl;
        }

        std::cerr << "Pager::allocatePage allocated page at index: " << pageNum << std::endl;
    }
}

int Pager::extendFile(uint32_t size) {
    // 定位到文件末尾
    if (_lseek(fileDescriptor, 0, SEEK_END) == -1) {
        std::cerr << "Error seeking to end of file: " << errno << " (" << strerror(errno) << ")" << std::endl;
        return -1;
    }

    // 动态分配内存
    char *emptyBuffer = new char[size];
    if (emptyBuffer == nullptr) {
        std::cerr << "Error: Memory allocation failed." << std::endl;
        return -1;
    }
    memset(emptyBuffer, 0, size);

    int bytesWritten = _write(fileDescriptor, emptyBuffer, size);
    if (bytesWritten == -1) {
        std::cerr << "Error extending file: " << errno << " (" << strerror(errno) << ")" << std::endl;
        return -1;
    }

    // 更新文件长度
    fileLength += bytesWritten;

    // 释放内存
    delete[] emptyBuffer;

    return 0;
}

uint32_t Pager::getNumPages() const {
    return numPages;
}

void Pager::markPageDirty(uint32_t page_num) {
    if (page_num < TABLE_MAX_PAGES) {
        pages_dirty[page_num] = true;
    }
}

void Pager::updateNumPages() {
    numPages = (fileLength + PAGE_SIZE - 1) / PAGE_SIZE;
    std::cerr << "Pager::updateNumPages called. numPages updated to: " << numPages << std::endl;
}

/*void Pager::allocatePage(uint32_t pageNum) {
    std::cerr << "Pager::allocatePage called for page: " << pageNum << std::endl;
    if (pages[pageNum] == nullptr) {
        // 更新 fileLength 和 numPages
        if (pageNum >= numPages) {
            std::cerr << "Pager::allocatePage allocating new page at index: " << pageNum << std::endl;
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
}*/

void Pager::flushPage(uint32_t pageNum) {
    std::cerr << "Pager::flushPage called for page: " << pageNum << std::endl;
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

    std::cerr << "Pager::flushPage finished for page: " << pageNum << std::endl;
}
