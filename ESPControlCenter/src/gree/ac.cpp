#include "AC.hpp"

#include "crypto/aes.h"

#include <libb64/cdecode.h>
#include <libb64/cencode.h>
#include <WiFiUdp.h>

static const int AesBlockSizeBytes = 16;
static const int AesBufferSize = 2048;
static const int Base64BufferSize = 4096;

struct plain_packet
{
    static const char data[] = { R"()" };
};

void dumpBuffer(char* buffer, int len)
{
    Serial.printf("Buffer length: %u\n", len);
    for (int i = 0; i < len; ++i)
    {
        if (i > 0 && i % 8 == 0)
            Serial.println("");
        Serial.printf("%02X ", buffer[i]);
    }
}

namespace gree
{

AC::AC(char* key, const IPAddress& ip)
    : m_ip(ip)
{
    m_aes.set_key(reinterpret_cast<byte*>(key), 128);

    char input[Base64BufferSize] = { 0 };
    char output[Base64BufferSize] = { 0 };

    snprintf(input, sizeof(input), "Hello");
    encryptBuffer(input, output, sizeof(input));
}

void AC::encryptBuffer(const char* plainText, char* output, int outputBufLen)
{
    char aesBuffer[AesBufferSize] = { 0 };

    strncpy(aesBuffer, plainText, sizeof(aesBuffer));
    int paddedLength = pkcs7Pad(aesBuffer, sizeof(aesBuffer));
    if (paddedLength == 0)
    { 
        Serial.printf("Cannot pad text, buffer too small (%d)\n", outputBufLen);
        return;
    }

    Serial.println("Padded plain text:");
    dumpBuffer(aesBuffer, paddedLength);
    Serial.println("");

    int blocks = paddedLength / 16 + (paddedLength % 16 > 0 ? 1 : 0);
    Serial.printf("Blocks to encrypt: %d\n", blocks);

    auto pbuf = reinterpret_cast<uint8_t*>(aesBuffer);
    for (int i = 0; i < blocks; ++i)
    {
        Serial.printf("Encrypting block %d\n", i);
        m_aes.encrypt(pbuf, pbuf);
        pbuf += 16;
    }

    Serial.println("Cipher text:");
    dumpBuffer(aesBuffer, sizeof(aesBuffer));
    Serial.println("");

    char base64Buffer[Base64BufferSize] = { 0 };

    base64_encodestate encodeState;
    base64_init_encodestate_nonewlines(&encodeState);
    int base64Length = base64_encode_block(aesBuffer, paddedLength, base64Buffer, &encodeState);
    base64_encode_blockend((base64Buffer + base64Length), &encodeState);

    Serial.printf("Base64 encoded text: %s\n", base64Buffer);
}

void AC::decryptBuffer(char* input, char* plainText, int outputBufLen)
{

}

int AC::pkcs7Pad(char* buf, int outputBufLen)
{
    int len = strlen(buf);
    Serial.printf("PKCS7 len: %d\n", len);

    int padLen = 0;
    if (int mod = len % 16)
        padLen = 16 - mod;

    Serial.printf("PKCS7 pad len: %d\n", padLen);
    
    int paddedLength = len + padLen;
    Serial.printf("PKCS7 padded len: %d\n", paddedLength);    

    if (paddedLength > outputBufLen)
        return 0;

    for (int i = len; i < paddedLength; ++i)
        buf[i] = padLen;

    return paddedLength;
}

}