#ifndef UART_H
#define UART_H

#include "stm32l4xx_hal.h"

#define UART_RX_BUF_SIZE 100

extern UART_HandleTypeDef huart2;
extern uint8_t rx_buffer[UART_RX_BUF_SIZE];

void uart_init(void);
void uart_start_dma(void);
void uart_process_data(void);

int _write(int file, char *ptr, int len);

#endif
