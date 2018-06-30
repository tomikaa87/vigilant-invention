/*
 * TestSocket.cpp
 *
 *  Created on: 2018. jún. 27.
 *      Author: ToMikaa
 */

#include "TestSocket.h"

#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"

#include <stdio.h>
#include <string.h>

namespace hub
{

TestSocket::TestSocket(struct netif* iface)
    : mIface(iface)
{
    mPcb = tcp_new();

    tcp_arg(mPcb, this);

    if (tcp_bind(mPcb, IP_ADDR_ANY, 23) == ERR_OK)
    {
        mPcb = tcp_listen(mPcb);

        tcp_accept(mPcb, [](void* arg, struct tcp_pcb* newPcb, err_t err) {
            return reinterpret_cast<TestSocket*>(arg)->onAccept(newPcb, err);
        });
    }
    else
    {
        memp_free(MEMP_TCP_PCB, mPcb);
    }
}

TestSocket::~TestSocket()
{
    tcp_shutdown(mPcb, true, true);
}

int8_t TestSocket::onAccept(struct tcp_pcb* newPcb, int8_t err)
{
    printf("Incoming connection from %d.%d.%d.%d\r\n",
            ip4_addr1(&newPcb->remote_ip),
            ip4_addr2(&newPcb->remote_ip),
            ip4_addr3(&newPcb->remote_ip),
            ip4_addr4(&newPcb->remote_ip));

    tcp_err(newPcb, [](void* arg, err_t err) {
        reinterpret_cast<TestSocket*>(arg)->onError(err);
    });

    tcp_recv(newPcb, [](void* arg, struct tcp_pcb* pcb, struct pbuf* buf, err_t err) {
        return reinterpret_cast<TestSocket*>(arg)->onReceive(pcb, buf, err);
    });

    tcp_sent(newPcb, [](void* arg, struct tcp_pcb* pcb, uint16_t len) {
        return reinterpret_cast<TestSocket*>(arg)->onSent(pcb, len);
    });

    return ERR_OK;
}

int8_t TestSocket::onReceive(struct tcp_pcb* pcb, struct pbuf* buf, int8_t err)
{
    printf("onReceive, tot_len=%d\r\n", buf->tot_len);

    if (err != ERR_OK)
    {
        printf("Error: %d\r\n", err);
        return err;
    }

    if (!buf)
    {
        printf("Buffer is null\r\n");
        tcp_shutdown(pcb, true, true);
        return ERR_CLSD;
    }

    uint32_t readBytes = 0;
    pbuf* nextBuf = buf;

    while (readBytes < buf->tot_len)
    {
        printf("Reading chunk, len=%d\r\n", nextBuf->len);

        readBytes += buf->len;

        auto s = new char[nextBuf->len + 1];
        memset(s, 0, nextBuf->len + 1);
        memcpy(s, nextBuf->payload, nextBuf->len);

        printf("Payload: %s\r\n", s);

        delete(s);

        tcp_recved(pcb, nextBuf->len);

        nextBuf = nextBuf->next;
    }

    pbuf_free(buf);

    tcp_write(pcb, (const void*)"Hello\r\n", 8, 0);

    return ERR_OK;
}

int8_t TestSocket::onSent(struct tcp_pcb* pcb, uint16_t len)
{
    printf("Sent bytes: %d\r\n", len);

    return ERR_OK;
}

void TestSocket::onError(int8_t err)
{
    printf("Error: %d\r\n", err);
}

} /* namespace hub */

