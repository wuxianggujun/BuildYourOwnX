#include "AesEncryptor.h"
#include <iostream>
#include <Windows.h>

int main() {
    // 设置控制台输出为UTF-8编码
    SetConsoleOutputCP(CP_UTF8);
    
    CString password = _T("wuxianggujun3344207732@qq.com");
    CString key = _T("icdmicdmicdmicdm");
    
    std::cout << "原文: wuxianggujun3344207732@qq.com" << std::endl;
    std::cout << "密钥: icdmicdmicdmicdm" << std::endl;
    
    CString result = AesEncryptor::EncryptAES_ECB_Base64(password, key);
    
    // 转换CString为std::string以便于输出
    CStringA resultA(result);
    std::cout << "加密后: " << (LPCSTR)resultA << std::endl;
    
    // 输出期望结果以进行比较
    std::cout << "期望结果: DczTQkaks3Uq5kabduPf6q5u8Xo8ith0lh8zv3l5e6M=" << std::endl;
    
    // 测试其他已知的输入
    CString password2 = _T("Xj@12345");
    CString result2 = AesEncryptor::EncryptAES_ECB_Base64(password2, key);
    CStringA result2A(result2);
    std::cout << "\n测试2 - 原文: Xj@12345" << std::endl;
    std::cout << "加密后: " << (LPCSTR)result2A << std::endl;
    std::cout << "期望结果: y4Yf5KQ/+3DbdPdzVbK5Jg==" << std::endl;
    
    CString password3 = _T("wuxianggujun3344207732@qq.comandxiaojuzitapiguzhenda");
    CString result3 = AesEncryptor::EncryptAES_ECB_Base64(password3, key);
    CStringA result3A(result3);
    std::cout << "\n测试3 - 原文: wuxianggujun3344207732@qq.comandxiaojuzitapiguzhenda" << std::endl;
    std::cout << "加密后: " << (LPCSTR)result3A << std::endl;
    std::cout << "期望结果: DczTQkaks3Uq5kabduPf6gfArYxvLjij/SaGjeS/QCz3dBhH0bcjmzS1GETVQ1FJzG7n7Unc9El6T6iXdLos+Q==" << std::endl;
    
    return 0;
} 