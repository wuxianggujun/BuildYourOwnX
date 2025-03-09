// stdlib
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <cmath>   // isnan
// system
#include <ctime>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
// C++
#include <string>
#include <vector>
// proj
#include "common.h"
#include "hashtable.h"
#include "zset.h"
#include "list.h"
#include "heap.h"
#include "thread_pool.h"

// 打印消息
static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

// 打印错误消息
static void msg_errno(const char *msg) {
    fprintf(stderr, "[errno:%d] %s\n", errno, msg);
}

// 处理致命错误
static void die(const char *msg) {
    fprintf(stderr, "[%d] %s\n", errno, msg);
    abort();
}

// 获取单调时间（毫秒）
static uint64_t get_monotonic_msec() {
    struct timespec tv = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return uint64_t(tv.tv_sec) * 1000 + tv.tv_nsec / 1000 / 1000;
}

// 设置文件描述符为非阻塞模式
static void fd_set_nb(int fd) {
    errno = 0;
    int flags = fcntl(fd, F_GETFL, 0);
    if (errno) {
        die("fcntl error");
        return;
    }

    flags |= O_NONBLOCK; // 设置非阻塞标志

    errno = 0;
    (void)fcntl(fd, F_SETFL, flags);
    if (errno) {
        die("fcntl error");
    }
}

// 最大消息大小
const size_t k_max_msg = 32 << 20;  // 可能大于内核缓冲区

typedef std::vector<uint8_t> Buffer; // 定义缓冲区类型

// 向缓冲区添加数据
static void buf_append(Buffer &buf, const uint8_t *data, size_t len) {
    buf.insert(buf.end(), data, data + len);
}

// 从缓冲区中移除数据
static void buf_consume(Buffer &buf, size_t n) {
    buf.erase(buf.begin(), buf.begin() + n);
}

// 连接结构体
struct Conn {
    int fd = -1; // 文件描述符
    bool want_read = false; // 是否希望读取
    bool want_write = false; // 是否希望写入
    bool want_close = false; // 是否希望关闭
    Buffer incoming; // 输入缓冲区
    Buffer outgoing; // 输出缓冲区
    uint64_t last_active_ms = 0; // 最后活动时间
    DList idle_node; // 空闲节点
};

// 全局状态
static struct {
    HMap db; // 数据库
    std::vector<Conn *> fd2conn; // 文件描述符到连接的映射
    DList idle_list; // 空闲连接列表
    std::vector<HeapItem> heap; // 定时器堆
    TheadPool thread_pool; // 线程池
} g_data;

// 处理接受连接的回调
static int32_t handle_accept(int fd) {
    // 接受连接
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
    if (connfd < 0) {
        msg_errno("accept() error");
        return -1;
    }
    uint32_t ip = client_addr.sin_addr.s_addr;
    fprintf(stderr, "new client from %u.%u.%u.%u:%u\n",
        ip & 255, (ip >> 8) & 255, (ip >> 16) & 255, ip >> 24,
        ntohs(client_addr.sin_port)
    );

    // 设置新连接的文件描述符为非阻塞模式
    fd_set_nb(connfd);

    // 创建连接结构体
    Conn *conn = new Conn();
    conn->fd = connfd;
    conn->want_read = true;
    conn->last_active_ms = get_monotonic_msec();
    dlist_insert_before(&g_data.idle_list, &conn->idle_node);

    // 将其放入映射中
    if (g_data.fd2conn.size() <= (size_t)conn->fd) {
        g_data.fd2conn.resize(conn->fd + 1);
    }
    assert(!g_data.fd2conn[conn->fd]);
    g_data.fd2conn[conn->fd] = conn;
    return 0;
}

// 销毁连接
static void conn_destroy(Conn *conn) {
    (void)close(conn->fd);
    g_data.fd2conn[conn->fd] = NULL;
    dlist_detach(&conn->idle_node);
    delete conn;
}

// 最大参数数量
const size_t k_max_args = 200 * 1000;

// 读取32位无符号整数
static bool read_u32(const uint8_t *&cur, const uint8_t *end, uint32_t &out) {
    if (cur + 4 > end) {
        return false;
    }
    memcpy(&out, cur, 4);
    cur += 4;
    return true;
}

// 读取字符串
static bool read_str(const uint8_t *&cur, const uint8_t *end, size_t n, std::string &out) {
    if (cur + n > end) {
        return false;
    }
    out.assign(cur, cur + n);
    cur += n;
    return true;
}

