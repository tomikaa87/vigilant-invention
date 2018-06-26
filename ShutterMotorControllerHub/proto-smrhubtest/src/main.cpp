
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "hub/Hub.h"
#include "hub/DiagTerminal.h"
#include "main.h"

#include <stdio.h>
#include <stdint.h>

static void setupPeripherals();

volatile uint32_t s_timingDelay = 0;

int main(void)
{
    setupPeripherals();

    Hub hub;

    while (1)
    {
        auto action = DiagTerminal::instance()->task();
        hub.task();

        switch (action)
        {
            case DiagTerminal::Action::ReadStatus:
                hub.readStatus();
                break;

            case DiagTerminal::Action::Shutter1Up:
                hub.shutter1Up();
                break;

            case DiagTerminal::Action::Shutter1Down:
                hub.shutter1Down();
                break;

            case DiagTerminal::Action::Shutter2Up:
                hub.shutter2Up();
                break;

            case DiagTerminal::Action::Shutter2Down:
                hub.shutter2Down();
                break;

            case DiagTerminal::Action::ScanUnits:
                hub.scanUnits();
                break;

            case DiagTerminal::Action::SelectDevice:
                hub.selectDevice(DiagTerminal::instance()->selectedDeviceIndex());
                break;

            default:
                break;
        }

        if (s_timingDelay == 0)
        {
            s_timingDelay = 25;
        }
    }
}

static void setupPeripherals()
{
    /* Initialize LEDs */
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);

    /* Turn on LEDs */
    STM_EVAL_LEDOn(LED3);
    STM_EVAL_LEDOn(LED4);
    STM_EVAL_LEDOn(LED5);
    STM_EVAL_LEDOn(LED6);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 | RCC_APB1Periph_SPI3 | RCC_APB1Periph_USART2 | RCC_APB1Periph_UART4, ENABLE);

    NVIC_InitTypeDef usart_irq;
    usart_irq.NVIC_IRQChannel = UART4_IRQn;
    usart_irq.NVIC_IRQChannelCmd = ENABLE;
    usart_irq.NVIC_IRQChannelPreemptionPriority = 0;
    usart_irq.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&usart_irq);

    USART_InitTypeDef usart2;
    USART_StructInit(&usart2);
    usart2.USART_BaudRate = 115200;
    usart2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart2.USART_Parity = USART_Parity_No;
    usart2.USART_StopBits = USART_StopBits_1;
    usart2.USART_WordLength = USART_WordLength_8b;

    USART_ClockInitTypeDef usart2_clock;
    USART_ClockStructInit(&usart2_clock);
    usart2_clock.USART_Clock = USART_Clock_Enable;
    USART_ClockInit(USART2, &usart2_clock);

    USART_Init(USART2, &usart2);
    USART_Init(UART4, &usart2);
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    USART_ITConfig(UART4, USART_IT_TXE, DISABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);
//    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

    GPIO_InitTypeDef usart_pins;
    GPIO_StructInit(&usart_pins);
    usart_pins.GPIO_Mode = GPIO_Mode_AF;
    usart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_1;
    usart_pins.GPIO_Speed = GPIO_Speed_50MHz;
//    usart_pins.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOA, &usart_pins);
//    usart_pins.GPIO_Pin = GPIO_Pin_3;
//    GPIO_Init(GPIOA, &usart_pins);

    USART_Cmd(USART2, ENABLE);
    USART_Cmd(UART4, ENABLE);

    printf("\r\n\r\n*** SMR HUB TEST ***\r\n\r\n");

    SPI_InitTypeDef spi;
    SPI_StructInit(&spi);
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    spi.SPI_Mode = SPI_Mode_Master;
    spi.SPI_NSS = SPI_NSS_Soft;

    SPI_Init(SPI2, &spi);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

    GPIO_InitTypeDef irq_pin;
    GPIO_StructInit(&irq_pin);
    irq_pin.GPIO_Pin = GPIO_Pin_1;
    irq_pin.GPIO_Mode = GPIO_Mode_IN;
    irq_pin.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &irq_pin);

    GPIO_InitTypeDef spi_pins;
    GPIO_StructInit(&spi_pins);
    spi_pins.GPIO_Mode = GPIO_Mode_AF;
    spi_pins.GPIO_OType = GPIO_OType_PP;
    spi_pins.GPIO_Speed = GPIO_Speed_50MHz;
    spi_pins.GPIO_PuPd = GPIO_PuPd_NOPULL;
    spi_pins.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &spi_pins);

    GPIO_InitTypeDef out_pins;
    GPIO_StructInit(&out_pins);
    out_pins.GPIO_Mode = GPIO_Mode_OUT;
    out_pins.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_7 | GPIO_Pin_8;
    out_pins.GPIO_Speed = GPIO_Speed_50MHz;
    out_pins.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOB, &out_pins);

    SPI_Cmd(SPI2, ENABLE);

    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);

    // 10 ms SysTick
    SysTick_Config(rcc_clocks.HCLK_Frequency / 100);
}

extern "C" void TimingDelay_Decrement()
{
    if (s_timingDelay > 0)
        --s_timingDelay;
}


/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 */
extern "C" void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
  /* TODO, implement your code here */
  return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
extern "C" uint16_t EVAL_AUDIO_GetSampleCallBack(void){
  /* TODO, implement your code here */
  return -1;
}
