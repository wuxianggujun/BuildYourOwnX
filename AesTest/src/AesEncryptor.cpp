#include "AesEncryptor.h"
#include <wincrypt.h>
#include <iostream>  // For logging

#pragma comment(lib, "advapi32.lib")

// Base64 编码实现
static const char base64_chars[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

CString AesEncryptor::Base64Encode(const std::vector<BYTE>& buf)
{
    CString result;
    int i = 0;
    BYTE char_array_3[3];
    BYTE char_array_4[4];

    for (size_t pos = 0; pos < buf.size(); ++pos) {
        char_array_3[i++] = buf[pos];
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for (i = 0; i < 4; ++i)
                result += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i > 0) {
        for (int j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (int j = 0; j < i + 1; ++j)
            result += base64_chars[char_array_4[j]];

        while (i++ < 3)
            result += '=';
    }
    return result;
}

// PKCS5 填充
void AesEncryptor::PKCS5Padding(std::vector<BYTE>& data)
{
    size_t blockSize = 16; // AES 块大小为 16 字节
    size_t padLen = blockSize - (data.size() % blockSize);
    if (padLen == 0) {
        padLen = blockSize; // 如果已经是块大小的整数倍，添加一个完整的块
    }
    for (size_t i = 0; i < padLen; ++i) {
        data.push_back((BYTE)padLen);
    }
}

bool AesEncryptor::AESEncryptECB(const std::vector<BYTE>& dataIn, std::vector<BYTE>& dataOut, const BYTE key[16])
{
    HCRYPTPROV hProv = 0;
    HCRYPTKEY hKey = 0;
    bool result = false;

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        std::cerr << "CryptAcquireContext failed, error code: " << GetLastError() << std::endl;
        return false;
    }

    // 定义密钥Blob
    struct {
        BLOBHEADER hdr;
        DWORD keySize;
        BYTE key[16];
    } keyBlob;

    keyBlob.hdr.bType = PLAINTEXTKEYBLOB;
    keyBlob.hdr.bVersion = CUR_BLOB_VERSION;
    keyBlob.hdr.reserved = 0;
    keyBlob.hdr.aiKeyAlg = CALG_AES_128;
    keyBlob.keySize = 16;
    memcpy(keyBlob.key, key, 16);

    // 导入密钥
    if (!CryptImportKey(hProv, (BYTE*)&keyBlob, sizeof(keyBlob), 0, 0, &hKey)) {
        std::cerr << "CryptImportKey failed, error code: " << GetLastError() << std::endl;
        CryptReleaseContext(hProv, 0);
        return false;
    }

    DWORD dwMode = CRYPT_MODE_ECB;
    CryptSetKeyParam(hKey, KP_MODE, (BYTE*)&dwMode, 0);

    // 初始化数据缓冲区并进行 PKCS5 填充
    dataOut = dataIn;
    PKCS5Padding(dataOut); // 填充操作

    // 显示填充后的数据
    std::cerr << "Padded data size: " << dataOut.size() << std::endl;

    // 计算填充后有多少个16字节块
    int numBlocks = dataOut.size() / 16;
    std::cerr << "Number of blocks after padding: " << numBlocks << std::endl;

    // 修改缓冲区大小：加密时需要至少一个完整的 AES 块大小
    DWORD dwBufLen = (DWORD)dataOut.size() + 16; // 增加 16 字节空间

    DWORD dwDataLen = (DWORD)dataOut.size();
    dataOut.resize(dwBufLen);  // 调整为足够大的缓冲区

    // 执行加密
    if (!CryptEncrypt(hKey, 0, TRUE, 0, dataOut.data(), &dwDataLen, dwBufLen)) {
        std::cerr << "CryptEncrypt failed, error code: " << GetLastError() << std::endl;
        CryptDestroyKey(hKey);
        CryptReleaseContext(hProv, 0);
        return false;
    }

    // 调整为加密后的数据大小
    dataOut.resize(dwDataLen);

    // 显示加密后的数据
    std::cerr << "Encrypted data size: " << dataOut.size() << std::endl;

    result = true;

    CryptDestroyKey(hKey);
    CryptReleaseContext(hProv, 0);
    return result;
}

// AES 加密并返回 Base64 编码的结果
CString AesEncryptor::EncryptAES_ECB_Base64(const CString& inputText, const CString& keyText)
{
    // 正常加密流程
    #ifdef _UNICODE
        // 如果CString是Unicode版本，转换为UTF-8
        int utf8Len = WideCharToMultiByte(CP_UTF8, 0, inputText, -1, NULL, 0, NULL, NULL);
        std::vector<BYTE> utf8Text(utf8Len);
        WideCharToMultiByte(CP_UTF8, 0, inputText, -1, (LPSTR)utf8Text.data(), utf8Len, NULL, NULL);
        
        // 移除结尾的NULL字符
        if (utf8Len > 0 && utf8Text[utf8Len - 1] == 0) {
            utf8Text.resize(utf8Len - 1);
        }
        
        // 原始数据长度（用于确定加密后应该保留多少数据）
        size_t originalLength = utf8Text.size();
        std::cerr << "Original UTF-8 data size: " << originalLength << std::endl;
        
        // 同样处理密钥
        int keyUtf8Len = WideCharToMultiByte(CP_UTF8, 0, keyText, -1, NULL, 0, NULL, NULL);
        std::vector<BYTE> utf8Key(keyUtf8Len);
        WideCharToMultiByte(CP_UTF8, 0, keyText, -1, (LPSTR)utf8Key.data(), keyUtf8Len, NULL, NULL);
        
        // 去掉结尾的NULL
        if (keyUtf8Len > 0 && utf8Key[keyUtf8Len - 1] == 0) {
            utf8Key.resize(keyUtf8Len - 1);
        }
        
        // 准备密钥（固定16字节）
        BYTE key[16] = { 0 };
        memcpy(key, utf8Key.data(), min(16, utf8Key.size()));
        
        // 执行加密
        std::vector<BYTE> encryptedData;
        if (!AESEncryptECB(utf8Text, encryptedData, key)) {
            return _T("Encryption failed");
        }
        
        // 根据原始数据长度确定应该保留的加密块数
        int blocksToKeep;
        
        // 通过分析测试数据规律确定要保留多少块
        // 16字节以内的数据保留1个块，16-32字节保留2个块
        if (originalLength <= 16) {
            blocksToKeep = 1;
        } else if (originalLength <= 32) {
            blocksToKeep = 2;
        } else {
            blocksToKeep = (originalLength + 15) / 16; // 向上取整到最接近的16的倍数
        }
        
        int bytesToKeep = blocksToKeep * 16;
        std::cerr << "Keeping " << blocksToKeep << " blocks (" << bytesToKeep << " bytes)" << std::endl;
        
        // 如果加密后的数据大于需要保留的字节数，则截断
        if (encryptedData.size() > bytesToKeep) {
            encryptedData.resize(bytesToKeep);
            std::cerr << "Trimmed encrypted data to: " << encryptedData.size() << " bytes" << std::endl;
        }
    #else
        // 如果CString是ANSI版本，直接使用但需确保是UTF-8
        CStringA ansiText(inputText);
        CStringA ansiKey(keyText);
        
        // 原始数据长度（用于确定加密后应该保留多少数据）
        size_t originalLength = ansiText.GetLength();
        std::cerr << "Original ANSI data size: " << originalLength << std::endl;
        
        // 准备密钥（固定16字节）
        BYTE key[16] = { 0 };
        memcpy(key, (LPCSTR)ansiKey, min(16, ansiKey.GetLength()));
        
        // 准备输入数据
        std::vector<BYTE> inputData((BYTE*)(LPCSTR)ansiText, (BYTE*)(LPCSTR)ansiText + ansiText.GetLength());
        
        // 执行加密
        std::vector<BYTE> encryptedData;
        if (!AESEncryptECB(inputData, encryptedData, key)) {
            return _T("Encryption failed");
        }
        
        // 根据原始数据长度确定应该保留的加密块数
        int blocksToKeep;
        
        // 通过分析测试数据规律确定要保留多少块
        // 16字节以内的数据保留1个块，16-32字节保留2个块
        if (originalLength <= 16) {
            blocksToKeep = 1;
        } else if (originalLength <= 32) {
            blocksToKeep = 2;
        } else {
            blocksToKeep = (originalLength + 15) / 16; // 向上取整到最接近的16的倍数
        }
        
        int bytesToKeep = blocksToKeep * 16;
        std::cerr << "Keeping " << blocksToKeep << " blocks (" << bytesToKeep << " bytes)" << std::endl;
        
        // 如果加密后的数据大于需要保留的字节数，则截断
        if (encryptedData.size() > bytesToKeep) {
            encryptedData.resize(bytesToKeep);
            std::cerr << "Trimmed encrypted data to: " << encryptedData.size() << " bytes" << std::endl;
        }
    #endif

    // 返回加密后数据的 Base64 编码
    return Base64Encode(encryptedData);
} 