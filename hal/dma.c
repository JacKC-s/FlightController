#include "dma.h"
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
  hdma->Str_Instance->CR &= ~DMA_SxCR_EN; // Disable stream before configuration

  // Set channel selection
  hdma->Str_Instance->CR &= ~DMA_SxCR_CHSEL;
  hdma->Str_Instance->CR |= (hdma->Channel << DMA_SxCR_CHSEL_Pos);

  // Set direction
  hdma->Str_Instance->CR &= ~DMA_SxCR_DIR;
  hdma->Str_Instance->CR |= (hdma->Direction << DMA_SxCR_DIR_Pos);

  // Set memory and peripheral incrememnt 
  hdma->Str_Instance->CR &= ~(DMA_SxCR_MSIZE | DMA_SxCR_PSIZE);
  hdma->Str_Instance->CR |= (hdma->memSize << DMA_SxCR_MSIZE_Pos);
  hdma->Str_Instance->CR |= (hdma->periSize << DMA_SxCR_PSIZE_Pos);
  hdma->Str_Instance->CR &= ~DMA_SxCR_MINC;
  if (hdma->memoryInc) {
    hdma->Str_Instance->CR |= DMA_SxCR_MINC;
  }
  hdma->Str_Instance->CR &= ~DMA_SxCR_PINC;
  if (hdma->peripheralInc) {
    hdma->Str_Instance->CR |= DMA_SxCR_PINC;
  }

  // Set priority level
  hdma->Str_Instance->CR &= ~DMA_SxCR_PL;
  hdma->Str_Instance->CR |= (hdma->Priority << DMA_SxCR_PL_Pos);

  // Set mode (normal or circular) -> Keep on circular for most things!
  if (hdma->Mode == DMA_MODE_CIRCULAR) {
    hdma->Str_Instance->CR |= DMA_SxCR_CIRC;
  } else {
    hdma->Str_Instance->CR &= ~DMA_SxCR_CIRC;
  }

  // Number of data items to transfer
  hdma->Str_Instance->NDTR = 0;
  
  hdma->Str_Instance->NDTR = hdma->tran_count;
  hdma->Str_Instance->PAR = (uint32_t)hdma->pPeriphAddress;
  hdma->Str_Instance->M0AR = (uint32_t)hdma->pMemAddress;

  // Enable HTIE, HCIE, HEIE
  hdma->Str_Instance->CR &= ~DMA_SxCR_HTIE;
  if (hdma->half_tran_interrupt) {
    hdma->Str_Instance->CR |= DMA_SxCR_HTIE;
  }
  hdma->Str_Instance->CR &= ~DMA_SxCR_TCIE;
  if (hdma->tran_complete_interrupt) {
    hdma->Str_Instance->CR |= DMA_SxCR_TCIE;
  }
  hdma->Str_Instance->CR &= ~DMA_SxCR_TEIE;
  if (hdma->tran_error_interrupt) {
    hdma->Str_Instance->CR |= DMA_SxCR_TEIE;
  }

}

void DMA_DeInit(DMA_Config_t *hdma) {
  // Disable the DMA stream
  hdma->Str_Instance->CR &= ~DMA_SxCR_EN;

  // Reset the DMA stream configuration
  hdma->Str_Instance->CR = 0;
  hdma->Str_Instance->NDTR = 0;
  hdma->Str_Instance->PAR = 0;
  hdma->Str_Instance->M0AR = 0;

  hdma->Str_Instance->CR &= ~DMA_SxCR_CHSEL;
  hdma->Str_Instance->CR &= ~DMA_SxCR_DIR;
  hdma->Str_Instance->CR &= ~(DMA_SxCR_MSIZE | DMA_SxCR_PSIZE);
  hdma->Str_Instance->CR &= ~DMA_SxCR_PL;
  hdma->Str_Instance->CR &= ~DMA_SxCR_CIRC;
}

void DMA_Start(DMA_Config_t *hdma) {
  // Enable the DMA stream
  hdma->Str_Instance->CR |= DMA_SxCR_EN;
}

void DMA_Stop(DMA_Config_t *hdma) {
  // Disable the DMA stream
  hdma->Str_Instance->CR &= ~DMA_SxCR_EN;
}
