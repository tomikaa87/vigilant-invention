/*
 * TestSocket.cpp
 *
 *  Created on: 2018. jún. 27.
 *      Author: ToMikaa
 */

#include "TestSocket.h"

#include "lwip/tcp.h"
#include "lwip/netif.h"

#include <stdio.h>

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

    return ERR_RST;
}

} /* namespace hub */

