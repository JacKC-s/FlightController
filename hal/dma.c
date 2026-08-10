#include dma.h
#include "FreeRTOS.h"
#include "gpio.h"
#include "stm32f446xx.h"
#include "task.h"
#include <stdio.h>

void DMA_Init(DMA_Config_t *hdma) {
  // Enable the clock for the DMA controller
  if (hdma->Instance == DMA1) {
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
  } else if (hdma->Instance == DMA2) {
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
  }

  // Configure DMA stream
  hdma->Instance->CR &= ~DMA_SxCR_EN; // Disable stream before configuration

  // Set channel selection
  hdma->Instance->CR &= ~DMA_SxCR_CHSEL;
  hdma->Instance->CR |= (hdma->Channel << DMA_SxCR_CHSEL_Pos);

  // Set direction
  hdma->Instance->CR &= ~DMA_SxCR_DIR;
  hdma->Instance->CR |= (hdma->Direction << DMA_SxCR_DIR_Pos);

  // Set memory and peripheral incrememnt 
  hdma->Instance->CR &= ~(DMA_SxCR_MSIZE | DMA_SxCR_PSIZE);
  hdma->Instance->CR |= (hdma->memSize << DMA_SxCR_MSIZE_Pos);
  hdma->Instance->CR |= (hdma->periSize << DMA_SxCR_PSIZE_Pos);

  // Set priority level
  hdma->Instance->CR &= ~DMA_SxCR_PL;
  hdma->Instance->CR |= (hdma->Priority << DMA_SxCR_PL_Pos);

  // Set mode (normal or circular) -> Keep on circular for most things!
  if (hdma->Mode == DMA_MODE_CIRCULAR) {
    hdma->Instance->CR |= DMA_SxCR_CIRC;
  } else {
    hdma->Instance->CR &= ~DMA_SxCR_CIRC;
  }

  // Number of data items to transfer
  hdma->Instance->NDTR = 0;
  
  hdma->Instance->NDTR = hdma->tran_count;
  hdma->Instance->PAR = (uint32_t)hdma->pPeriphAddress;
  hdma->Instance->M0AR = (uint32_t)hdma->pMemAddress;
}

void DMA_Start(DMA_Config_t *hdma) {
  // Enable the DMA stream
  hdma->Instance->CR |= DMA_SxCR_EN;
}

void DMA_Stop(DMA_Config_t *hdma) {
  // Disable the DMA stream
  hdma->Instance->CR &= ~DMA_SxCR_EN;
}
