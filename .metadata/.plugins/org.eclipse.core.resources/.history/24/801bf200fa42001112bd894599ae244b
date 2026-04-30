#include "stm32l4xx_hal.h"

#define BTN_PORT GPIOC
#define BTN_PIN  GPIO_PIN_13

#define LED_PORT GPIOA
#define LED_PIN  GPIO_PIN_5

void pc13_btn_init(void);

void pa5_led_init(void);

GPIO_InitTypeDef GPIO_A_InitStruct = {0};


int main(void)
{
    HAL_Init();

    pc13_btn_init();
    pa5_led_init();

    while (1)
    {
        // PC13 button is usually active LOW
        if (HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) == GPIO_PIN_RESET)
        {
            HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
        }
    }
}

void pa5_led_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_A_InitStruct.Pin = LED_PIN;
    GPIO_A_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_A_InitStruct.Pull = GPIO_NOPULL;
    GPIO_A_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &GPIO_A_InitStruct);
}

void pc13_btn_init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_A_InitStruct.Pin = BTN_PIN;
    GPIO_A_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_A_InitStruct.Pull = GPIO_NOPULL;
    GPIO_A_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOC, &GPIO_A_InitStruct);
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}
