#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include <stdint.h>

/* -------- BASE ADDRESSES -------- */
#define PERIPH_BASE        0x40000000UL
#define AHB2PERIPH_BASE    (PERIPH_BASE + 0x08000000UL)

#define GPIOA_BASE         (AHB2PERIPH_BASE + 0x0000UL)
#define RCC_BASE           (PERIPH_BASE + 0x21000UL)

/* -------- GPIO STRUCT -------- */
typedef struct
{
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
} GPIO_TypeDef;

/* -------- RCC STRUCT -------- */
typedef struct
{
    volatile uint32_t CR;
    volatile uint32_t ICSCR;
    volatile uint32_t CFGR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t RESERVED0[4];
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
} RCC_TypeDef;

/* -------- POINTERS -------- */
#define GPIOA ((GPIO_TypeDef*) GPIOA_BASE)
#define RCC   ((RCC_TypeDef*) RCC_BASE)

/* -------- FUNCTION PROTOTYPES -------- */
void GPIOA_EnableClock(void);
void GPIO_Init_Output(GPIO_TypeDef *GPIOx, uint8_t pin);
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t state);
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t pin);
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t pin);

#endif
