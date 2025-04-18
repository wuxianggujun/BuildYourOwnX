#pragma once
#include <vector>
#include <string>
#include <Windows.h>
#include <atlstr.h>  // 添加对CString的支持

class AesEncryptor {
public:
    static CString EncryptAES_ECB_Base64(const CString& inputText, const CString& keyText);

private:
    static CString Base64Encode(const std::vector<BYTE>& buf);
    static void PKCS5Padding(std::vector<BYTE>& data);
    static bool AESEncryptECB(const std::vector<BYTE>& dataIn, std::vector<BYTE>& dataOut, const BYTE key[16]);
}; 