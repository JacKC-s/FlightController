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

// The IRQHandler is just a bunch of rote code
void DMA_IRQHandler(DMA_Config_t *hdma) {
  if (hdma == NULL || hdma->Instance == NULL || hdma->Str_Instance == NULL) {
    return;
  }

  uint32_t status;
  uint32_t transferComplete;
  uint32_t transferError;
  uint32_t clearFlags;
  volatile uint32_t *clearRegister;

  if (hdma->Str_Instance == DMA1_Stream0 || hdma->Str_Instance == DMA2_Stream0) {
    status = hdma->Instance->LISR;
    transferComplete = DMA_LISR_TCIF0;
    transferError = DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 | DMA_LISR_TEIF0;
    clearFlags = DMA_LIFCR_CFEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CTEIF0 |
                 DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTCIF0;
    clearRegister = &hdma->Instance->LIFCR;
  } else if (hdma->Str_Instance == DMA1_Stream1 || hdma->Str_Instance == DMA2_Stream1) {
    status = hdma->Instance->LISR;
    transferComplete = DMA_LISR_TCIF1;
    transferError = DMA_LISR_FEIF1 | DMA_LISR_DMEIF1 | DMA_LISR_TEIF1;
    clearFlags = DMA_LIFCR_CFEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CTEIF1 |
                 DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTCIF1;
    clearRegister = &hdma->Instance->LIFCR;
  } else if (hdma->Str_Instance == DMA1_Stream2 || hdma->Str_Instance == DMA2_Stream2) {
    status = hdma->Instance->LISR;
    transferComplete = DMA_LISR_TCIF2;
    transferError = DMA_LISR_FEIF2 | DMA_LISR_DMEIF2 | DMA_LISR_TEIF2;
    clearFlags = DMA_LIFCR_CFEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CTEIF2 |
                 DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTCIF2;
    clearRegister = &hdma->Instance->LIFCR;
  } else if (hdma->Str_Instance == DMA1_Stream3 || hdma->Str_Instance == DMA2_Stream3) {
    status = hdma->Instance->LISR;
    transferComplete = DMA_LISR_TCIF3;
    transferError = DMA_LISR_FEIF3 | DMA_LISR_DMEIF3 | DMA_LISR_TEIF3;
    clearFlags = DMA_LIFCR_CFEIF3 | DMA_LIFCR_CDMEIF3 | DMA_LIFCR_CTEIF3 |
                 DMA_LIFCR_CHTIF3 | DMA_LIFCR_CTCIF3;
    clearRegister = &hdma->Instance->LIFCR;
  } else if (hdma->Str_Instance == DMA1_Stream4 || hdma->Str_Instance == DMA2_Stream4) {
    status = hdma->Instance->HISR;
    transferComplete = DMA_HISR_TCIF4;
    transferError = DMA_HISR_FEIF4 | DMA_HISR_DMEIF4 | DMA_HISR_TEIF4;
    clearFlags = DMA_HIFCR_CFEIF4 | DMA_HIFCR_CDMEIF4 | DMA_HIFCR_CTEIF4 |
                 DMA_HIFCR_CHTIF4 | DMA_HIFCR_CTCIF4;
    clearRegister = &hdma->Instance->HIFCR;
  } else if (hdma->Str_Instance == DMA1_Stream5 || hdma->Str_Instance == DMA2_Stream5) {
    status = hdma->Instance->HISR;
    transferComplete = DMA_HISR_TCIF5;
    transferError = DMA_HISR_FEIF5 | DMA_HISR_DMEIF5 | DMA_HISR_TEIF5;
    clearFlags = DMA_HIFCR_CFEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CTEIF5 |
                 DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTCIF5;
    clearRegister = &hdma->Instance->HIFCR;
  } else if (hdma->Str_Instance == DMA1_Stream6 || hdma->Str_Instance == DMA2_Stream6) {
    status = hdma->Instance->HISR;
    transferComplete = DMA_HISR_TCIF6;
    transferError = DMA_HISR_FEIF6 | DMA_HISR_DMEIF6 | DMA_HISR_TEIF6;
    clearFlags = DMA_HIFCR_CFEIF6 | DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CTEIF6 |
                 DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTCIF6;
    clearRegister = &hdma->Instance->HIFCR;
  } else if (hdma->Str_Instance == DMA1_Stream7 || hdma->Str_Instance == DMA2_Stream7) {
    status = hdma->Instance->HISR;
    transferComplete = DMA_HISR_TCIF7;
    transferError = DMA_HISR_FEIF7 | DMA_HISR_DMEIF7 | DMA_HISR_TEIF7;
    clearFlags = DMA_HIFCR_CFEIF7 | DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CTEIF7 |
                 DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTCIF7;
    clearRegister = &hdma->Instance->HIFCR;
  } else {
    return;
  }

  if (status & transferError) {
    DMA_Stop(hdma);
    *clearRegister = clearFlags;
    if (hdma->transferErrorCallback != NULL) {
    hdma->transferErrorCallback(hdma);
	}
    return;
  }

  if (status & transferComplete) {
    *clearRegister = clearFlags;
    if (hdma->transferCompleteCallback != NULL) {
    hdma->transferCompleteCallback(hdma);
    }
  }
}
