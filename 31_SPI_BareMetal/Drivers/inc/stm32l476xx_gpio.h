/*
 * stm32l476xx_gpio.h
 *
 *  Created on: May 4, 2026
 *      Author: Venu Madhav Anyam
 */

#ifndef INC_STM32L476XX_GPIO_H_
#define INC_STM32L476XX_GPIO_H_

#include "stm32l476xx.h"

/* ================= GPIO MODES ================= */

#define GPIO_MODE_INPUT        0
#define GPIO_MODE_OUTPUT       1
#define GPIO_MODE_ALTFN        2
#define GPIO_MODE_ANALOG       3

/* Interrupt modes */
#define GPIO_MODE_IT_FT        4   // Falling edge
#define GPIO_MODE_IT_RT        5   // Rising edge
#define GPIO_MODE_IT_RFT       6   // Rising + Falling

/* ============ OUTPUT TYPES ============ */

#define GPIO_OP_TYPE_PP        0   // Push-pull
#define GPIO_OP_TYPE_OD        1   // Open-drain

/* ============ OUTPUT SPEED ============ */

#define GPIO_SPEED_LOW         0
#define GPIO_SPEED_MEDIUM      1
#define GPIO_SPEED_FAST        2
#define GPIO_SPEED_HIGH        3

/* ============ PULL-UP / PULL-DOWN ============ */

#define GPIO_NO_PUPD           0
#define GPIO_PIN_PU            1
#define GPIO_PIN_PD            2

typedef struct {
	uint8_t GPIO_PinNumber;
	uint8_t GPIO_PinMode;
	uint8_t GPIO_PinSpeed;
	uint8_t GPIO_PinPuPdControl;
	uint8_t GPIO_PinOPType;
	uint8_t GPIO_PinAltFunMode;
} GPIO_PinConfig_t;

typedef struct {
	GPIO_TypeDef *pGPIOx;
	GPIO_PinConfig_t GPIO_PinConfig;
} GPIO_Handle_t;

//Peripheral Clock setup
void GPIO_PeriClockControl(GPIO_TypeDef *pGPIOx, uint8_t En_or_Dis);

//Init and De-init
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_TypeDef *pGPIOx);

// Data read and write
uint8_t GPIO_ReadFromInputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_TypeDef *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber,
		uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_TypeDef *pGPIOx, uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber);

//IRQ Configuration and ISR handling
void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t En_or_Dis);
void GPIO_IRQHandling(uint8_t PinNumber);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);

#endif /* INC_STM32L476XX_GPIO_H_ */
