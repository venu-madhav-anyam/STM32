/*
 * stm32l476xx_i2c.c
 *
 *  Created on: May 6, 2026
 *      Author: Venu Madhav Anyam
 */

#include "stm32l476xx_i2c.h"

void delay_ms(uint32_t time) {
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
	pI2CHandle->pI2Cx->TIMINGR = 0x00707CBB;

	/* Enable peripheral */
	pI2CHandle->pI2Cx->CR1 |= (1 << 0);
}

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer,
		uint32_t Len, uint8_t SlaveAddr) {
	uint32_t timeout;

	/* Clear flags */
	pI2CHandle->pI2Cx->ICR |= I2C_ICR_STOPCF | I2C_ICR_NACKCF;

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

		while (!(pI2CHandle->pI2Cx->ISR &
		I2C_ISR_TXIS)) {
			if (pI2CHandle->pI2Cx->ISR &
			I2C_ISR_NACKF) {
				return;
			}

			if (--timeout == 0) {
				return;
			}
		}

		pI2CHandle->pI2Cx->TXDR = *pTxBuffer;

		pTxBuffer++;
		Len--;
	}

	while (!(pI2CHandle->pI2Cx->ISR &
	I2C_ISR_STOPF)) {
		;
	}

	/* Clear STOP */
	pI2CHandle->pI2Cx->ICR |= I2C_ICR_STOPCF;

	/* Reset CR2 */
	pI2CHandle->pI2Cx->CR2 = 0;
}

void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer,
		uint32_t Len, uint8_t SlaveAddr) {
	uint32_t timeout;

	/* Clear flags */
	pI2CHandle->pI2Cx->ICR |= I2C_ICR_STOPCF | I2C_ICR_NACKCF;

	/* Clear CR2 */
	pI2CHandle->pI2Cx->CR2 = 0;

	/* Slave address */
	pI2CHandle->pI2Cx->CR2 |= (SlaveAddr << 1);

	/* READ mode */
	pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_RD_WRN_Pos);

	/* Number of bytes */
	pI2CHandle->pI2Cx->CR2 |= (Len << I2C_CR2_NBYTES_Pos);

	/* AUTOEND */
	pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_AUTOEND_Pos);

	/* START */
	pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_START_Pos);

	while (Len > 0) {
		timeout = 100000;

		/* Wait until RXNE */
		while (!(pI2CHandle->pI2Cx->ISR &
		I2C_ISR_RXNE)) {
			/* NACK */
			if (pI2CHandle->pI2Cx->ISR &
			I2C_ISR_NACKF) {
				return;
			}

			/* Timeout */
			if (--timeout == 0) {
				return;
			}
		}

		/* Read data */
		*pRxBuffer = pI2CHandle->pI2Cx->RXDR;

		pRxBuffer++;
		Len--;
	}

	timeout = 100000;

	/* Wait STOP */
	while (!(pI2CHandle->pI2Cx->ISR &
	I2C_ISR_STOPF)) {
		if (--timeout == 0) {
			return;
		}
	}

	/* Clear STOP */
	pI2CHandle->pI2Cx->ICR |= I2C_ICR_STOPCF;

	/* Reset CR2 */
	pI2CHandle->pI2Cx->CR2 = 0;
}

void I2C_ScanBus(I2C_Handle_t *pI2CHandle) {
	//printf("Scanning I2C Bus...\r\n");

	for (uint8_t addr = 1; addr < 128; addr++) {
		/* Clear flags */
		pI2CHandle->pI2Cx->ICR |=
		I2C_ICR_STOPCF |
		I2C_ICR_NACKCF;

		/* Clear CR2 */
		pI2CHandle->pI2Cx->CR2 = 0;

		/* Slave address */
		pI2CHandle->pI2Cx->CR2 |= (addr << 1);

		/* 1 byte */
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_NBYTES_Pos);

		/* AUTOEND */
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_AUTOEND_Pos);

		/* START */
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_START_Pos);

		uint32_t timeout = 10000;

		while (1) {
			uint32_t isr = pI2CHandle->pI2Cx->ISR;

			/* ADDRESS ACKED */
			if (isr & I2C_ISR_TXIS) {
				//printf("Device Found at 0x%X\r\n", addr);

				/* send dummy byte */
				pI2CHandle->pI2Cx->TXDR = 0x00;

				break;
			}

			/* NACK */
			if (isr & I2C_ISR_NACKF) {
				break;
			}

			if (--timeout == 0) {
				break;
			}
		}

		/* Wait STOP */
		timeout = 10000;

		while (!(pI2CHandle->pI2Cx->ISR &
		I2C_ISR_STOPF)) {
			if (--timeout == 0)
				break;
		}

		/* Clear flags */
		pI2CHandle->pI2Cx->ICR |=
		I2C_ICR_STOPCF |
		I2C_ICR_NACKCF;

		delay_ms(2);
	}

	//printf("Scan Complete\r\n");
}

uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t SlaveAddr,
		uint8_t reg, uint8_t *pRxBuffer, uint32_t Len) {
	if (pI2CHandle->TxRxState != I2C_READY)
		return 0;

	pI2CHandle->RegAddrSent = 0;

	pI2CHandle->TxRxState = I2C_BUSY_RX;

	pI2CHandle->DevAddr = SlaveAddr;

	pI2CHandle->RegAddr = reg;

	pI2CHandle->pRxBuffer = pRxBuffer;

	pI2CHandle->RxLen = Len;

	/* Clear flags */
	pI2CHandle->pI2Cx->ICR |=
	I2C_ICR_STOPCF |
	I2C_ICR_NACKCF;

	/* WRITE REGISTER ADDRESS FIRST */

	pI2CHandle->pI2Cx->CR2 = 0;

	pI2CHandle->pI2Cx->CR2 |= (SlaveAddr << 1);

	pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_NBYTES_Pos);

	/* START */
	pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_START_Pos);

	/* Enable interrupts */
	pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_TXIE_Pos) | (1 << I2C_CR1_RXIE_Pos)
			| (1 << I2C_CR1_TCIE_Pos) | (1 << I2C_CR1_STOPIE_Pos)
			| (1 << I2C_CR1_NACKIE_Pos);

	return 1;
}

void I2C_IRQHandling(I2C_Handle_t *pI2CHandle) {
	uint32_t isr;

	isr = pI2CHandle->pI2Cx->ISR;

	/* ================= TXIS ================= */

	if (isr & I2C_ISR_TXIS) {
		if (pI2CHandle->RegAddrSent == 0) {
			pI2CHandle->pI2Cx->TXDR = pI2CHandle->RegAddr;

			pI2CHandle->RegAddrSent = 1;
		}
	}

	/* ================= TC ================= */

	if (isr & I2C_ISR_TC) {
		pI2CHandle->pI2Cx->CR2 = 0;

		pI2CHandle->pI2Cx->CR2 |= (pI2CHandle->DevAddr << 1);

		/* READ */
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_RD_WRN_Pos);

		/* NBYTES */
		pI2CHandle->pI2Cx->CR2 |= (pI2CHandle->RxLen <<
		I2C_CR2_NBYTES_Pos);

		/* AUTOEND */
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_AUTOEND_Pos);

		/* RESTART */
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_START_Pos);
	}

	/* ================= RXNE ================= */

	if (isr & I2C_ISR_RXNE) {
		*(pI2CHandle->pRxBuffer) = pI2CHandle->pI2Cx->RXDR;

		pI2CHandle->pRxBuffer++;

		pI2CHandle->RxLen--;
	}

	/* ================= STOPF ================= */

	if (isr & I2C_ISR_STOPF) {
		pI2CHandle->pI2Cx->ICR |=
		I2C_ICR_STOPCF;

		/* Disable interrupts */
		pI2CHandle->pI2Cx->CR1 &= ~((1 << I2C_CR1_TXIE_Pos)
				| (1 << I2C_CR1_RXIE_Pos) | (1 << I2C_CR1_TCIE_Pos)
				| (1 << I2C_CR1_STOPIE_Pos) | (1 << I2C_CR1_NACKIE_Pos));

		pI2CHandle->pI2Cx->CR2 = 0;

		pI2CHandle->TxRxState =
		I2C_READY;
	}

	/* ================= NACK ================= */

	if (isr & I2C_ISR_NACKF) {
		pI2CHandle->pI2Cx->ICR |=
		I2C_ICR_NACKCF;

		pI2CHandle->TxRxState =
		I2C_READY;
	}
}

void I2C_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDi) {
	/* ================= ENABLE / DISABLE ================= */

	if (EnorDi == ENABLE) {
		if (IRQNumber <= 31) {
			*NVIC_ISER0 |= (1 << IRQNumber);
		} else if (IRQNumber < 64) {
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));
		} else if (IRQNumber < 96) {
			*NVIC_ISER2 |= (1 << (IRQNumber % 64));
		}
	} else {
		if (IRQNumber <= 31) {
			*NVIC_ICER0 |= (1 << IRQNumber);
		} else if (IRQNumber < 64) {
			*NVIC_ICER1 |= (1 << (IRQNumber % 32));
		} else if (IRQNumber < 96) {
			*NVIC_ICER2 |= (1 << (IRQNumber % 64));
		}
	}

	/* ================= PRIORITY ================= */

	uint8_t iprx = IRQNumber / 4;

	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}

