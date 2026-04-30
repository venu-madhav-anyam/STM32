#include <stdint.h>
#include "stm32l4xx_hal.h"

SPI_HandleTypeDef hspi1;

volatile uint8_t tx = 'A';
volatile uint8_t rx;

const uint32_t MSIRangeTable[] = { 100000, 200000, 400000, 800000, 1000000,
		2000000, 4000000, 8000000, 16000000, 24000000, 32000000, 48000000 };

void SysTick_Handler(void) {
	HAL_IncTick();
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef osc = { 0 };
	RCC_ClkInitTypeDef clk = { 0 };

	osc.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	osc.MSIState = RCC_MSI_ON;
	osc.MSIClockRange = RCC_MSIRANGE_8; // 16 MHz
	osc.PLL.PLLState = RCC_PLL_NONE;

	HAL_RCC_OscConfig(&osc);

	clk.ClockType = RCC_CLOCKTYPE_SYSCLK |
	RCC_CLOCKTYPE_HCLK |
	RCC_CLOCKTYPE_PCLK1 |
	RCC_CLOCKTYPE_PCLK2;

	clk.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clk.APB1CLKDivider = RCC_HCLK_DIV1;
	clk.APB2CLKDivider = RCC_HCLK_DIV1;

	HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_0);
}

void SPI1_Init(void) {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_SPI1_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	// PA5=SCK, PA6=MISO, PA7=MOSI
	GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;

	HAL_SPI_Init(&hspi1);

	HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(SPI1_IRQn);
}

void SPI1_IRQHandler(void) {
	HAL_SPI_IRQHandler(&hspi1);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance == SPI1) {
		// Restart continuously
		HAL_SPI_TransmitReceive_IT(&hspi1, (uint8_t*) &tx, (uint8_t*) &rx, 1);
	}
}

int main(void) {
	HAL_Init();
	SystemClock_Config();

	SPI1_Init();

	// Start SPI interrupt transfer
	HAL_SPI_TransmitReceive_IT(&hspi1, (uint8_t*) &tx, (uint8_t*) &rx, 1);

	while (1) {
		// empty loop
	}
}
