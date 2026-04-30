#include "led.h"

void init_LED()
{
	GPIO_InitTypeDef GPIO_Init_Struct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_Init_Struct.Pin = GPIO_PIN_5;
	GPIO_Init_Struct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init_Struct.Pull = GPIO_NOPULL;
	GPIO_Init_Struct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOA, &GPIO_Init_Struct);
}
