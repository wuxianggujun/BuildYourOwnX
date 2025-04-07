#include <iostream>       // 用于 std::cout 和 std::endl
#include <string>         // 用于 std::string
#include <vector>         // 用于 std::vector
#include <filesystem>     // C++17 文件系统库 (用于目录迭代、路径操作、文件检查)
#include <algorithm>      // 用于 std::sort
#include <system_error>   // 用于捕获文件系统错误

// 为 std::filesystem 创建命名空间别名，方便使用
namespace fs = std::filesystem;

/**
 * @brief 递归打印指定目录及其子目录下的所有文件名。
 *
 * @param dir_path 要扫描的目录路径。
 */
void printnames(const fs::path& dir_path) {
    // 检查路径是否存在且确实是一个目录
    std::error_code ec; // 用于接收错误码，避免抛出异常
    if (!fs::exists(dir_path, ec) || !fs::is_directory(dir_path, ec) || ec) {
        // 如果路径无效或是访问出错，打印错误信息并返回
        // 使用 std::cerr 输出错误信息
        std::cerr << "Error: Cannot access directory or path is invalid: "
                  << dir_path.string() << " (Code: " << ec.message() << ")" << std::endl;
        return;
    }

    // 1. 获取目录中的所有条目 (文件和子目录)
    std::vector<fs::path> entries;
    try {
        for (const auto& entry : fs::directory_iterator(dir_path)) {
            entries.push_back(entry.path());
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error iterating directory " << dir_path.string() << ": " << e.what() << std::endl;
        return; // 发生错误则停止处理此目录
    }

    // 2. 对条目进行排序 (按完整路径排序，这与 Python sorted(listdir()) 行为一致)
    std::sort(entries.begin(), entries.end());

    // 3. 遍历排序后的条目
    for (const auto& fullpath : entries) {
        try {
            // 检查当前条目是文件还是目录
            if (fs::is_regular_file(fullpath)) {
                // 如果是文件，打印文件名 (不含路径部分)
                // fullpath.filename() 获取路径的最后一部分 (文件名)
                // .string() 将路径对象转换为字符串以便打印
                std::cout << fullpath.filename().string() << std::endl;
            } else if (fs::is_directory(fullpath)) {
                // 如果是目录，递归调用 printnames 处理子目录
                printnames(fullpath);
            }
            // 注意：此代码（和原始 Python 代码）忽略了符号链接、块设备等其他类型的文件系统对象。
            // 如果需要处理它们，可以添加额外的 else if (fs::is_symlink(fullpath)) 等检查。

        } catch (const fs::filesystem_error& e) {
            // 捕获检查文件类型或访问时可能发生的错误
            std::cerr << "Error processing path " << fullpath.string() << ": " << e.what() << std::endl;
            // 可以选择继续处理下一个条目 (continue;) 或停止
            continue;
        }
    }
}

int main() {
    // 调用函数，开始扫描 "CMakeFiles" 目录
    // 假设 "CMakeFiles" 目录存在于程序运行的当前工作目录下
    std::string start_dir = "CMakeFiles";
    std::cout << "Listing files in directory: " << start_dir << std::endl;
    std::cout << "--- Start ---" << std::endl;

    try {
        printnames(start_dir); // 将字符串隐式转换为 fs::path
    } catch (const std::exception& e) {
        // 捕获可能未在 printnames 内部处理的其他异常
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        return 1; // 返回非零表示错误
    }

    std::cout << "--- End ---" << std::endl;
    return 0; // 返回 0 表示成功
}
