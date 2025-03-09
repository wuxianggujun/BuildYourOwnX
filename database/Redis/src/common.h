#pragma once

#include <stdint.h>
#include <stddef.h>


// intrusive data structure
// 定义一个用于获取包含指针的结构体的容器的模板函数
template <typename T, typename M>
T* container_of(M* ptr, T M::* member) {
    return reinterpret_cast<T*>(
        reinterpret_cast<char*>(ptr) - offsetof(T, member) // 计算偏移量
    );
}

// FNV hash
// FNV哈希函数
inline uint64_t str_hash(const uint8_t *data, size_t len) {
    uint32_t h = 0x811C9DC5; // 初始化哈希值
    for (size_t i = 0; i < len; i++) {
        h = (h + data[i]) * 0x01000193; // 更新哈希值
    }
    return h; // 返回哈希值
}
