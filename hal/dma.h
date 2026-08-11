#ifndef DMA_H
#define DMA_H

#include "stm32f446xx.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

//TODO: Add dma for mem to mem

typedef enum {
    PERIPHERAL_TO_MEMORY = 0,
    MEMORY_TO_PERIPHERAL = 1,
    MEMORY_TO_MEMORY = 2,
    RESERVED = 3
} DMA_Direction_t;

typedef enum {
    DMA_PRIORITY_LOW = 0,
    DMA_PRIORITY_MEDIUM = 1,
    DMA_PRIORITY_HIGH = 2,
    DMA_PRIORITY_VERY_HIGH = 3
} DMA_Priority_t;

typedef enum {
    DMA_MEM_SIZE_8BIT = 0,
    DMA_MEM_SIZE_16BIT = 1,
    DMA_MEM_SIZE_32BIT = 2,
    DMA_MEM_SIZE_RESERVED = 3
} DMA_Mem_Size_t;

typedef enum {
    DMA_PERIPH_SIZE_8BIT = 0,
    DMA_PERIPH_SIZE_16BIT = 1,
    DMA_PERIPH_SIZE_32BIT = 2,
    DMA_PERIPH_SIZE_RESERVED = 3
} DMA_Periph_Size_t;

typedef enum {
    DMA_MODE_NORMAL = 0,
    DMA_MODE_CIRCULAR = 1
} DMA_Mode_t;


typedef struct {
    void *Owner;
    DMA_TypeDef *Instance; 
    uint8_t Channel; // DMA channel number
    DMA_Stream_TypeDef *Str_Instance; // Pointer to the actual DMA stream.
    DMA_Priority_t Priority; // Priority level of the DMA stream
    DMA_Direction_t Direction; // Direction of data transfer
    DMA_Mem_Size_t memSize; // Memory data size
    DMA_Periph_Size_t periSize; // Peripheral data size
    DMA_Mode_t Mode; // Mode of operation (normal or circular)
    uint16_t tran_count;

    volatile void *pPeriphAddress;
    void *pMemAddress;

    bool peripheralInc; // Peripheral increment mode
    bool memoryInc; // Memory increment mode

    bool half_tran_interrupt; // Half transfer interrupt enable
    bool tran_complete_interrupt; // Transfer complete interrupt enable
    bool tran_error_interrupt; // Transfer error interrupt enable
} DMA_Config_t;

void DMA_Init(DMA_Config_t *hdma);
void DMA_DeInit(DMA_Config_t *hdma);
void DMA_Start(DMA_Config_t *hdma);
void DMA_Stop(DMA_Config_t *hdma);

void DMA_IRQHandler(DMA_Config_t *hdma);

#endif
