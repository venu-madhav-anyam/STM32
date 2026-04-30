#ifndef ADC_H
#define ADC_H

#include "stm32f4xx_hal.h"

#define ADC_BUF_LEN 10

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern uint32_t adc_buffer[ADC_BUF_LEN];

void adc_dma_init(void);
void adc_start_dma(void);

#endif
