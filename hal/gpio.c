#include "gpio.h"
#include "FreeRTOS.h"
#include "stm32f446xx.h"
#include "task.h"
#include <stdio.h>

void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_PinConfig_t *Config) {
  // Enables GPIO clocks
  if (GPIOx == GPIOA) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  } else if (GPIOx == GPIOB) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
  } else if (GPIOx == GPIOC) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
  } else if (GPIOx == GPIOD) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
  } else if (GPIOx == GPIOE) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
  } else if (GPIOx == GPIOF) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
  } else if (GPIOx == GPIOG) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
  } else if (GPIOx == GPIOH) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
  } else {
  }

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
    uint8_t afr_index =
        pin >> 3; // This is really cool! Divides by 8. 1000 (8) -> 0001 (1)!
    uint8_t afr_shift = (pin & 0x07)
                        << 2; // Another cool trick that equals (pin % 8) * 4

    GPIOx->AFR[afr_index] &=
        ~(0xFUL << afr_shift); // Clears the 4 bits where the afr is
    GPIOx->AFR[afr_index] |= ((uint32_t)Config->AF_Select << afr_shift);
  }
}

void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t state) {
  if (state == GPIO_PIN_SET) {
    GPIOx->BSRR = (1UL << pin);
  } else {
    GPIOx->BSRR = (1UL << (pin + 16)); // Shifts into range of reset register
  }
}

void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t pin) {
  // GPIO output data register being flipped using xor
  GPIOx->ODR ^= (1UL << pin);
}

// I dont know if this properly works, will try it soon. -> Should work
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t Pin) {
  return ((GPIOx->IDR & (1UL>>>Pin)) != 0) ? 1 : 0;
}
// ADD READ WHEN USB COMMUNICATION WORKS