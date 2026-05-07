#include "gpio.h"

/* -------- ENABLE CLOCK -------- */
void GPIOA_EnableClock(void) {
	RCC->AHB2ENR |= (1 << 0);   // GPIOA clock enable
}

/* -------- INIT OUTPUT -------- */
void GPIO_Init_Output(GPIO_TypeDef *GPIOx, uint8_t pin) {
	/* MODE = Output (01) */
	GPIOx->MODER &= ~(3 << (2 * pin));
	GPIOx->MODER |= (1 << (2 * pin));

	/* Output type = Push-pull */
	GPIOx->OTYPER &= ~(1 << pin);

	/* Speed = Low */
	GPIOx->OSPEEDR &= ~(3 << (2 * pin));

	/* No pull */
	GPIOx->PUPDR &= ~(3 << (2 * pin));
}

/* -------- WRITE -------- */
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t state) {
	if (state)
		GPIOx->BSRR = (1 << pin);          // SET
	else
		GPIOx->BSRR = (1 << (pin + 16));   // RESET
}

/* -------- TOGGLE -------- */
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t pin) {
	GPIOx->ODR ^= (1 << pin);
}

/* -------- READ -------- */
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t pin) {
	return (GPIOx->IDR & (1 << pin)) ? 1 : 0;
}
