#include "FreeRTOS.h"
#include "gpio.h"
#include "stm32f446xx.h"
#include "task.h"

#define STEP_PORT GPIOA
#define STEP_PIN 5
#define DIR_PORT GPIOA
#define DIR_PIN 6

static void vStepTask(void *pvParameters) {
  (void)pvParameters;


  
  for (;;) {
    GPIO_WritePin(STEP_PORT, STEP_PIN, GPIO_PIN_SET);
    vTaskDelay(pdMS_TO_TICKS(1000));
    GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_RESET);
    vTaskDelay(pdMS_TO_TICKS(1000));

    GPIO_WritePin(STEP_PORT, STEP_PIN, GPIO_PIN_RESET);
    vTaskDelay(pdMS_TO_TICKS(1000));
    GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_SET);
    vTaskDelay(pdMS_TO_TICKS(1000));

  }
}

int main(void) {

  GPIO_PinConfig_t stepConf = {.Pin = STEP_PIN,
                              .Mode = GPIO_MODE_OUTPUT,
                              .OType = GPIO_OTYPE_PP,
                              .Speed = GPIO_SPEED_HIGH,
                              .Pull = GPIO_PUPD_NONE,
                              .AF_Select = 0};

  GPIO_PinConfig_t dirConf = {.Pin = DIR_PIN,
                              .Mode = GPIO_MODE_OUTPUT,
                              .OType = GPIO_OTYPE_PP,
                              .Speed = GPIO_SPEED_HIGH,
                              .Pull = GPIO_PUPD_NONE,
                              .AF_Select = 0};

  GPIO_Init(STEP_PORT, &stepConf);
  GPIO_Init(DIR_PORT, &dirConf);
  GPIO_WritePin(STEP_PORT, STEP_PIN, GPIO_PIN_RESET);
  GPIO_WritePin(DIR_PORT, DIR_PIN, GPIO_PIN_RESET);

  if (xTaskCreate(vStepTask, "STEP", 128, NULL, 3, NULL) != pdPASS) {
    while (1)
      ;
  }

  vTaskStartScheduler();

  while (1)
    ;

}
