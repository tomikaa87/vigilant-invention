/**
 ******************************************************************************
 * File Name          : ethernetif.c
 * Description        : This file provides code for the configuration
 *                      of the ethernetif.c MiddleWare.
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * Copyright (c) 2018 STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "ethernetif.h"
#include "enc28j60.h"
#include <string.h>

/* Network interface name */
#define IFNAME0 's'
#define IFNAME1 't'

/* Global Ethernet handle */
ENC_HandleTypeDef EncHandle __attribute__((section(".ccmram")));

extern SPI_HandleTypeDef hspi2;

/*******************************************************************************
 LL Driver Interface ( LwIP stack --> ETH)
 *******************************************************************************/

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif) {
	/* set MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;

	/* set MAC hardware address */
	netif->hwaddr[0] = 0x54;
	netif->hwaddr[1] = 0x10;
	netif->hwaddr[2] = 0xEC;
	netif->hwaddr[3] = 0x87;
	netif->hwaddr[4] = 0x04;
	netif->hwaddr[5] = 0x23;

	EncHandle.Init.MACAddr = netif->hwaddr;
	EncHandle.Init.DuplexMode = ETH_MODE_HALFDUPLEX;
	EncHandle.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
	EncHandle.Init.InterruptEnableBits = EIE_LINKIE;

	/* configure ethernet peripheral (GPIOs, clocks, MAC, DMA) */
//	ENC_MSPInit(&EncHandle);
	/* Set netif link flag */
	//  netif->flags |= NETIF_FLAG_LINK_UP;
	/* maximum transfer unit */
	netif->mtu = 1500;

	/* device capabilities */
	/* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
	netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

	/* Start the EN28J60 module */
	if (ENC_Start(&EncHandle)) {
		/* Set the MAC address */
		ENC_SetMacAddr(&EncHandle);

		/* Set netif link flag */
		netif->flags |= NETIF_FLAG_LINK_UP;
	}

}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p) {
	/* TODO use netif to check if we are the right ethernet interface */
	err_t errval;
	struct pbuf *q;
	uint32_t framelength = 0;

	/* Prepare ENC28J60 Tx buffer */
	errval = ENC_RestoreTXBuffer(&EncHandle, p->tot_len);
	if (errval != ERR_OK) {
		return errval;
	}

	/* copy frame from pbufs to driver buffers and send packet */
	for (q = p; q != NULL; q = q->next) {
		ENC_WriteBuffer(q->payload, q->len);
		framelength += q->len;
	}

	if (framelength != p->tot_len) {
		return ERR_BUF;
	}

	EncHandle.transmitLength = p->tot_len;
	/* Actual transmission is triggered in main loop */

	return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf * low_level_input(struct netif *netif) {
	struct pbuf *p = NULL;
	struct pbuf *q;
	uint16_t len;
	uint8_t *buffer;
	uint32_t bufferoffset = 0;

	if (!ENC_GetReceivedFrame(&EncHandle)) {
		return NULL;
	}

	/* Obtain the size of the packet and put it into the "len" variable. */
	len = EncHandle.RxFrameInfos.length;
	buffer = (uint8_t *) EncHandle.RxFrameInfos.buffer;

	if (len > 0) {
		/* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
		p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
	}

	if (p != NULL) {
		bufferoffset = 0;

		for (q = p; q != NULL; q = q->next) {
			/* Copy data in pbuf */
			memcpy((uint8_t*) ((uint8_t*) q->payload),
					(uint8_t*) ((uint8_t*) buffer + bufferoffset), q->len);
			bufferoffset = bufferoffset + q->len;
		}
	}

 	return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input(struct netif *netif) {
	err_t err;
	struct pbuf *p;

	/* move received packet into a new pbuf */
	p = low_level_input(netif);

	/* no packet could be read, silently ignore this */
	if (p == NULL)
		return;

	/* entry point to the LwIP stack */
	err = netif->input(p, netif);

	if (err != ERR_OK) {
		LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
		pbuf_free(p);
		p = NULL;
	}
}

#if !LWIP_ARP
/**
 * This function has to be completed by user in case of ARP OFF.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if ...
 */
static err_t low_level_output_arp_off(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
	err_t errval;
	errval = ERR_OK;

	/* USER CODE BEGIN 5 */

	/* USER CODE END 5 */

	return errval;

}
#endif /* LWIP_ARP */ 

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif) {
	LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;
	/* We directly use etharp_output() here to save a function call.
	 * You can instead declare your own function an call etharp_output()
	 * from it if you have to do some checks before sending (e.g. if link
	 * is available...) */

#if LWIP_IPV4
#if LWIP_ARP || LWIP_ETHERNET
#if LWIP_ARP
	netif->output = etharp_output;
#else
	/* The user should write ist own code in low_level_output_arp_off function */
	netif->output = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
	netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */

	netif->linkoutput = low_level_output;

	/* initialize the hardware */
	low_level_init(netif);

	return ERR_OK;
}

/**
 * @brief  Returns the current time in milliseconds
 *         when LWIP_TIMERS == 1 and NO_SYS == 1
 * @param  None
 * @retval Time
 */
u32_t sys_jiffies(void) {
	return HAL_GetTick();
}

/**
 * @brief  Returns the current time in milliseconds
 *         when LWIP_TIMERS == 1 and NO_SYS == 1
 * @param  None
 * @retval Time
 */
u32_t sys_now(void) {
	return HAL_GetTick();
}

/**
  * @brief  This function sets the netif link status.
  * @param  netif: the network interface
  * @retval None
  */
