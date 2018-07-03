/*
 * BlynkSocket.h
 *
 *  Created on: 2018. jun. 30.
 *      Author: ToMikaa
 */

#ifndef HUB_BLYNKSOCKET_H_
#define HUB_BLYNKSOCKET_H_

#define BLYNK_PRINT stderr

#include <Blynk/BlynkProtocol.h>

#include <QHostAddress>
#include <QTcpSocket>

// TODO: use SSL socket instead of raw TCP
#include <QSslSocket>

namespace hub
{

#include <cstdint>

class BlynkTransportSocket
{
public:
    BlynkTransportSocket()
    {
        BLYNK_LOG("Creating BlynkTransportSocket");
    }

    void begin(const char* h, uint16_t p)
    {
        mDomain = h;
        mPort = p;
    }

    bool connect()
    {
        if (mSocket.state() == QAbstractSocket::ConnectedState)
        {
            BLYNK_LOG("Socket already connected");
            return false;
        }

        mSocket.connectToHost(mDomain, mPort);

        if (!mSocket.waitForConnected())
        {
            BLYNK_LOG("Failed to connect to %s:%d", mDomain.toLocal8Bit().constData(), mPort);
            BLYNK_LOG("Error: %s", mSocket.errorString().toLocal8Bit().constData());
            return false;
        }

        return true;
    }

    void disconnect()
    {
        if (connected())
        {
            BLYNK_LOG("Disconnecting");

            mSocket.disconnectFromHost();
        }
    }

    size_t read(void* buf, size_t len)
    {
        //BLYNK_LOG("Reading %d bytes", len);

        return mSocket.read(reinterpret_cast<char*>(buf), len);
    }

    size_t write(const void* buf, size_t len)
    {
        //BLYNK_LOG("Writing %d bytes", len);

        return mSocket.write(reinterpret_cast<const char*>(buf), len);
    }

    bool connected()
    {
        return mSocket.state() == QAbstractSocket::ConnectedState;
    }

    int available()
    {
        char tmp[1024];
        return mSocket.peek(tmp, sizeof(tmp));
    }

protected:
    QString mDomain;
    uint16_t mPort = 0;
    QTcpSocket mSocket;
};

class BlynkSocket: public BlynkProtocol<BlynkTransportSocket>
{
    typedef BlynkProtocol<BlynkTransportSocket> Base;

public:
    BlynkSocket(BlynkTransportSocket& transport)
        : Base(transport)
    {}

    void begin(const char* auth,
               const char* domain = BLYNK_DEFAULT_DOMAIN,
               uint16_t    port   = BLYNK_DEFAULT_PORT)
    {
        Base::begin(auth);
        conn.begin(domain, port);
    }
};

}

#endif /* HUB_BLYNKSOCKET_H_ */
