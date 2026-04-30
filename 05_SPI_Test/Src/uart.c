#include "uart.h"
#include <string.h>

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

uint8_t rx_buffer[UART_RX_BUF_SIZE];

void uart_init(void) {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	// PA2 → TX, PA3 → RX
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// UART config
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_UART_Init(&huart2);

	// -------- DMA CONFIG --------
	hdma_usart2_rx.Instance = DMA1_Channel6;
	hdma_usart2_rx.Init.Request = DMA_REQUEST_2; // USART2_RX
	hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
	hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;

	HAL_DMA_Init(&hdma_usart2_rx);

	// Link DMA to UART
	__HAL_LINKDMA(&huart2, hdmarx, hdma_usart2_rx);

	// Enable DMA IRQ
	HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

	// Enable UART IRQ
	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	// Enable IDLE interrupt
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
}

void uart_start_dma(void) {
	HAL_UART_Receive_DMA(&huart2, rx_buffer, UART_RX_BUF_SIZE);
}

void uart_process_data(void) {
	uint16_t len = UART_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart2.hdmarx);

	if (len > 0) {
		// Echo back
		HAL_UART_Transmit(&huart2, rx_buffer, len, 100);

		// Clear buffer (optional)
		memset(rx_buffer, 0, UART_RX_BUF_SIZE);
	}
}

int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, 100);
    return len;
}