void ethernetif_set_link(struct netif *netif)
{
    /* Handle ENC28J60 interrupt */
    ENC_IRQHandler(&EncHandle);

    /* Check whether the link is up or down*/
    if(((EncHandle.LinkStatus) & PHSTAT2_LSTAT)!= 0) {
      netif_set_link_up(netif);
    } else {
      netif_set_link_down(netif);
    }

    /* Reenable interrupts */
    ENC_EnableInterrupts(EIE_INTIE);
}

/**
  * @brief  Link callback function, this function is called on change of link status
  *         to update low level driver configuration.
* @param  netif: The network interface
  * @retval None
  */
void ethernetif_update_config(struct netif *netif)
{
  if(netif_is_link_up(netif)) {
      /* Restart the EN28J60 module */
      low_level_init(netif);
  }

  ethernetif_notify_conn_changed(netif);
}

/**
  * @brief  This function notify user about link status changement.
  * @param  netif: the network interface
  * @retval None
  */
void ethernetif_notify_conn_changed(struct netif *netif)
{
  /* NOTE : This is function could be implemented in user file
            when the callback is needed,
  */

	  if(netif_is_link_up(netif))
	  {
	//    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
	//    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
	//    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	//
	//    netif_set_addr(netif, &ipaddr , &netmask, &gw);
	//
	//    /* When the netif is fully configured this function must be called.*/
	    netif_set_up(netif);
	//
	//    BSP_LED_Off(LED4);
	//    BSP_LED_On(LED3);
	  }
	  else
	  {
	    /*  When the netif link is down this function must be called.*/
	    netif_set_down(netif);

	//    BSP_LED_Off(LED3);
	//    BSP_LED_On(LED4);
	  }
}

/**
 * Implement actual transmission triggering
 */
void ethernet_transmit(void) {
    ENC_Transmit(&EncHandle);
}

/**
 * Implement SPI single byte send and receive.
 * The ENC28J60 slave SPI must already be selected and wont be deselected after transmission
 * Must be provided by user code
 * param  command: command or data to be sent to ENC28J60
 * retval answer from ENC28J60
 */

uint8_t ENC_SPI_SendWithoutSelection(uint8_t command) {
	HAL_SPI_TransmitReceive(&hspi2, &command, &command, 1, 1000);
	return command;
}

/**
 * Implement SPI single byte send and receive. Must be provided by user code
 * param  command: command or data to be sent to ENC28J60
 * retval answer from ENC28J60
 */

uint8_t ENC_SPI_Send(uint8_t command) {
	/* Select ENC28J60 module */
	HAL_NVIC_DisableIRQ(ENC_nIRQ_EXTI_IRQn);
	HAL_GPIO_WritePin(ENC_nCS_GPIO_Port, ENC_nCS_Pin, GPIO_PIN_RESET);
	up_udelay(1);

	HAL_SPI_TransmitReceive(&hspi2, &command, &command, 1, 1000);

	/* De-select ENC28J60 module */
	HAL_GPIO_WritePin(ENC_nCS_GPIO_Port, ENC_nCS_Pin, GPIO_PIN_SET);
	up_udelay(1);

	HAL_NVIC_EnableIRQ(ENC_nIRQ_EXTI_IRQn);
	return command;
}

/**
 * Implement SPI buffer send and receive. Must be provided by user code
 * param  master2slave: data to be sent from host to ENC28J60, can be NULL if we only want to receive data from slave
 * param  slave2master: answer from ENC28J60 to host, can be NULL if we only want to send data to slave
 * retval none
 */

void ENC_SPI_SendBuf(uint8_t *master2slave, uint8_t *slave2master,
		uint16_t bufferSize) {
	/* Select ENC28J60 module */
	HAL_NVIC_DisableIRQ(ENC_nIRQ_EXTI_IRQn);
	HAL_GPIO_WritePin(ENC_nCS_GPIO_Port, ENC_nCS_Pin, GPIO_PIN_RESET);
	up_udelay(1);

	/* Transmit or receuve data */
	if (slave2master == NULL) {
		if (master2slave != NULL) {
			HAL_SPI_Transmit(&hspi2, master2slave, bufferSize, 1000);
		}
	} else if (master2slave == NULL) {
		HAL_SPI_Receive(&hspi2, slave2master, bufferSize, 1000);
	} else {
		HAL_SPI_TransmitReceive(&hspi2, master2slave, slave2master, bufferSize,
				1000);
	}

	/* De-select ENC28J60 module */
	HAL_GPIO_WritePin(ENC_nCS_GPIO_Port, ENC_nCS_Pin, GPIO_PIN_SET);
	up_udelay(1);
	HAL_NVIC_EnableIRQ(ENC_nIRQ_EXTI_IRQn);
}

/**
 * Implement SPI Slave selection and deselection. Must be provided by user code
 * param  select: true if the ENC28J60 slave SPI if selected, false otherwise
 * retval none
 */

void ENC_SPI_Select(bool select) {
	/* Select or de-select ENC28J60 module */
	if (select) {
		HAL_NVIC_DisableIRQ(ENC_nIRQ_EXTI_IRQn);
		HAL_GPIO_WritePin(ENC_nCS_GPIO_Port, ENC_nCS_Pin, GPIO_PIN_RESET);
		up_udelay(1);
	} else {
		HAL_GPIO_WritePin(ENC_nCS_GPIO_Port, ENC_nCS_Pin, GPIO_PIN_SET);
		up_udelay(1);
		HAL_NVIC_EnableIRQ(ENC_nIRQ_EXTI_IRQn);
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

