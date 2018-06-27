/*
 * Hub.cpp
 *
 *  Created on: 2018. j�n. 27.
 *      Author: ToMikaa
 */

#include "Hub.h"

#include "TestSocket.h"

hub::TestSocket* gTestSocket = nullptr;

void hub_init(struct netif* iface)
{
    static hub::TestSocket sTestSocket{ iface };
    gTestSocket = &sTestSocket;
}
