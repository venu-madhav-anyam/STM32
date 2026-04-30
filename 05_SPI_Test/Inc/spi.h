#ifndef SPI_H
#define SPI_H

#include "stm32l4xx_hal.h"

extern SPI_HandleTypeDef hspi1;

void spi_init(void);

#endif
