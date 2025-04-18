# AES加密示例

这是一个使用Windows加密API（WinCrypt）实现AES-ECB模式加密并进行Base64编码的示例项目。

## 功能特点

- AES-ECB加密模式
- PKCS5填充
- Base64编码输出
- 支持Unicode文本

## 系统要求

- Windows操作系统
- 支持C++11的编译器
- CMake 3.10或更高版本

## 编译方法

```bash
# 创建构建目录
mkdir build
cd build

# 配置CMake项目
cmake ..

# 构建项目
cmake --build .
```

## 使用示例

```cpp
#include "AesEncryptor.h"
#include <iostream>

int main() {
    std::wstring plaintext = L"Hello, AES Encryption!";
    std::wstring key = L"1234567890123456"; // 16字节密钥
    
    std::wstring encrypted = AesEncryptor::EncryptAES_ECB_Base64(plaintext, key);
    std::wcout << L"加密结果: " << encrypted << std::endl;
    
    return 0;
}
```

## 注意事项

- 本项目仅在Windows环境下可用，因为使用了Windows特定的加密API
- 使用的是AES-128位加密，密钥长度应为16字节
- 如果输入密钥长度不足16字节，将会用零填充；如果超过16字节，将只使用前16字节 