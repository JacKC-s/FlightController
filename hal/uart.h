#ifndef UART_H
#define UART_H

#include "stm32f446xx.h"
#include "gpio.h"
#include <stdint.h>
#include <sys/types.h>


typedef enum {
    /*USART1*/
    USART1_Conf0, // TX: PA9 RX: PA10 AF7
    USART1_Conf1, // TX: PB6 RX: PB7 AF7
    /*USART2*/
    USART2_Conf0, // TX: PA2  RX: PA3  AF7
    USART2_Conf1, // TX: PD5  RX: PD6  AF7
    /*USART3*/
    USART3_Conf0, // TX: PB10 RX: PB11 AF7
    USART3_Conf1, // TX: PC10 RX: PC11 AF7
    USART3_Conf2, // TX: PD8  RX: PD9  AF7
    /*USART4*/
    UART4_Conf0,  // TX: PA0  RX: PA1  AF8
    UART4_Conf1,  // TX: PC10 RX: PC11 AF8
    /*USART5*/
    UART5_Conf0,  // TX: PC12 RX: PD2  AF8
    /*USART6*/
    USART6_Conf0, // TX: PA11 RX: PA12 AF8
    USART6_Conf1  // TX: PC6  RX: PC7  AF8
} USART_Pin_t;


typedef struct {
    USART_Pin_t Pin;
    uint32_t baud;
    uint32_t word_length; // 8 or 9, chosen by USARTx_CR1_M
    uint32_t StopBits;
    uint32_t Parity;
} UART_Config_t;


/*Everything under here is non functional yet. Need to learn more about UART.*/

// Initialization functions
void UART_Init(UART_Config_t *Config);
void UART_DeInit(UART_Config_t *Config);
//Low level init function?

//TX functions
void Transmit_Poll(UART_Config_t *Config, uint8_t data);
void Transmit_Interrupt(UART_Config_t *Config, uint8_t data);
void Transmit_DMA(UART_Config_t *Config, uint8_t data);

//RX functions
uint8_t Recieve_Poll(UART_Config_t *Config);
uint8_t Recieve_Interrupt(UART_Config_t *Config);
uint8_t Recieve_DMA(UART_Config_t *Config);

#endif