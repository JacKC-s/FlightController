#include "uart.h"
#include "FreeRTOS.h"
#include "gpio.h"
#include "stm32f446xx.h"
#include "task.h"
#include "dma.h"
#include <stdio.h>

#define APB1_CLK 45000000U // APB1 clock speed
#define APB2_CLK 90000000U // APB2 clock speed

static UART_HandleTypeDef *g_huart1 = NULL;
static UART_HandleTypeDef *g_huart2 = NULL;
static UART_HandleTypeDef *g_huart3 = NULL;
static UART_HandleTypeDef *g_huart4 = NULL;
static UART_HandleTypeDef *g_huart5 = NULL;
static UART_HandleTypeDef *g_huart6 = NULL;

DMA_Config_t hdma_tx = {0};
DMA_Config_t hdma_rx = {0};

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
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enables the GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Enables the USART1 clock
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_9;
    txConfig.AF_Select = 7;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_10;
    rxConfig.AF_Select = 7;
    pclk = APB2_CLK; // Set peripheral clock speed for USART1
    g_huart1 = huart; // Store the handle for USART1
    NVIC_EnableIRQ(USART1_IRQn); // Enable the USART1 interrupt in the NVIC
    break;
  case USART1_Conf1: // TX: PB6 RX: PB7 AF7
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enables the GPIOB clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Enables the USART1 clock
    txPort = GPIOB;
    txConfig.Pin = GPIO_PIN_6;
    txConfig.AF_Select = 7;
    rxPort = GPIOB;
    rxConfig.Pin = GPIO_PIN_7;
    rxConfig.AF_Select = 7;
    pclk = APB2_CLK; // Set peripheral clock speed for USART1
    g_huart1 = huart; // Store the handle for USART1
    if (huart->Init.DMA_Enable) { 
      huart->Instance->CR3 |= USART_CR3_DMAT;
      // TX
      hdma_tx = (DMA_Config_t){
          .Instance = DMA2,
          .Str_Instance = DMA2_Stream7,
          .Channel = 4,
          .Priority = DMA_PRIORITY_HIGH, // subject to change.
          .Direction = MEMORY_TO_PERIPHERAL,
          .memSize = DMA_MEM_SIZE_8BIT,
          .periSize = DMA_PERIPH_SIZE_8BIT,
          .Mode = DMA_MODE_NORMAL,
          .tran_count = huart->txTranSize,
          .pPeriphAddress = (volatile void *)&huart->Instance->DR,
          .pMemAddress = (void *)huart->pTxBuff,
          .peripheralInc = false,
          .memoryInc = true,
          .half_tran_interrupt = false,
          .tran_complete_interrupt = true,
          .tran_error_interrupt = true
      };

      // RX
      hdma_rx = (DMA_Config_t){
          .Instance = DMA2,
          .Str_Instance = DMA2_Stream2,
          .Channel = 4,
          .Priority = DMA_PRIORITY_HIGH,
          .Direction = PERIPHERAL_TO_MEMORY,
          .memSize = DMA_MEM_SIZE_8BIT,
          .periSize = DMA_PERIPH_SIZE_8BIT,
          .Mode = DMA_MODE_NORMAL,
          .tran_count = huart->rxTranSize,
          .pPeriphAddress = (volatile void *)&huart->Instance->DR, // Pointer to the peripheral
          .pMemAddress = (void *)huart->pRxBuff, // Pointer to the memory buffer
          .peripheralInc = false,
          .memoryInc = true,
          .half_tran_interrupt = false,
          .tran_complete_interrupt = true,
          .tran_error_interrupt = true
      };
      DMA_Init(&hdma_tx);
      DMA_Init(&hdma_rx);
    }
    NVIC_EnableIRQ(USART1_IRQn);
    break;
  /* USART2 */
  case USART2_Conf0: // TX: PA2  RX: PA3  AF7
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enables the GPIOA clock
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Enables the USART2 clock
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_2;
    txConfig.AF_Select = 7;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_3;
    rxConfig.AF_Select = 7;
    pclk = APB1_CLK; // Set peripheral clock speed for USART2
    g_huart2 = huart; // Store the handle for USART2
    NVIC_EnableIRQ(USART2_IRQn);
    break;
  case USART2_Conf1: // TX: PD5  RX: PD6  AF7
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enables the GPIOD clock
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Enables the USART2 clock
    txPort = GPIOD;
    txConfig.Pin = GPIO_PIN_5;
    txConfig.AF_Select = 7;
    rxPort = GPIOD;
    rxConfig.Pin = GPIO_PIN_6;
    rxConfig.AF_Select = 7;
    pclk = APB1_CLK; // Set peripheral clock speed for USART2
    g_huart2 = huart; // Store the handle for USART2
    if (huart->Init.DMA_Enable) {
      huart->Instance->CR3 |= USART_CR3_DMAT;
      hdma_tx = (DMA_Config_t){
          .Instance = DMA1,
          .Str_Instance = DMA1_Stream6,
          .Channel = 4,
          .Priority = DMA_PRIORITY_HIGH,
          .Direction = MEMORY_TO_PERIPHERAL,
          .memSize = DMA_MEM_SIZE_8BIT,
          .periSize = DMA_PERIPH_SIZE_8BIT,
          .Mode = DMA_MODE_NORMAL,
          .tran_count = huart->txTranSize,
          .pPeriphAddress = (volatile void *)&huart->Instance->DR,
          .pMemAddress = (void *)huart->pTxBuff,
          .peripheralInc = false,
          .memoryInc = true,
          .half_tran_interrupt = false,
          .tran_complete_interrupt = true,
          .tran_error_interrupt = true
      };

      hdma_rx = (DMA_Config_t){
          .Instance = DMA1,
          .Str_Instance = DMA1_Stream5,
          .Channel = 4,
          .Priority = DMA_PRIORITY_HIGH,
          .Direction = PERIPHERAL_TO_MEMORY,
          .memSize = DMA_MEM_SIZE_8BIT,
          .periSize = DMA_PERIPH_SIZE_8BIT,
          .Mode = DMA_MODE_NORMAL,
          .tran_count = huart->rxTranSize,
          .pPeriphAddress = (volatile void *)&huart->Instance->DR,
          .pMemAddress = (void *)huart->pRxBuff,
          .peripheralInc = false,
          .memoryInc = true,
          .half_tran_interrupt = false,
          .tran_complete_interrupt = true,
          .tran_error_interrupt = true
      };
      DMA_Init(&hdma_tx);
      DMA_Init(&hdma_rx);
    }
    NVIC_EnableIRQ(USART2_IRQn);
    break;
  /* USART3 */
  case USART3_Conf0: // TX: PB10 RX: PB11 AF7
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enables the GPIOB clock
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // Enables the USART3 clock
    txPort = GPIOB;
    txConfig.Pin = GPIO_PIN_10;
    txConfig.AF_Select = 7;
    rxPort = GPIOB;
    rxConfig.Pin = GPIO_PIN_11;
    rxConfig.AF_Select = 7;
    pclk = APB1_CLK; // Set peripheral clock speed for USART3
    g_huart3 = huart; // Store the handle for USART3
    NVIC_EnableIRQ(USART3_IRQn);
    break;
  case USART3_Conf1: // TX: PC10 RX: PC11 AF7
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // Enables the GPIOC clock
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // Enables the USART3 clock
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_10;
    txConfig.AF_Select = 7;
    rxPort = GPIOC;
    rxConfig.Pin = GPIO_PIN_11;
    rxConfig.AF_Select = 7;
    pclk = APB1_CLK; // Set peripheral clock speed for USART3
    g_huart3 = huart; // Store the handle for USART3
    NVIC_EnableIRQ(USART3_IRQn);
    break;
  case USART3_Conf2: // TX: PD8  RX: PD9  AF7
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enables the GPIOD clock
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // Enables the USART3 clock
    txPort = GPIOD;
    txConfig.Pin = GPIO_PIN_8;
    txConfig.AF_Select = 7;
    rxPort = GPIOD;
    rxConfig.Pin = GPIO_PIN_9;
    rxConfig.AF_Select = 7;
    pclk = APB1_CLK; // Set peripheral clock speed for USART3
    g_huart3 = huart; // Store the handle for USART3
    if (huart->Init.DMA_Enable) {
      huart->Instance->CR3 |= USART_CR3_DMAT;
      hdma_tx = (DMA_Config_t){
          .Instance = DMA1,
          .Str_Instance = DMA1_Stream3,
          .Channel = 4,
          .Priority = DMA_PRIORITY_HIGH,
          .Direction = MEMORY_TO_PERIPHERAL,
          .memSize = DMA_MEM_SIZE_8BIT,
          .periSize = DMA_PERIPH_SIZE_8BIT,
          .Mode = DMA_MODE_NORMAL,
          .tran_count = huart->txTranSize,
          .pPeriphAddress = (volatile void *)&huart->Instance->DR,
          .pMemAddress = (void *)huart->pTxBuff,
          .peripheralInc = false,
          .memoryInc = true,
          .half_tran_interrupt = false,
          .tran_complete_interrupt = true,
          .tran_error_interrupt = true
      };

      hdma_rx = (DMA_Config_t){
          .Instance = DMA1,
          .Str_Instance = DMA1_Stream1,
          .Channel = 4,
          .Priority = DMA_PRIORITY_HIGH,
          .Direction = PERIPHERAL_TO_MEMORY,
          .memSize = DMA_MEM_SIZE_8BIT,
          .periSize = DMA_PERIPH_SIZE_8BIT,
          .Mode = DMA_MODE_NORMAL,
          .tran_count = huart->rxTranSize,
          .pPeriphAddress = (volatile void *)&huart->Instance->DR,
          .pMemAddress = (void *)huart->pRxBuff,
          .peripheralInc = false,
          .memoryInc = true,
          .half_tran_interrupt = false,
          .tran_complete_interrupt = true,
          .tran_error_interrupt = true
      };
      DMA_Init(&hdma_tx);
      DMA_Init(&hdma_rx);
    }
    NVIC_EnableIRQ(USART3_IRQn);
    break;
  /* UART4 */
  case UART4_Conf0: // TX: PA0  RX: PA1  AF8
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enables the GPIOA clock
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN; // Enables the UART4 clock
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_0;
    txConfig.AF_Select = 8;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_1;
    rxConfig.AF_Select = 8;
    pclk = APB1_CLK; // Set peripheral clock speed for UART4
    g_huart4 = huart; // Store the handle for UART4
    NVIC_EnableIRQ(UART4_IRQn);
    break;
  case UART4_Conf1: // TX: PC10 RX: PC11 AF8
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // Enables the GPIOC clock
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN; // Enables the UART4 clock
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_10;
    txConfig.AF_Select = 8;
    rxPort = GPIOC;
    rxConfig.Pin = GPIO_PIN_11;
    rxConfig.AF_Select = 8;
    pclk = APB1_CLK; // Set peripheral clock speed for UART4
    g_huart4 = huart; // Store the handle for UART4
    if (huart->Init.DMA_Enable) {
      huart->Instance->CR3 |= USART_CR3_DMAT;
      hdma_tx = (DMA_Config_t){
          .Instance = DMA1,
          .Str_Instance = DMA1_Stream4,
          .Channel = 4,
          .Priority = DMA_PRIORITY_HIGH,
          .Direction = MEMORY_TO_PERIPHERAL,
          .memSize = DMA_MEM_SIZE_8BIT,
          .periSize = DMA_PERIPH_SIZE_8BIT,
          .Mode = DMA_MODE_NORMAL,
          .tran_count = huart->txTranSize,
          .pPeriphAddress = (volatile void *)&huart->Instance->DR,
          .pMemAddress = (void *)huart->pTxBuff,
          .peripheralInc = false,
          .memoryInc = true,
          .half_tran_interrupt = false,
          .tran_complete_interrupt = true,
          .tran_error_interrupt = true
      };

      hdma_rx = (DMA_Config_t) {
          .Instance = DMA1,
          .Str_Instance = DMA1_Stream2,
          .Channel = 4,
          .Priority = DMA_PRIORITY_HIGH,
          .Direction = PERIPHERAL_TO_MEMORY,
          .memSize = DMA_MEM_SIZE_8BIT,
          .periSize = DMA_PERIPH_SIZE_8BIT,
          .Mode = DMA_MODE_NORMAL,
          .tran_count = huart->rxTranSize,
          .pPeriphAddress = (volatile void *)&huart->Instance->DR,
          .pMemAddress = (void *)huart->pRxBuff,
          .peripheralInc = false,
          .memoryInc = true,
          .half_tran_interrupt = false,
          .tran_complete_interrupt = true,
          .tran_error_interrupt = true
      };
      DMA_Init(&hdma_tx);
      DMA_Init(&hdma_rx);
    }
    NVIC_EnableIRQ(UART4_IRQn);
    break;
  /* UART5 */
  case UART5_Conf0: // TX: PC12 RX: PD2  AF8
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // Enables the GPIOC clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enables the GPIOD clock
    RCC->APB1ENR |= RCC_APB1ENR_UART5EN; // Enables the UART5 clock
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_12;
    txConfig.AF_Select = 8;
    rxPort = GPIOD;
    rxConfig.Pin = GPIO_PIN_2;
    rxConfig.AF_Select = 8;
    pclk = APB1_CLK; // Set peripheral clock speed for UART5
    g_huart5 = huart; // Store the handle for UART5
    if (huart->Init.DMA_Enable) {
      huart->Instance->CR3 |= USART_CR3_DMAT;
      hdma_tx = (DMA_Config_t){
          .Instance = DMA1,
          .Str_Instance = DMA1_Stream7,
          .Channel = 4,
          .Priority = DMA_PRIORITY_HIGH,
          .Direction = MEMORY_TO_PERIPHERAL,
          .memSize = DMA_MEM_SIZE_8BIT,
          .periSize = DMA_PERIPH_SIZE_8BIT,
          .Mode = DMA_MODE_NORMAL,
          .tran_count = huart->txTranSize,
          .pPeriphAddress = (volatile void *)&huart->Instance->DR,
          .pMemAddress = (void *)huart->pTxBuff,
          .peripheralInc = false,
          .memoryInc = true,
          .half_tran_interrupt = false,
          .tran_complete_interrupt = true,
          .tran_error_interrupt = true
      };

      hdma_rx = (DMA_Config_t) {
          .Instance = DMA1,
          .Str_Instance = DMA1_Stream0,
          .Channel = 4,
          .Priority = DMA_PRIORITY_HIGH,
          .Direction = PERIPHERAL_TO_MEMORY,
          .memSize = DMA_MEM_SIZE_8BIT,
          .periSize = DMA_PERIPH_SIZE_8BIT,
          .Mode = DMA_MODE_NORMAL,
          .tran_count = huart->rxTranSize,
          .pPeriphAddress = (volatile void *)&huart->Instance->DR,
          .pMemAddress = (void *)huart->pRxBuff,
          .peripheralInc = false,
          .memoryInc = true,
          .half_tran_interrupt = false,
          .tran_complete_interrupt = true,
          .tran_error_interrupt = true
      };
      DMA_Init(&hdma_tx);
      DMA_Init(&hdma_rx);
    }
    NVIC_EnableIRQ(UART5_IRQn);
    break;
  /* USART6 */
  case USART6_Conf0: // TX: PA11 RX: PA12 AF8
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enables the GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN; // Enables the USART6 clock
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_11;
    txConfig.AF_Select = 8;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_12;
    rxConfig.AF_Select = 8;
    pclk = APB2_CLK; // Set peripheral clock speed for USART6
    g_huart6 = huart; // Store the handle for USART6
    NVIC_EnableIRQ(USART6_IRQn);
    break;
  case USART6_Conf1: // TX: PC6  RX: PC7  AF8
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // Enables the GPIOC clock
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN; // Enables the USART6 clock  
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_6;
    txConfig.AF_Select = 8;
    rxPort = GPIOC;
    rxConfig.Pin = GPIO_PIN_7;
    rxConfig.AF_Select = 8;
    pclk = APB2_CLK; // Set peripheral clock speed for USART6
    g_huart6 = huart; // Store the handle for USART6
    if (huart->Init.DMA_Enable) {
      huart->Instance->CR3 |= USART_CR3_DMAT;
      hdma_tx = (DMA_Config_t){
          .Instance = DMA2,
          .Str_Instance = DMA2_Stream6,
          .Channel = 5,
          .Priority = DMA_PRIORITY_HIGH,
          .Direction = MEMORY_TO_PERIPHERAL,
          .memSize = DMA_MEM_SIZE_8BIT,
          .periSize = DMA_PERIPH_SIZE_8BIT,
          .Mode = DMA_MODE_NORMAL,
          .tran_count = huart->txTranSize,
          .pPeriphAddress = (volatile void *)&huart->Instance->DR,
          .pMemAddress = (void *)huart->pTxBuff,
          .peripheralInc = false,
          .memoryInc = true,
          .half_tran_interrupt = false,
          .tran_complete_interrupt = true,
          .tran_error_interrupt = true
      };

      hdma_rx = (DMA_Config_t){
          .Instance = DMA2,
          .Str_Instance = DMA2_Stream1,
          .Channel = 5,
          .Priority = DMA_PRIORITY_HIGH,
          .Direction = PERIPHERAL_TO_MEMORY,
          .memSize = DMA_MEM_SIZE_8BIT,
          .periSize = DMA_PERIPH_SIZE_8BIT,
          .Mode = DMA_MODE_NORMAL,
          .tran_count = huart->rxTranSize,
          .pPeriphAddress = (volatile void *)&huart->Instance->DR,
          .pMemAddress = (void *)huart->pRxBuff,
          .peripheralInc = false,
          .memoryInc = true,
          .half_tran_interrupt = false,
          .tran_complete_interrupt = true,
          .tran_error_interrupt = true
      };
      DMA_Init(&hdma_tx);
      DMA_Init(&hdma_rx);
    }
    NVIC_EnableIRQ(USART6_IRQn);
    break;
  default:
    return; // Invalid configuration
  }
  // If both pins configured, then enables the gpio pins using the GPIO_Init Function!
  if (txPort && rxPort) {
    GPIO_Init(txPort, &txConfig);
    GPIO_Init(rxPort, &rxConfig);
  }

  // Enable UART and setting word length, stop bits, and parity in the CR1 and CR2 registers
  huart->Instance->CR1 &= ~USART_CR1_UE;
  // Changed to integer math to avoid floating point operations
  if (pclk != 0) {
    if (huart->Init.oversampling == 0) { // Oversampling by 16
      huart->Instance->BRR = (pclk + (huart->Init.baud / 2U)) / huart->Init.baud; // Round to nearest integer
    } else { // Oversampling by 8
    uint32_t div = (2 * (pclk + (huart->Init.baud / 2U))) / huart->Init.baud; // Round to nearest integer
    huart->Instance->BRR = ((div & ~0x0F) >> 1) | ((div & 0x07)); // Set BRR with oversampling by 8
    }
  }
  

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
  huart->Instance->CR1 |= (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE);
}

