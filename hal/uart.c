#include "uart.h"
#include "FreeRTOS.h"
#include "gpio.h"
#include "stm32f446xx.h"
#include "task.h"
#include <stdio.h>

// Initialization function uses gpio driver

void UART_Init(UART_HandleTypeDef *huart) {
  // Setting up pins as null originally, then having a switch case tree decide
  // what pins to use
  GPIO_PinConfig_t txConfig = {0};
  GPIO_PinConfig_t rxConfig = {0};
  GPIO_TypeDef *txPort = NULL;
  GPIO_TypeDef *rxPort = NULL;

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
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_9;
    txConfig.AF_Select = 7;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_10;
    rxConfig.AF_Select = 7;
    break;
  case USART1_Conf1: // TX: PB6 RX: PB7 AF7
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    txPort = GPIOB;
    txConfig.Pin = GPIO_PIN_6;
    txConfig.AF_Select = 7;
    rxPort = GPIOB;
    rxConfig.Pin = GPIO_PIN_7;
    rxConfig.AF_Select = 7;
    break;
  /* USART2 */
  case USART2_Conf0: // TX: PA2  RX: PA3  AF7
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_2;
    txConfig.AF_Select = 7;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_3;
    rxConfig.AF_Select = 7;
    break;
  case USART2_Conf1: // TX: PD5  RX: PD6  AF7
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    txPort = GPIOD;
    txConfig.Pin = GPIO_PIN_5;
    txConfig.AF_Select = 7;
    rxPort = GPIOD;
    rxConfig.Pin = GPIO_PIN_6;
    rxConfig.AF_Select = 7;
    break;
  /* USART3 */
  case USART3_Conf0: // TX: PB10 RX: PB11 AF7
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    txPort = GPIOB;
    txConfig.Pin = GPIO_PIN_10;
    txConfig.AF_Select = 7;
    rxPort = GPIOB;
    rxConfig.Pin = GPIO_PIN_11;
    rxConfig.AF_Select = 7;
    break;
  case USART3_Conf1: // TX: PC10 RX: PC11 AF7
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_10;
    txConfig.AF_Select = 7;
    rxPort = GPIOC;
    rxConfig.Pin = GPIO_PIN_11;
    rxConfig.AF_Select = 7;
    break;
  case USART3_Conf2: // TX: PD8  RX: PD9  AF7
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    txPort = GPIOD;
    txConfig.Pin = GPIO_PIN_8;
    txConfig.AF_Select = 7;
    rxPort = GPIOD;
    rxConfig.Pin = GPIO_PIN_9;
    rxConfig.AF_Select = 7;
    break;
  /* UART4 */
  case UART4_Conf0: // TX: PA0  RX: PA1  AF8
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_0;
    txConfig.AF_Select = 8;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_1;
    rxConfig.AF_Select = 8;
    break;
  case UART4_Conf1: // TX: PC10 RX: PC11 AF8
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_10;
    txConfig.AF_Select = 8;
    rxPort = GPIOC;
    rxConfig.Pin = GPIO_PIN_11;
    rxConfig.AF_Select = 8;
    break;
  /* UART5 */
  case UART5_Conf0: // TX: PC12 RX: PD2  AF8
    RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_12;
    txConfig.AF_Select = 8;
    rxPort = GPIOD;
    rxConfig.Pin = GPIO_PIN_2;
    rxConfig.AF_Select = 8;
    break;
  /* USART6 */
  case USART6_Conf0: // TX: PA11 RX: PA12 AF8
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_11;
    txConfig.AF_Select = 8;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_12;
    rxConfig.AF_Select = 8;
    break;
  case USART6_Conf1: // TX: PC6  RX: PC7  AF8
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_6;
    txConfig.AF_Select = 8;
    rxPort = GPIOC;
    rxConfig.Pin = GPIO_PIN_7;
    rxConfig.AF_Select = 8;
    break;
  default:
    return; // Invalid configuration
  }
  // If both pins configured, then enables the gpio pins using the GPIO_Init
  // Function!
  if (txPort && rxPort) {
    GPIO_Init(txPort, &txConfig);
    GPIO_Init(rxPort, &rxConfig);
  }

  huart->Instance->CR1 &= ~(USART_CR1_UE); // Disables UART for configuration

  huart->Instance->BRR = SystemCoreClock / huart->Init.baud; // sets Baud Rate

  huart->Instance->CR1 &=
      ~(USART_CR1_M | USART_CR1_PCE |
        USART_CR1_PS); // Clears word length, parity control, enable, and parity
                       // selection bits

  huart->Instance->CR2 &= ~(USART_CR2_STOP); // Clears stop bits

  // 0 = 8 bits, 1 = 9 bits
  if (huart->Init.WordLength == 9) {
    huart->Instance->CR1 |= USART_CR1_M; // 9-bit word length
  }

  // Parity Even = 0 Parity Odd = 1
  if (huart->Init.Parity == 0) {
    huart->Instance->CR1 |= USART_CR1_PCE; // Just enables parity control
  } else if (huart->Init.Parity == 1) {
    huart->Instance->CR1 |=
        USART_CR1_PCE |
        USART_CR1_PS; // Enables parity control and puts 1 into the parity bit
  }

  // 00 = 1 stop bit, 01 = 0.5 stop bit, 10 = 2 stop bits, 11 = 1.5 stop bits
  huart->Instance->CR2 &= ~(USART_CR2_STOP); // Clears stop bits for setting
  if (huart->Init.StopBits == USART_STOPBITS_1) {
    huart->Instance->CR2 &= ~(USART_CR2_STOP_0 | USART_CR2_STOP_1);
  } else if (huart->Init.StopBits == USART_STOPBITS_0_5) {
    huart->Instance->CR2 |= USART_CR2_STOP_0;
  } else if (huart->Init.StopBits == USART_STOPBITS_2) {
    huart->Instance->CR2 |= USART_CR2_STOP_1;
  } else if (huart->Init.StopBits == USART_STOPBITS_1_5) {
    huart->Instance->CR2 |= USART_CR2_STOP_0 | USART_CR2_STOP_1;
  }

  huart->Instance->CR1 |= USART_CR1_UE; // Finally enables the UART peripherial
}

void UART_DeInit(UART_HandleTypeDef *huart) {
  // Need to make a GPIO_DeInit Function...
}