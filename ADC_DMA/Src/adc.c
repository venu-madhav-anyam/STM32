#include "adc.h"

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

void adc_dma_init(void) {
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_DMA2_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	// PA0 → analog
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// ADC config
	hadc1.Instance = ADC1;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = DISABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;

	HAL_ADC_Init(&hadc1);

	// Channel config
	ADC_ChannelConfTypeDef sConfig = { 0 };
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;

	HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	// ---------------- DMA CONFIG ----------------
	hdma_adc1.Instance = DMA2_Stream0;
	hdma_adc1.Init.Channel = DMA_CHANNEL_0;
	hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
	hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_adc1.Init.Mode = DMA_CIRCULAR;   // ⭐ continuous mode
	hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;

	HAL_DMA_Init(&hdma_adc1);

	// Link DMA with ADC
	__HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);

	// Enable DMA interrupt
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