void UART_DeInit(UART_HandleTypeDef *huart) {

  GPIO_PinConfig_t txConfig = {0};
  GPIO_PinConfig_t rxConfig = {0};
  GPIO_TypeDef *txPort = NULL;
  GPIO_TypeDef *rxPort = NULL;

  // Zeroing out configuration structs to ensure no residual settings

  switch (huart->Init.Pin) {
  /* USART1 */
  case USART1_Conf0: // TX: PA9 RX: PA10 AF7
    RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN; // Disables the USART1 clock
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_9;
    txConfig.Mode = GPIO_MODE_AF;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_10;
    rxConfig.Mode = GPIO_MODE_AF;
    break;
  case USART1_Conf1: // TX: PB6 RX: PB7 AF7
    RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN; // Disables the USART1 clock
    txPort = GPIOB;
    txConfig.Pin = GPIO_PIN_6;
    txConfig.Mode = GPIO_MODE_AF;
    rxPort = GPIOB;
    rxConfig.Pin = GPIO_PIN_7;
    rxConfig.Mode = GPIO_MODE_AF;
    break;
  /* USART2 */
  case USART2_Conf0: // TX: PA2  RX: PA3  AF7
    RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN; // Disables the USART2 clock
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_2;
    txConfig.Mode = GPIO_MODE_AF;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_3;
    rxConfig.Mode = GPIO_MODE_AF;
    break;
  case USART2_Conf1: // TX: PD5  RX: PD6  AF7
    RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN; // Disables the USART2 clock
    txPort = GPIOD;
    txConfig.Pin = GPIO_PIN_5;
    txConfig.Mode = GPIO_MODE_AF;
    rxPort = GPIOD;
    rxConfig.Pin = GPIO_PIN_6;
    rxConfig.Mode = GPIO_MODE_AF;
    break;
  /* USART3 */
  case USART3_Conf0: // TX: PB10 RX: PB11 AF7
    RCC->APB1ENR &= ~RCC_APB1ENR_USART3EN; // Disables the USART3 clock
    txPort = GPIOB;
    txConfig.Pin = GPIO_PIN_10;
    txConfig.Mode = GPIO_MODE_AF;
    rxPort = GPIOB;
    rxConfig.Pin = GPIO_PIN_11;
    rxConfig.Mode = GPIO_MODE_AF;
    break;
  case USART3_Conf1: // TX: PC10 RX: PC11 AF7
    RCC->APB1ENR &= ~RCC_APB1ENR_USART3EN; // Disables the USART3 clock
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_10;
    txConfig.Mode = GPIO_MODE_AF;
    rxPort = GPIOC;
    rxConfig.Pin = GPIO_PIN_11;
    rxConfig.Mode = GPIO_MODE_AF;
    break;
  case USART3_Conf2: // TX: PD8  RX: PD9  AF7
    RCC->APB1ENR &= ~RCC_APB1ENR_USART3EN; // Disables the USART3 clock
    txPort = GPIOD;
    txConfig.Pin = GPIO_PIN_8;    
    txConfig.Mode = GPIO_MODE_AF;
    rxPort = GPIOD;
    rxConfig.Pin = GPIO_PIN_9;
    rxConfig.Mode = GPIO_MODE_AF;
    break;
  /* UART4 */
  case UART4_Conf0: // TX: PA0  RX: PA1  AF8
    RCC->APB1ENR &= ~RCC_APB1ENR_UART4EN; // Disables the UART4 clock
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_0;
    txConfig.Mode = GPIO_MODE_AF;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_1;
    rxConfig.Mode = GPIO_MODE_AF;
    break;
  case UART4_Conf1: // TX: PC10 RX: PC11 AF8
    RCC->APB1ENR &= ~RCC_APB1ENR_UART4EN; // Disables the UART4 clock
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_10;
    txConfig.Mode = GPIO_MODE_AF;
    rxPort = GPIOC;
    rxConfig.Pin = GPIO_PIN_11;
    rxConfig.Mode = GPIO_MODE_AF;
    break;
  /* UART5 */
  case UART5_Conf0: // TX: PC12 RX: PD2  AF8
    RCC->APB1ENR &= ~RCC_APB1ENR_UART5EN; // Disables the UART5 clock
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_12;
    txConfig.Mode = GPIO_MODE_AF;
    rxPort = GPIOD;
    rxConfig.Pin = GPIO_PIN_2;
    rxConfig.Mode = GPIO_MODE_AF;
    break;
  /* USART6 */
  case USART6_Conf0: // TX: PA11 RX: PA12 AF8
    RCC->APB2ENR &= ~RCC_APB2ENR_USART6EN; // Disables the USART6 clock
    txPort = GPIOA;
    txConfig.Pin = GPIO_PIN_11;
    txConfig.Mode = GPIO_MODE_AF;
    rxPort = GPIOA;
    rxConfig.Pin = GPIO_PIN_12;
    rxConfig.Mode = GPIO_MODE_AF;
    break;
  case USART6_Conf1: // TX: PC6  RX: PC7  AF8
    RCC->APB2ENR &= ~RCC_APB2ENR_USART6EN; // Disables the USART6 clock
    txPort = GPIOC;
    txConfig.Pin = GPIO_PIN_6;
    txConfig.Mode = GPIO_MODE_AF;
    rxPort = GPIOC;
    rxConfig.Pin = GPIO_PIN_7;
    rxConfig.Mode = GPIO_MODE_AF;
    break;
  default:
    return; // Invalid configuration
  }
  // If both pins configured, then enables the gpio pins using the GPIO_Init Function!
  if (txPort && rxPort) {
    GPIO_DeInit(txPort, &txConfig);
    GPIO_DeInit(rxPort, &rxConfig);
  }


  // Deinitializing the UART peripherals
  huart->Instance->CR1 &= ~(USART_CR1_UE | USART_CR1_TE | USART_CR1_RE);

  huart->Instance->BRR = 0; // Resetting the baud rate register

  // UnSet word length
  huart->Instance->CR1 &= ~(USART_CR1_M); 

  // UnSet pairty
  huart->Instance->CR1 &= ~(USART_CR1_PCE);

  // UnSet oversampling
  huart->Instance->CR1 &= ~(USART_CR1_OVER8);
  
  huart->Instance->CR2 &= ~(USART_CR2_STOP);

  // disable the clock
  if (!(huart->Init.Pin == UART4_Conf0 || huart->Init.Pin == UART4_Conf1 || huart->Init.Pin == UART5_Conf0)) {
    huart->Instance->CR2 &= ~USART_CR2_CLKEN;
  }

}


