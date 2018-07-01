/*
 * Hub.h
 *
 *  Created on: 2018. jún. 27.
 *      Author: ToMikaa
 */

#ifndef HUB_HUB_H_
#define HUB_HUB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/netif.h"

void hub_init(struct netif* iface);
void hub_task();

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* HUB_HUB_H_ */
