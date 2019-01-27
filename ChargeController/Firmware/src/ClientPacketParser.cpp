#include "ClientPacketParser.h"

#include <Arduino.h>

#include <algorithm>
#include <cstring>

static const uint16_t HEADER_MAGIC = 'K' | 'T' << 8;
static const uint16_t HEADER_END_MAGIC = 'T' | 'K' << 8;

void ClientPacketParser::reset()
{
    memset(m_buffer, 0, sizeof(m_buffer));
    m_bufferCursor = 0;
    m_readCursor = 0;
    m_completed = false;
#ifdef HEADER_DIRECT_ACCESS
    m_packetHeader = nullptr;
#else
    m_headerOk = false;
#endif
    m_payload = nullptr;
}

void ClientPacketParser::input(const char* data, const uint16_t length)
{
    if (m_completed)
        return;

    Serial.printf("Parser: input, length=%d\r\n", length);

    uint16_t copyAmount = std::min(static_cast<uint16_t>(sizeof(m_buffer) - m_bufferCursor), length);

    Serial.printf("Parser: input, copyAmount=%d\r\n", copyAmount);

    if (copyAmount == 0)
        return;

    Serial.printf("Parser: input, copying to %p, m_buffer=%p\r\n", m_buffer + m_bufferCursor, m_buffer);

    memcpy(m_buffer + m_bufferCursor, data, copyAmount);
    m_bufferCursor += copyAmount;

    Serial.printf("Parser: input, new buffer cursor = %d\r\n", m_bufferCursor);

    processBuffer();
}

//uint16_t ClientPacketParser::availableBufferSpace() const
//{
//    return sizeof(m_buffer) - m_bufferCursor;
//}
//
//char* ClientPacketParser::inputBuffer()
//{
//    return m_buffer + m_bufferCursor;
//}

bool ClientPacketParser::isCompleted() const
{
    return m_completed;
}

const char* ClientPacketParser::payload() const
{
    return m_payload;
}

void ClientPacketParser::processBuffer()
{
    if (m_completed)
        return;

#ifdef HEADER_DIRECT_ACCESS
    if (!m_packetHeader)
#else
    if (!m_headerOk)
#endif
    {
        Serial.println("Parser: processBuffer, reading header");

        while (m_bufferCursor - m_readCursor >= sizeof(ClientPacketHeader))
        {

#ifdef HEADER_DIRECT_ACCESS
            /*const*/ char* data = m_buffer + m_readCursor;

            Serial.printf("Parser: processBuffer, looking at %p, readCursor=%d\r\n", data, m_readCursor);

            auto header = reinterpret_cast</*const*/ ClientPacketHeader*>(data);

            Serial.printf("Parser: processBuffer, header size = %d\r\n", sizeof(ClientPacketHeader));
            Serial.printf("Parser: processBuffer, header=%p\r\n", header);
            Serial.printf("Parser: processBuffer, first byte = %x\r\n", static_cast<uint8_t>(*data));
            Serial.printf("Parser: processBuffer, header magic=%x\r\n", header->magic);
            Serial.printf("Parser: processBuffer, header endMagic=%x\r\n", header->endMagic);

            if (header->magic == HeaderMagic && header->endMagic == HeaderEndMagic)
            {
                Serial.printf("Parser: processBuffer, header found at %p, readCursor=%d\r\n", data, m_readCursor);

                // DEBUG receive payload length as a hexadecimal number
                header->endMagic = 0;
                uint16_t payloadLength = std::strtoul(data + 2, nullptr, 16);
                if (payloadLength <= sizeof(m_buffer) - m_bufferCursor)
                    header->payloadLength = payloadLength;
                // DEBUG

                Serial.printf("Parser: processBuffer, payloadLength=%d\r\n", payloadLength);

                if (header->payloadLength > sizeof(m_buffer) - m_bufferCursor)
                    break;

                Serial.printf("Parser: processBuffer, header is valid\r\n");

                m_readCursor += sizeof(ClientPacketHeader);
                m_packetHeader = header;

                processBuffer();

                Serial.printf("Parser: processBuffer, header processing finished\r\n");

                break;
            }
#else
            char* data = m_buffer + m_readCursor;

            Serial.printf("Parser: processBuffer, looking at %p, readCursor=%d\r\n", data, m_readCursor);

            ClientPacketHeader header;
            memcpy(header.data, data, sizeof(ClientPacketHeader));

            Serial.printf("Parser: processBuffer, header magic=%x\r\n", header.magic);
            Serial.printf("Parser: processBuffer, header endMagic=%x\r\n", header.endMagic);

            if (header.magic == HEADER_MAGIC && header.endMagic == HEADER_END_MAGIC)
            {
                Serial.printf("Parser: processBuffer, header found at %p, readCursor=%d\r\n", data, m_readCursor);

#if 0
                // DEBUG receive payload length as a hexadecimal number
                data[4] = 0;
                uint16_t payloadLength = std::strtoul(data + 2, nullptr, 16);
                if (payloadLength <= sizeof(m_buffer) - m_bufferCursor)
                    header.payloadLength = payloadLength;
                // DEBUG
#endif

                Serial.printf("Parser: processBuffer, payloadLength=%d\r\n", header.payloadLength);

                if (header.payloadLength > sizeof(m_buffer) - m_bufferCursor)
                    break;

                Serial.printf("Parser: processBuffer, header is valid\r\n");

                m_readCursor += sizeof(ClientPacketHeader);
                m_packetHeader = header;
                m_headerOk = true;

                processBuffer();

                Serial.printf("Parser: processBuffer, header processing finished\r\n");

                break;
            }
#endif

            ++m_readCursor;
        }
    }
    else
    {
        if (m_bufferCursor - m_readCursor < 
#ifdef HEADER_DIRECT_ACCESS
                m_packetHeader->payloadLength)
#else
                m_packetHeader.payloadLength)
#endif
            return;

        Serial.printf("Parser: processBuffer, payload buffered\r\n");

        m_payload = m_buffer + m_readCursor;
#ifdef HEADER_DIRECT_ACCESS
        m_readCursor += m_packetHeader->payloadLength;
#else
        m_readCursor += m_packetHeader.payloadLength;
#endif
        m_buffer[m_readCursor] = 0;
        m_completed = true;

        Serial.printf("Parser: processBuffer, readCursor=%d\r\n", m_readCursor);
    }
}