USART_Status_t Transmit_Poll(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t timeout) {
  if (huart == NULL || pData == NULL || Size == 0) {
    return USART_STATUS_ERROR; // Invalid parameters
  }

  for (uint16_t i = 0; i < Size; i++) {
    // Wait until TXE (Transmit Data Register Empty) flag is set
    uint32_t startTick = xTaskGetTickCount(); // cool use of FreeRTOS tick count for timeout
    while (!(huart->Instance->SR & USART_SR_TXE)) {
      if ((xTaskGetTickCount() - startTick) > timeout) {
        return USART_STATUS_TIMEOUT; // Timeout occurred
      }
    }

    // Write data to the data register -> this causes the data to be transmitted
    huart->Instance->DR = pData[i];
  }

  // Wait until TC (Transmission Complete) flag is set
  uint32_t startTick = xTaskGetTickCount();
  while (!(huart->Instance->SR & USART_SR_TC)) {
    if ((xTaskGetTickCount() - startTick) > timeout) {
      return USART_STATUS_TIMEOUT; // Timeout occurred
    }

}
return USART_STATUS_OK;
}

USART_Status_t Transmit_Interrupt(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
  if (huart == NULL || pData == NULL || Size == 0) {
    return USART_STATUS_ERROR; // Invalid parameters
  }

  if (huart->txTranRemain != 0) {
    return USART_STATUS_ERROR; // Transmission already in progress
  }

  huart->pTxBuff = pData;
  huart->txTranSize = Size;
  huart->txTranRemain = Size;

  // Enable the TXE interrupt
  huart->Instance->CR1 |= USART_CR1_TXEIE;

  return USART_STATUS_OK;
}

