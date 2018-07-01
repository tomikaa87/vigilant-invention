/*
 * BlynkSocket.h
 *
 *  Created on: 2018. jún. 30.
 *      Author: ToMikaa
 */

#ifndef HUB_BLYNKSOCKET_H_
#define HUB_BLYNKSOCKET_H_

#include <Blynk/BlynkProtocol.h>

namespace hub
{

#include <cstdint>

#include "lwip.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/ip4_addr.h"
#include "ethernetif.h"

class BlynkTransportSocket
{
public:
    BlynkTransportSocket(struct netif* iface)
        : mIface(iface)
    {
        BLYNK_LOG("Creating BlynkTransportSocket");

        IP4_ADDR(&mServerAddr, 139, 59, 206, 133);
    }

    void begin(const char* h, uint16_t p)
    {
        mDomain = h;
        mPort = p;
    }

    bool connect()
    {
        if (mPcb)
        {
            BLYNK_LOG("Socket already created. Disconnect first");
            return false;
        }

        if (!netif_is_link_up(mIface))
        {
            BLYNK_LOG("Inteface is down");
            return false;
        }

        mPcb = tcp_new();
        mBuf = nullptr;

        ip_set_option(mPcb, SOF_REUSEADDR);

        BLYNK_LOG("TCP socket created: %p", mPcb);

        BLYNK_LOG("Connecting to %s:%d", mDomain, mPort);

        struct ConnectResult
        {
            bool connected = false;
            bool finished = false;
        } result;

        tcp_arg(mPcb, &result);

        tcp_err(mPcb, [](void* arg, err_t err) {
            BLYNK_LOG("TCP error: %d", err);
            auto result = reinterpret_cast<ConnectResult*>(arg);
            result->finished = true;
        });

        auto err = tcp_connect(mPcb, &mServerAddr, mPort, [](void* arg, struct tcp_pcb*, err_t err) {
            BLYNK_LOG("Connect result: %d", err);
            auto result = reinterpret_cast<ConnectResult*>(arg);
            result->finished = true;
            result->connected = err == ERR_OK;
            return err;
        });

        if (err != ERR_OK)
        {
            if (err == ERR_RTE)
            {
                BLYNK_LOG("No route to host");
            }

            tcp_close(mPcb);
            tcp_shutdown(mPcb, true, true);
//            tcp_abort(mPcb);
            mPcb = nullptr;

            return false;
        }

        while (!result.finished)
        {
            // TODO: these should not be here
            ethernetif_input(mIface);
            ethernet_transmit();
            sys_check_timeouts();
        }

        tcp_err(mPcb, nullptr);

        mConnected = result.connected;

        BLYNK_LOG("Connected: %s", mConnected ? "yes" : "no");

        if (!mConnected)
        {
            tcp_close(mPcb);
            tcp_shutdown(mPcb, true, true);
//            tcp_abort(mPcb);
            mPcb = nullptr;

            return false;
        }

        tcp_arg(mPcb, this);

        tcp_recv(mPcb, [](void* arg, struct tcp_pcb*, struct pbuf* buf, err_t err) {
            return reinterpret_cast<BlynkTransportSocket*>(arg)->onReceive(buf, err);
        });

        return true;
    }

    void disconnect()
    {
        if (connected())
        {
            BLYNK_LOG("Disconnecting");

            tcp_close(mPcb);
            tcp_shutdown(mPcb, true, true);
//            tcp_abort(mPcb);
            mPcb = nullptr;
            mConnected = false;
        }
    }

    size_t read(void* buf, size_t len)
    {
        BLYNK_LOG("Reading max %d bytes", len);

        if (!mBuf)
        {
            BLYNK_LOG("Cannot read from null buffer");
            return -1;
        }

        if (mBuf->tot_len == len)
        {
            int readBytes = 0;
            auto nextBuf = mBuf;

            while (readBytes < mBuf->tot_len)
            {
                memcpy(buf + readBytes, mBuf->payload, mBuf->len);
                readBytes += mBuf->len;
                nextBuf = nextBuf->next;
            }

            tcp_recved(mPcb, readBytes);

            pbuf_free(mBuf);
            mBuf = nullptr;

            return readBytes;
        }
        else
        {
            BLYNK_LOG("Partial read is not implemented");
            return 0;
        }

//        ssize_t rlen = ::read(sockfd, buf, len);
//        if (rlen == -1) {
//            //BLYNK_LOG4("Read error ", errno, ": ", strerror(errno));
//            if (errno == ETIMEDOUT || errno == EWOULDBLOCK || errno == EAGAIN) {
//                return 0;
//            }
//            disconnect();
//            return -1;
//        }
//        return rlen;
    }

    size_t write(const void* buf, size_t len)
    {
        BLYNK_LOG("Writing %d bytes", len);

        if (tcp_write(mPcb, buf, len, 0) == ERR_OK)
        {
            tcp_output(mPcb);
            return len;
        }

//        return ::write(sockfd, buf, len);
        return 0;
    }

    bool connected()
    {
//      return sockfd >= 0;
        return mConnected;
    }

    int available()
    {
        int avail = mBuf ? mBuf->tot_len : 0;
//        BLYNK_LOG("available(): %d", avail);

//        if (avail == 0)
//        {
//            HAL_Delay(100);
//        }
//        if (!connected()) {
//            return 0;
//        }
//
//        int count = 0;
//        if (0 == ioctl(sockfd, FIONREAD, &count)) {
//            if (!count) {
//                usleep(10000); // not to stall CPU with 100% load
//            }
//            return count;
//        }
        return avail;
    }

    err_t onReceive(struct pbuf* buf, err_t err)
    {
        printf("onReceive, tot_len=%d, buf=%p\r\n", buf->tot_len, buf);

        if (err != ERR_OK)
        {
            printf("Error: %d\r\n", err);

            if (mBuf)
            {
                pbuf_free(mBuf);
                mBuf = nullptr;
            }

            return err;
        }

        if (!buf)
        {
            printf("Socket disconnected\r\n");
            disconnect();
            return ERR_CLSD;
        }

        if (mBuf)
        {
            pbuf_chain(mBuf, buf);
        }
        else
        {
            mBuf = buf;
        }

//        uint32_t readBytes = 0;
//        pbuf* nextBuf = buf;
//
//        while (readBytes < buf->tot_len)
//        {
//            printf("Reading chunk, len=%d\r\n", nextBuf->len);
//
//            readBytes += buf->len;
//
//            auto s = new char[nextBuf->len + 1];
//            memset(s, 0, nextBuf->len + 1);
//            memcpy(s, nextBuf->payload, nextBuf->len);
//
//            printf("Payload: %s\r\n", s);
//
//            delete(s);
//
//            tcp_recved(mPcb, nextBuf->len);
//
//            nextBuf = nextBuf->next;
//        }
//
//        pbuf_free(buf);

        return ERR_OK;
    }

protected:
    const char* mDomain = nullptr;
    uint16_t mPort = 0;

    struct netif* mIface = nullptr;
    struct tcp_pcb* mPcb = nullptr;
    struct pbuf* mBuf = nullptr;

    ip4_addr_t mServerAddr;

    bool mConnected = false;
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