// 解析请求
static int32_t parse_req(const uint8_t *data, size_t size, std::vector<std::string> &out) {
    const uint8_t *end = data + size;
    uint32_t nstr = 0;
    if (!read_u32(data, end, nstr)) {
        return -1;
    }
    if (nstr > k_max_args) {
        return -1;  // 安全限制
    }

    while (out.size() < nstr) {
        uint32_t len = 0;
        if (!read_u32(data, end, len)) {
            return -1;
        }
        out.push_back(std::string());
        if (!read_str(data, end, len, out.back())) {
            return -1;
        }
    }
    if (data != end) {
        return -1;  // 尾部垃圾数据
    }
    return 0;
}

// 错误代码
enum {
    ERR_UNKNOWN = 1,    // 未知命令
    ERR_TOO_BIG = 2,    // 响应过大
    ERR_BAD_TYP = 3,    // 意外值类型
    ERR_BAD_ARG = 4,    // 错误参数
};

// 数据类型
enum {
    TAG_NIL = 0,    // 空值
    TAG_ERR = 1,    // 错误代码 + 消息
    TAG_STR = 2,    // 字符串
    TAG_INT = 3,    // 整数
    TAG_DBL = 4,    // 双精度浮点数
    TAG_ARR = 5,    // 数组
};

// 序列化帮助函数
static void buf_append_u8(Buffer &buf, uint8_t data) {
    buf.push_back(data);
}
static void buf_append_u32(Buffer &buf, uint32_t data) {
    buf_append(buf, (const uint8_t *)&data, 4);
}
static void buf_append_i64(Buffer &buf, int64_t data) {
    buf_append(buf, (const uint8_t *)&data, 8);
}
static void buf_append_dbl(Buffer &buf, double data) {
    buf_append(buf, (const uint8_t *)&data, 8);
}

// 开始序列化
static void out_nil(Buffer &out) {
    buf_append_u8(out, TAG_NIL);
}
static void out_str(Buffer &out, const char *s, size_t size) {
    buf_append_u8(out, TAG_STR);
    buf_append_u32(out, (uint32_t)size);
    buf_append(out, (const uint8_t *)s, size);
}
static void out_int(Buffer &out, int64_t val) {
    buf_append_u8(out, TAG_INT);
    buf_append_i64(out, val);
}
static void out_dbl(Buffer &out, double val) {
    buf_append_u8(out, TAG_DBL);
    buf_append_dbl(out, val);
}
static void out_err(Buffer &out, uint32_t code, const std::string &msg) {
    buf_append_u8(out, TAG_ERR);
    buf_append_u32(out, code);
    buf_append_u32(out, (uint32_t)msg.size());
    buf_append(out, (const uint8_t *)msg.data(), msg.size());
}
static void out_arr(Buffer &out, uint32_t n) {
    buf_append_u8(out, TAG_ARR);
    buf_append_u32(out, n);
}
static size_t out_begin_arr(Buffer &out) {
    out.push_back(TAG_ARR);
    buf_append_u32(out, 0);     // 由 out_end_arr() 填充
    return out.size() - 4;      // ctx 参数
}
static void out_end_arr(Buffer &out, size_t ctx, uint32_t n) {
    assert(out[ctx - 1] == TAG_ARR);
    memcpy(&out[ctx], &n, 4);
}

// 处理请求
static bool try_one_request(Conn *conn) {
    // 尝试解析协议：消息头
    if (conn->incoming.size() < 4) {
        return false;   // 需要读取
    }
    uint32_t len = 0;
    memcpy(&len, conn->incoming.data(), 4);
    if (len > k_max_msg) {
        msg("too long");
        conn->want_close = true;
        return false;   // 需要关闭
    }
    // 消息体
    if (4 + len > conn->incoming.size()) {
        return false;   // 需要读取
    }
    const uint8_t *request = &conn->incoming[4];

    // 处理请求
    std::vector<std::string> cmd;
    if (parse_req(request, len, cmd) < 0) {
        msg("bad request");
        conn->want_close = true;
        return false;   // 需要关闭
    }
    size_t header_pos = 0;
    response_begin(conn->outgoing, &header_pos);
    do_request(cmd, conn->outgoing);
    response_end(conn->outgoing, header_pos);

    // 请求处理完成！移除请求消息。
    buf_consume(conn->incoming, 4 + len);
    return true;        // 成功
}