// Functional
void UART_IRQHandler(UART_HandleTypeDef *huart) {
  if ((huart->Instance->SR & USART_SR_TXE) && (huart->Instance->CR1 & USART_CR1_TXEIE)) {
    if (huart->txTranRemain > 0) {
      // Write data to the data register
      huart->Instance->DR = *(huart->pTxBuff);
      huart->pTxBuff++;
      huart->txTranRemain--;
    } else {
      // Transmission complete, disable TXE interrupt
      huart->Instance->CR1 &= ~USART_CR1_TXEIE;
    }

  }
}

// DMA driver is WIP
USART_Status_t Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
  //TODO: Implement DMA flags clearing before reuse
  if (huart == NULL || pData == NULL || Size == 0) {
    return USART_STATUS_ERROR; // Invalid parameters  
  }

  huart->pTxBuff = pData;
  huart->txTranSize = Size;
  huart->txTranRemain = Size;

  if ((huart->txTranRemain != 0) || (hdma_tx.Str_Instance->CR & DMA_SxCR_EN)) {
    return USART_STATUS_ERROR; // Transmission already in progress
  }

  hdma_tx.tran_count = Size; // Set the transfer count for DMA
  hdma_tx.pMemAddress = (void *)pData; // Set the memory address for DMA transfer
  DMA_Init(&hdma_tx); // Re-initialize the DMA with updated parameters
  DMA_Start(&hdma_tx); // Start the DMA transfer
  // TODO: Implement a mechanism to wait for DMA transfer completion or handle it via interrupt
  return USART_STATUS_OK;
}

uint8_t Recieve_Poll(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t timeout) {

}
uint8_t Recieve_Interrupt(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {

}
uint8_t Recieve_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {

}


// Vector table interrupts translated into universal function call to UART_IRQHandler
void USART1_IRQHandler(void) {
  if (g_huart1 != NULL) {
        UART_IRQHandler(g_huart1);
      }
  }

void USART2_IRQHandler(void) {
  if (g_huart2 != NULL) {
        UART_IRQHandler(g_huart2);
      }
  }

void USART3_IRQHandler(void) {
  if (g_huart3 != NULL) {
        UART_IRQHandler(g_huart3);
      }
  }

void UART4_IRQHandler(void) {
  if (g_huart4 != NULL) {
        UART_IRQHandler(g_huart4);
      }
  }

void UART5_IRQHandler(void) {
  if (g_huart5 != NULL) {
        UART_IRQHandler(g_huart5);
      }
  }

void USART6_IRQHandler(void) {
  if (g_huart6 != NULL) {
        UART_IRQHandler(g_huart6);
      }
  }
