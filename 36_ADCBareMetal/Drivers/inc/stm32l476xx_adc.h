/*
 * stm32l476xx_adc.h
 *
 *  Created on: May 11, 2026
 *      Author: Venu Madhav Anyam
 */

#ifndef INC_STM32L476XX_ADC_H_
#define INC_STM32L476XX_ADC_H_

#include "stm32l476xx.h"


void ADC_PeriClockControl(ADC_TypeDef *pADCx,
                          uint8_t EnOrDi);

void ADC_Init(void);

uint16_t ADC_Read(void);

#endif /* INC_STM32L476XX_ADC_H_ */
