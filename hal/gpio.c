#include "FreeRTOS.h"
#include "task.h"
#include "stm32f446xx.h"
#include <stdio.h>
#include "gpio.h"

void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_PinConfig_t *Config) {
    // Get pin
    uint8_t pin = Config->Pin;

    // Set mode
    GPIOx->MODER &= ~(3UL << (pin * 2)); // multipled by two because two bit register. 
    // The 3UL is just 3 in binary being bitshifted 
    GPIOx->MODER |= ((uint32_t)Config->Mode << (pin * 2));

    // Set output type
    GPIOx->OTYPER &= ~(1UL << pin); // 1UL because only one bit
    GPIOx->OTYPER |= ((uint32_t)Config->OType << pin);

    // Set Output Speed
    GPIOx->OSPEEDR &= ~(3UL << (pin * 2));
    GPIOx->OSPEEDR |= ((uint32_t)Config->Speed << (pin * 2));

    // Set Push Pull -> usually none
    GPIOx->PUPDR &= ~(3UL << (pin * 2));
    GPIOx->PUPDR |= ((uint32_t)Config->Pull << (pin * 2));

    // Setting Alternate Function
    if (Config->Mode == GPIO_MODE_AF) {
        // To find if it is high or low register
        uint8_t afr_index = pin >> 3; // This is really cool! Divides by 8. 1000 (8) -> 0001 (1)!
        uint8_t // Continue here
    }

}

