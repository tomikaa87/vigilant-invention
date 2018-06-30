/*
 * TestSocket.h
 *
 *  Created on: 2018. jún. 27.
 *      Author: ToMikaa
 */

#ifndef HUB_TESTSOCKET_H_
#define HUB_TESTSOCKET_H_

#include <cstdint>

struct netif;
struct pbuf;
struct tcp_pcb;

namespace hub
{

class TestSocket
{
public:
    TestSocket(struct netif* iface);
    virtual ~TestSocket();

    int8_t onAccept(struct tcp_pcb* newPcb, int8_t err);
    int8_t onReceive(struct tcp_pcb* pcb, struct pbuf* buf, int8_t err);
    int8_t onSent(struct tcp_pcb* pcb, uint16_t len);
    void onError(int8_t err);

private:
    struct netif* mIface = nullptr;
    struct tcp_pcb* mPcb = nullptr;
};

} /* namespace hub */

#endif /* HUB_TESTSOCKET_H_ */
