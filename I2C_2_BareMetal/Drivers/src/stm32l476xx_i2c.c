/*
 * stm32l476xx_i2c.c
 *
 *  Created on: May 6, 2026
 *      Author: Venu Madhav Anyam
 */

#include "stm32l476xx_i2c.h"

static void delay_ms(uint32_t time) {
	for (uint32_t i = 0; i < time * 4000; i++) {
		__asm("nop");
	}
}

void I2C_PeriClockControl(I2C_TypeDef *pI2Cx, uint8_t EnOrDi) {
	if (EnOrDi == ENABLE) {
		if (pI2Cx == I2C1) {
			I2C1_CLK_ENABLE();
		} else if (pI2Cx == I2C2) {
			I2C2_CLK_ENABLE();
		} else if (pI2Cx == I2C3) {
			I2C3_CLK_ENABLE();
		}
	} else {
		// disable later if needed
	}
}

void I2C_Init(I2C_Handle_t *pI2CHandle) {
	I2C_PeriClockControl(pI2CHandle->pI2Cx, ENABLE);

	/* Disable peripheral */
	pI2CHandle->pI2Cx->CR1 &= ~(1 << 0);

	/*
	 * TIMINGR value for:
	 * SYSCLK = 4MHz
	 * I2C = 100kHz
	 */
	pI2CHandle->pI2Cx->TIMINGR = 0x00303D5B;

	/* Enable peripheral */
	pI2CHandle->pI2Cx->CR1 |= (1 << 0);
}

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer,
		uint32_t Len, uint8_t SlaveAddr) {
	uint32_t timeout;

	/* Clear CR2 */
	pI2CHandle->pI2Cx->CR2 = 0;

	/* Slave address */
	pI2CHandle->pI2Cx->CR2 |= (SlaveAddr << 1);

	/* Number of bytes */
	pI2CHandle->pI2Cx->CR2 |= (Len << I2C_CR2_NBYTES_Pos);

	/* AUTOEND */
	pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_AUTOEND_Pos);

	/* START */
	pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_START_Pos);

	while (Len > 0) {
		timeout = 100000;

		while (!(pI2CHandle->pI2Cx->ISR & (1 << I2C_ISR_TXIS_Pos))) {
			/* NACK */
			if (pI2CHandle->pI2Cx->ISR & (1 << I2C_ISR_NACKF_Pos)) {
				pI2CHandle->pI2Cx->ICR |= (1 << I2C_ICR_NACKCF_Pos);

				return;
			}

			if (--timeout == 0)
				return;
		}

		pI2CHandle->pI2Cx->TXDR = *pTxBuffer;

		pTxBuffer++;
		Len--;
	}

	timeout = 100000;

	while (!(pI2CHandle->pI2Cx->ISR & (1 << I2C_ISR_STOPF_Pos))) {
		if (--timeout == 0)
			return;
	}

	/* Clear STOP */
	pI2CHandle->pI2Cx->ICR |= (1 << I2C_ICR_STOPCF_Pos);
}

void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer,
		uint32_t Len, uint8_t SlaveAddr) {
	/* Clear CR2 */
	pI2CHandle->pI2Cx->CR2 = 0;

	/*
	 * Slave address
	 */
	pI2CHandle->pI2Cx->CR2 |= (SlaveAddr << 1);

	/* RD_WRN = 1 (Read) */
	pI2CHandle->pI2Cx->CR2 |= (1 << 10);

	/* Number of bytes */
	pI2CHandle->pI2Cx->CR2 |= (Len << 16);

	/* AUTOEND = 1 */
	pI2CHandle->pI2Cx->CR2 |= (1 << 25);

	/* START */
	pI2CHandle->pI2Cx->CR2 |= (1 << 13);

	while (Len > 0) {
		/* Wait until RX buffer not empty */
		while (!(pI2CHandle->pI2Cx->ISR & (1 << I2C_ISR_RXNE_Pos)))
			/* Read data */
			*pRxBuffer = pI2CHandle->pI2Cx->RXDR;

		pRxBuffer++;
		Len--;
	}

	/* Wait for STOP */
	while (!(pI2CHandle->pI2Cx->ISR & (1 << I2C_ISR_STOPF_Pos)))

		/* Clear STOP flag */
		pI2CHandle->pI2Cx->ICR |= (1 << 5);
}

void I2C_ScanBus(I2C_Handle_t *pI2CHandle) {
	//uint8_t dummy = 0;

	//printf("Scanning I2C Bus...\r\n");

	for (uint8_t addr = 1; addr < 128; addr++) {
		/* Clear CR2 */
		pI2CHandle->pI2Cx->CR2 = 0;

		/*
		 * Slave address
		 * NBYTES = 1
		 */
		pI2CHandle->pI2Cx->CR2 |= (addr << 1);
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_NBYTES_Pos);

		/* AUTOEND */
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_AUTOEND_Pos);

		/* START */
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_START_Pos);

		uint32_t timeout = 10000;

		while (!(pI2CHandle->pI2Cx->ISR
				& ((1 << I2C_ISR_STOPF_Pos) | (1 << I2C_ISR_NACKF_Pos)))) {
			if (--timeout == 0)
				break;
		}

		/* Device responded */
		if (!(pI2CHandle->pI2Cx->ISR & (1 << I2C_ISR_NACKF_Pos))) {
			//printf("Device Found at 0x%X\r\n", addr);
		}

		/* Clear STOP */
		if (pI2CHandle->pI2Cx->ISR & (1 << I2C_ISR_STOPF_Pos)) {
			pI2CHandle->pI2Cx->ICR |= (1 << I2C_ICR_STOPCF_Pos);
		}

		/* Clear NACK */
		if (pI2CHandle->pI2Cx->ISR & (1 << I2C_ISR_NACKF_Pos)) {
			pI2CHandle->pI2Cx->ICR |= (1 << I2C_ICR_NACKCF_Pos);
		}

		delay_ms(2);
	}

	//printf("Scan Complete\r\n");
}
