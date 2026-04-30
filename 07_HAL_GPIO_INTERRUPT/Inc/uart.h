#ifndef UART_H
#define UART_H

#include "stm32f4xx_hal.h"
#include <stdio.h>

extern UART_HandleTypeDef huart2;

// Function prototypes
void uart_init(void);
int _write(int file, char *ptr, int len);

#endif /* UART_H */
