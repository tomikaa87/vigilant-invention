/*
 * Hub.cpp
 *
 *  Created on: 2018. jún. 27.
 *      Author: ToMikaa
 */

#include "Hub.h"

#include "TestSocket.h"

#define BLYNK_LOG(msg, ...) { printf("[%ld] " msg "\r\n", BlynkMillis(), ##__VA_ARGS__); }
//#define BLYNK_LOG1(p1)                  { printf("[%ld] " p1 "\n", BlynkMillis()); }
//#define BLYNK_LOG2(p1,p2)               { BLYNK_LOG(p1,p2);}
//#define BLYNK_LOG3(p1,p2,p3)            { BLYNK_LOG(p1,p2,p3);}
//#define BLYNK_LOG4(p1,p2,p3,p4)         { BLYNK_LOG(p1,p2,p3,p4);}
//#define BLYNK_LOG6(p1,p2,p3,p4,p5,p6)   { BLYNK_LOG(p1,p2,p3,p4,p5,p6); }

#define BLYNK_LOG1(p1)                  { BLYNK_LOG("%s", p1) }
#define BLYNK_LOG2(p1,p2)
#define BLYNK_LOG3(p1,p2,p3)
#define BLYNK_LOG4(p1,p2,p3,p4)
#define BLYNK_LOG6(p1,p2,p3,p4,p5,p6)

#define BLYNK_LOG_TIME() { printf("[%ld]", BlynkMillis()); }

#define BLYNK_LOG_IP(msg, ip)
#define BLYNK_LOG_IP_REV(msg, ip)


#include "BlynkSocket.h"

#include "stm32f4xx_hal.h"

#include <cstdio>

hub::TestSocket* gTestSocket = nullptr;
hub::BlynkSocket* gBlynk = nullptr;

void hub_init(struct netif* iface)
{
    static hub::TestSocket sTestSocket{ iface };
    gTestSocket = &sTestSocket;

    static hub::BlynkTransportSocket sBlynkTransportSocket{ iface };
    static hub::BlynkSocket sBlynk{ sBlynkTransportSocket };
    sBlynk.begin("7482036d35bf48c4946d876513663628", "blynk-cloud.com", 80);
    gBlynk = &sBlynk;
}

void hub_task()
{
    gBlynk->run(false);
}

millis_time_t BlynkMillis()
{
    return HAL_GetTick();
}

void BlynkDelay(uint32_t ms)
{
    HAL_Delay(ms);
}

void BlynkOnDisconnected()
{
}

void BlynkOnConnected()
{
}
