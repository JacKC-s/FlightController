#ifndef GPIO_H
#define GPIO_H

#include "stm32f446xx.h"

// Not all pins will be available for each Port

#define GPIO_PIN_0   0
#define GPIO_PIN_1   1
#define GPIO_PIN_2   2
#define GPIO_PIN_3   3
#define GPIO_PIN_4   4
#define GPIO_PIN_5   5
#define GPIO_PIN_6   6
#define GPIO_PIN_7   7
#define GPIO_PIN_8   8
#define GPIO_PIN_9   9
#define GPIO_PIN_10  10
#define GPIO_PIN_11  11
#define GPIO_PIN_12  12
#define GPIO_PIN_13  13
#define GPIO_PIN_14  14
#define GPIO_PIN_15  15

// Output mode
typedef enum {
    GPIO_MODE_INPUT = 0x00,
    GPIO_MODE_OUTPUT = 0x01,
    GPIO_MODE_AF = 0x02,
    GPIO_MODE_ANALOG = 0x04
} GPIO_MODE_t;


// Output type
typedef enum {
    GPIO_OTYPE_PP = 0, // Push pull
    GPIO_OTYPE_OD = 1 // Open drain
} GPIO_OTYPE_t;


// Output speed
typedef enum {
    GPIO_SPEED_LOW = 0x00,
    GPIO_SPEED_MED = 0x01,
    GPIO_SPEED_FAST = 0x02,
    GPIO_SPEED_HIGH = 0x03
} GPIO_SPEED_t;

// Output pull-up/pull down
typedef enum {
    GPIO_PUPD_NONE = 0x00,
    GPIO_PUPD_PU = 0x01,
    GPIO_PUPD_PD = 0x02,
    GPIO_PUPD_R = 0x03 
} GPIO_PUPD_t;

// Bit set reset
typedef enum {
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET = 1
} GPIO_STATE_t;

// Configuration for the pin
typedef struct {
    uint8_t Pin;  
    uint8_t Mode;
    uint8_t OType;
    uint8_t Speed;
    uint8_t Pull;
    uint8_t State;
    uint8_t AF_Select;
} GPIO_PinConfig_t;

// Function Definitions
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_PinConfig_t *Config);
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint8_t Pin, GPIO_STATE_t State);
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t Pin);
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t Pin);

#endif