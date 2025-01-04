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
    numPages = fileLength / PAGE_SIZE;
    if (fileLength % PAGE_SIZE != 0) {
        numPages++;
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pages[i] = nullptr;
    }
}

Pager::~Pager() {
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
    if (pageNum > TABLE_MAX_PAGES) {
        std::cerr << "Tried to fetch page number out of bounds. " << pageNum << " > " << TABLE_MAX_PAGES << std::endl;
        exit(EXIT_FAILURE);
    }

    if (pages[pageNum] == nullptr) {
        void *page = malloc(PAGE_SIZE);
        if (pageNum <= numPages) {
            _lseek(fileDescriptor, pageNum * PAGE_SIZE,SEEK_SET);
            int bytesRead = _read(fileDescriptor, page, PAGE_SIZE);
            if (bytesRead == -1) {
                std::cerr << "Error reading file: " << errno << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        pages[pageNum] = page;
    }
    return pages[pageNum];
}

void Pager::flush() {
    for (uint32_t i = 0; i < numPages; i++) {
        if (pages[i] == nullptr) {
            continue;
        }

        _lseek(fileDescriptor, i * PAGE_SIZE,SEEK_SET);
        int bytesWritten = _write(fileDescriptor, pages[i], PAGE_SIZE);

        if (bytesWritten == -1) {
            std::cerr << "Error writing file: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

uint32_t Pager::getFileLength() const {
    return fileLength;
}
