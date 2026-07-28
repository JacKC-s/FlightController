#include "uart.h"
#include "FreeRTOS.h"
#include "gpio.h"
#include "stm32f446xx.h"
#include "task.h"
#include <stdio.h>

#define APB1_CLK 45000000U // APB1 clock speed
#define APB2_CLK 90000000U // APB2 clock speed


// Initialization function uses gpio driver

void UART_Init(UART_HandleTypeDef *huart) {
  // Setting up pins as null originally, then having a switch case tree decide
  // what pins to use
  GPIO_PinConfig_t txConfig = {0};
  GPIO_PinConfig_t rxConfig = {0};
  GPIO_TypeDef *txPort = NULL;
  GPIO_TypeDef *rxPort = NULL;

  uint32_t pclk = 0; // Peripheral clock speed, used to calculate the baud rate
  // Normal configuration for TX and RX
  txConfig.Mode = GPIO_MODE_AF;
  txConfig.OType = GPIO_OTYPE_PP;
  txConfig.Speed = GPIO_SPEED_FAST;
  txConfig.Pull = GPIO_PUPD_PU;

  rxConfig.Mode = GPIO_MODE_AF;
  rxConfig.OType = GPIO_OTYPE_PP;
  rxConfig.Speed = GPIO_SPEED_FAST;
  rxConfig.Pull = GPIO_PUPD_PU;

  switch (huart->Init.Pin) {
  /* USART1 */
  case USART1_Conf0: // TX: PA9 RX: PA10 AF7
    RCC->APB2ENR |= RCC_APB2ENR_GPIOAEN; // Enables the USART1 clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Enables the USART1 clock
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_9;
    txConfig.AF_Select = 7;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_10;
    rxConfig.AF_Select = 7;
    pclk = APB2_CLK; // Set peripheral clock speed for USART1
    break;
  case USART1_Conf1: // TX: PB6 RX: PB7 AF7
    RCC->APB2ENR |= RCC_APB2ENR_GPIOBEN; // Enables the USART1 clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Enables the USART1 clock
    txPort = GPIOB;
    txConfig.Pin = GPIO_PIN_6;
    txConfig.AF_Select = 7;
    rxPort = GPIOB;
    rxConfig.Pin = GPIO_PIN_7;
    rxConfig.AF_Select = 7;
    pclk = APB2_CLK; // Set peripheral clock speed for USART1
    break;
  /* USART2 */
  case USART2_Conf0: // TX: PA2  RX: PA3  AF7
    RCC->APB1ENR |= RCC_APB1ENR_GPIOAEN; // Enables the USART2 clock
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Enables the USART2 clock
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_2;
    txConfig.AF_Select = 7;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_3;
    rxConfig.AF_Select = 7;
    pclk = APB1_CLK; // Set peripheral clock speed for USART2
    break;
  case USART2_Conf1: // TX: PD5  RX: PD6  AF7
    RCC->APB1ENR |= RCC_APB1ENR_GPIODEN; // Enables the USART2 clock
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Enables the USART2 clock
    txPort = GPIOD;
    txConfig.Pin = GPIO_PIN_5;
    txConfig.AF_Select = 7;
    rxPort = GPIOD;
    rxConfig.Pin = GPIO_PIN_6;
    rxConfig.AF_Select = 7;
    pclk = APB1_CLK; // Set peripheral clock speed for USART2
    break;
  /* USART3 */
  case USART3_Conf0: // TX: PB10 RX: PB11 AF7
    RCC->APB1ENR |= RCC_APB1ENR_GPIOBEN; // Enables the USART3 clock
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // Enables the USART3 clock
    txPort = GPIOB;
    txConfig.Pin = GPIO_PIN_10;
    txConfig.AF_Select = 7;
    rxPort = GPIOB;
    rxConfig.Pin = GPIO_PIN_11;
    rxConfig.AF_Select = 7;
    pclk = APB1_CLK; // Set peripheral clock speed for USART3
    break;
  case USART3_Conf1: // TX: PC10 RX: PC11 AF7
    RCC->APB1ENR |= RCC_APB1ENR_GPIOCEN; // Enables the USART3 clock
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // Enables the USART3 clock
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_10;
    txConfig.AF_Select = 7;
    rxPort = GPIOC;
    rxConfig.Pin = GPIO_PIN_11;
    rxConfig.AF_Select = 7;
    pclk = APB1_CLK; // Set peripheral clock speed for USART3
    break;
  case USART3_Conf2: // TX: PD8  RX: PD9  AF7
    RCC->APB1ENR |= RCC_APB1ENR_GPIODEN; // Enables the USART3 clock
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // Enables the USART3 clock
    txPort = GPIOD;
    txConfig.Pin = GPIO_PIN_8;
    txConfig.AF_Select = 7;
    rxPort = GPIOD;
    rxConfig.Pin = GPIO_PIN_9;
    rxConfig.AF_Select = 7;
    pclk = APB1_CLK; // Set peripheral clock speed for USART3
    break;
  /* UART4 */
  case UART4_Conf0: // TX: PA0  RX: PA1  AF8
    RCC->APB1ENR |= RCC_APB1ENR_GPIOAEN; // Enables the UART4 clock
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN; // Enables the UART4 clock
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_0;
    txConfig.AF_Select = 8;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_1;
    rxConfig.AF_Select = 8;
    pclk = APB1_CLK; // Set peripheral clock speed for UART4
    break;
  case UART4_Conf1: // TX: PC10 RX: PC11 AF8
    RCC->APB1ENR |= RCC_APB1ENR_GPIOCEN; // Enables the UART4 clock
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN; // Enables the UART4 clock
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_10;
    txConfig.AF_Select = 8;
    rxPort = GPIOC;
    rxConfig.Pin = GPIO_PIN_11;
    rxConfig.AF_Select = 8;
    pclk = APB1_CLK; // Set peripheral clock speed for UART4
    break;
  /* UART5 */
  case UART5_Conf0: // TX: PC12 RX: PD2  AF8
    RCC->APB1ENR |= RCC_APB1ENR_GPIOCEN; // Enables the UART5 clock
    RCC->APB1ENR |= RCC_APB1ENR_GPIODEN; // Enables the UART5 clock
    RCC->APB1ENR |= RCC_APB1ENR_UART5EN; // Enables the UART5 clock
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_12;
    txConfig.AF_Select = 8;
    rxPort = GPIOD;
    rxConfig.Pin = GPIO_PIN_2;
    rxConfig.AF_Select = 8;
    pclk = APB1_CLK; // Set peripheral clock speed for UART5
    break;
  /* USART6 */
  case USART6_Conf0: // TX: PA11 RX: PA12 AF8
    RCC->APB2ENR |= RCC_APB2ENR_GPIOAEN; // Enables the USART6 clock
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN; // Enables the USART6 clock
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_11;
    txConfig.AF_Select = 8;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_12;
    rxConfig.AF_Select = 8;
    pclk = APB2_CLK; // Set peripheral clock speed for USART6
    break;
  case USART6_Conf1: // TX: PC6  RX: PC7  AF8
    RCC->APB2ENR |= RCC_APB2ENR_GPIOCEN; // Enables the USART6 clock
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN; // Enables the USART6 clock  
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_6;
    txConfig.AF_Select = 8;
    rxPort = GPIOC;
    rxConfig.Pin = GPIO_PIN_7;
    rxConfig.AF_Select = 8;
    pclk = APB2_CLK; // Set peripheral clock speed for USART6
    break;
  default:
    return; // Invalid configuration
  }
  // If both pins configured, then enables the gpio pins using the GPIO_Init Function!
  if (txPort && rxPort) {
    GPIO_Init(txPort, &txConfig);
    GPIO_Init(rxPort, &rxConfig);
  }
  // Changed to integer math to avoid floating point operations
  if (pclk != 0) {
    if (oversampling == 0) { // Oversampling by 16
      huart->Instance->BRR = (pclk + (huart->Init.baud / 2U)) / huart->Init.baud; // Round to nearest integer
    } else { // Oversampling by 8
    uint32_t div = (2 * (pclk + (huart->Init.baud / 2U))) / huart->Init.baud; // Round to nearest integer
    huart->Instance->BRR = ((div & ~0x0F) >> 1) | ((div & 0x07)); // Set BRR with oversampling by 8
    }
  }
  
  // Enable UART and setting word length, stop bits, and parity in the CR1 and CR2 registers
  huart->Instance->CR1 &= ~USART_CR1_UE;

  // Set word length
  huart->Instance->CR1 &= ~(USART_CR1_M); 
  huart->Instance->CR1 |= (huart->Init.wordLength << USART_CR1_M_Pos);

  // Set pairty
  huart->Instance->CR1 &= ~(USART_CR1_PCE);
  huart->Instance->CR1 |= (huart->Init.Parity << USART_CR1_PCE_Pos);

  // Set oversampling
  huart->Instance->CR1 &= ~(USART_CR1_OVER8);
  huart->Instance->CR1 |= (huart->Init.oversampling << USART_CR1_OVER8_Pos);
  
  // Set stop bits -> The 0.5 Stop bit and 1.5 Stop bit are not available for UART4 & UART5
  huart->Instance->CR2 &= ~(USART_CR2_STOP);
  huart->Instance->CR2 |= (huart->Init.StopBits << USART_CR2_STOP_Pos);

  // Enable the clock
  if (!(huart->Init.Pin == UART4_Conf0 || huart->Init.Pin == UART4_Conf1 || huart->Init.Pin == UART5_Conf0)) {
    huart->Instance->CR2 |= USART_CR2_CLKEN;
  }

  // Enable the UART peripheral
  huart->Instance->CR1 |= USART_CR1_UE;
}

void UART_DeInit(UART_HandleTypeDef *huart) {
// Complete the init function first!
}


void Transmit_Poll(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t timeout) {
  
}