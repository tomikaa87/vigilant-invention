#include "ClientPacketParser.h"

#include <algorithm>
#include <cstring>

static const uint16_t HeaderMagic = 'K' | 'T' << 8;
static const uint16_t HeaderEndMagic = 'T' | 'K' << 8;

ClientPacketParser::ClientPacketParser()
{
}


ClientPacketParser::~ClientPacketParser()
{
}

void ClientPacketParser::reset()
{
    memset(m_buffer, 0, sizeof(m_buffer));
    m_bufferCursor = 0;
    m_readCursor = 0;
    m_completed = false;
    m_packetHeader = nullptr;
    m_payload = nullptr;
}

void ClientPacketParser::input(const char* data, const uint16_t length)
{
    if (m_completed)
        return;

    uint16_t copyAmount = std::min(static_cast<uint16_t>(sizeof(m_buffer) - m_bufferCursor), length);

    if (copyAmount == 0)
        return;

    memcpy(m_buffer + m_bufferCursor, data, copyAmount);
    m_bufferCursor += copyAmount;

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

    if (!m_packetHeader)
    {
        while (m_bufferCursor - m_readCursor >= sizeof(ClientPacketHeader))
        {
            /*const */char* data = m_buffer + m_readCursor;

            auto header = reinterpret_cast</*const */ClientPacketHeader*>(data);

            if (header->magic == HeaderMagic && header->endMagic == HeaderEndMagic)
            {
                // DEBUG receive payload length as a hexadecimal number
                header->endMagic = 0;
                uint16_t payloadLength = std::strtoul(data + 2, nullptr, 16);
                if (payloadLength <= sizeof(m_buffer) - m_bufferCursor)
                    header->payloadLength = payloadLength;
                // DEBUG

                if (header->payloadLength > sizeof(m_buffer) - m_bufferCursor)
                    break;

                m_readCursor += sizeof(ClientPacketHeader);
                m_packetHeader = header;

                processBuffer();

                break;
            }

            ++m_readCursor;
        }
    }
    else
    {
        if (m_bufferCursor - m_readCursor < m_packetHeader->payloadLength)
            return;

        m_payload = m_buffer + m_readCursor;
        m_readCursor += m_packetHeader->payloadLength;
        m_buffer[m_readCursor] = 0;
        m_completed = true;
    }
}
