#pragma once

#include "crypto/aes.h"
#include <IPAddress.h>

namespace gree
{

class AC
{
public:
    AC(char* key, const IPAddress& ip);

private:
    AES m_aes;
    IPAddress m_ip;

    void encryptBuffer(const char* plainText, char* output, int maxLen);
    void decryptBuffer(char* input, char* plainText, int maxLen);
    int pkcs7Pad(char* buf, int maxLen);
};

}