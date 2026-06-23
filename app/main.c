#include "FreeRTOS.h"
#include "gpio.h"
#include "stm32f446xx.h"
#include "task.h"

#define LED_PORT GPIOA
#define LED_PIN 5

// LED Blink Task

static void vLEDTask(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    GPIO_TogglePin(LED_PORT, LED_PIN);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

int main(void) {

  GPIO_PinConfig_t ledConf = {.Pin = LED_PIN,
                              .Mode = GPIO_MODE_OUTPUT,
                              .OType = GPIO_OTYPE_PP,
                              .Speed = GPIO_SPEED_HIGH,
                              .Pull = GPIO_PUPD_NONE,
                              .AF_Select = 0};

  GPIO_Init(LED_PORT, &ledConf);

  if (xTaskCreate(vLEDTask, "LED Blink", 128, NULL, 1, NULL) != pdPASS) {
    while (1)
      ;
  }

  vTaskStartScheduler();

  while (1)
    ;
}