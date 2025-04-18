#include "AesEncryptor.h"
#include <iostream>
#include <Windows.h>

int main() {
    // 设置控制台输出为UTF-8编码
    SetConsoleOutputCP(CP_UTF8);
    
    CString password = _T("wuxianggujun3344207732@qq.com");
    CString key = _T("icdmicdmicdmicdm");
    
    std::cout << "原文: " << password<< std::endl;
    std::cout << "密钥: " << key << std::endl;
    
    CString result = AesEncryptor::EncryptAES_ECB_Base64(password, key);
    
    // 转换CString为std::string以便于输出
    CStringA resultA(result);
    std::cout << "加密后: " << (LPCSTR)resultA << std::endl;
    
    // 输出期望结果以进行比较
    std::cout << "期望结果: y4Yf5KQ/+3DbdPdzVbK5Jg==" << std::endl;
    
    return 0;
} 