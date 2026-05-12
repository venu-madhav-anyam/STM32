/*
 * stm32l476xx_spi.c
 *
 *  Created on: May 5, 2026
 *      Author: Venu Madhav Anyam
 */

#include "stm32l476xx_spi.h"

static void SPI_TXE_InterruptHandle(SPI_Handle_t *pHandle);
static void SPI_RXNE_InterruptHandle(SPI_Handle_t *pHandle);

/* ================= CLOCK ================= */

void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi) {
	if (EnOrDi == ENABLE) {
		if (pSPIx == SPI1)
			SPI1_CLK_ENABLE();
		else if (pSPIx == SPI2)
			SPI2_CLK_ENABLE();
		else if (pSPIx == SPI3)
			SPI3_CLK_ENABLE();
	} else {
		if (pSPIx == SPI1)
			SPI1_CLK_DISABLE();
		else if (pSPIx == SPI2)
			SPI2_CLK_DISABLE();
		else if (pSPIx == SPI3)
			SPI3_CLK_DISABLE();
	}
}

/* ================= INIT ================= */

void SPI_Init(SPI_Handle_t *pSPIHandle) {
	uint32_t temp = 0;

	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	/* -------- Device Mode -------- */
	temp |= (pSPIHandle->SPI_Config.DeviceMode << SPI_CR1_MSTR);

	/* -------- Bus Config -------- */
	if (pSPIHandle->SPI_Config.BusConfig == SPI_BUS_CONFIG_FD) {
		temp &= ~(1 << SPI_CR1_BIDIMODE);   // full
	} else if (pSPIHandle->SPI_Config.BusConfig == SPI_BUS_CONFIG_HD) {
		temp |= (1 << SPI_CR1_BIDIMODE);    // half
	} else if (pSPIHandle->SPI_Config.BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY) {
		temp &= ~(1 << SPI_CR1_BIDIMODE);   // 2-line
		temp |= (1 << SPI_CR1_RXONLY);     // RX only
	}

	/* -------- Baud Rate -------- */
	temp |= (pSPIHandle->SPI_Config.SclkSpeed << SPI_CR1_BR);

	/* -------- CPOL & CPHA -------- */
	temp |= (pSPIHandle->SPI_Config.CPOL << SPI_CR1_CPOL);
	temp |= (pSPIHandle->SPI_Config.CPHA << SPI_CR1_CPHA);

	/* -------- DFF -------- */
	/* -------- Data size (CR2 DS bits) -------- */
	pSPIHandle->pSPIx->CR2 &= ~(0xF << SPI_CR2_DS);

	if (pSPIHandle->SPI_Config.DFF == SPI_DFF_8BITS)
		pSPIHandle->pSPIx->CR2 |= (7 << SPI_CR2_DS);   // 8-bit
	else
		pSPIHandle->pSPIx->CR2 |= (15 << SPI_CR2_DS);  // 16-bit

	/* -------- SSM -------- */
	temp |= (pSPIHandle->SPI_Config.SSM << SPI_CR1_SSM);

	pSPIHandle->pSPIx->CR1 = temp;

	/* -------- SSI (must for master + SSM) -------- */
	if (pSPIHandle->SPI_Config.SSM == SPI_SSM_EN) {
		pSPIHandle->pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	}

	/* -------- Enable SPI -------- */
	pSPIHandle->pSPIx->CR1 |= (1 << SPI_CR1_SPE);
}

/* ================= SEND ================= */

void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len) {
	while (Len > 0) {
		/* Wait until TXE = 1 */
		while (!(pSPIx->SR & SPI_TXE_FLAG))
			;

		/* Check DFF */
		if (pSPIx->CR1 & (1 << SPI_CR1_CRCL))   // 16-bit mode
				{
			/* Load 16-bit data */
			pSPIx->DR = *((uint16_t*) pTxBuffer);

			Len -= 2;
			pTxBuffer += 2;
		} else   // 8-bit mode
		{
			pSPIx->DR = *pTxBuffer;

			Len--;
			pTxBuffer++;
		}
	}

	/* Wait until SPI not busy */
	while (pSPIx->SR & SPI_BUSY_FLAG)
		;
}

/* ================= RECEIVE ================= */

void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len) {
	while (Len > 0) {
		/* Wait until TXE = 1 */
		while (!(pSPIx->SR & SPI_TXE_FLAG))
			;

		/* Check DFF */
		if (pSPIx->CR1 & (1 << SPI_CR1_CRCL))   // 16-bit mode
				{
			pSPIx->DR = 0xFFFF;   // send dummy 16-bit

			while (!(pSPIx->SR & SPI_RXNE_FLAG))
				;

			*((uint16_t*) pRxBuffer) = pSPIx->DR;

			pRxBuffer += 2;
			Len -= 2;
		} else   // 8-bit mode
		{
			*((volatile uint8_t*) &pSPIx->DR) = 0xFF;   // dummy byte

			while (!(pSPIx->SR & SPI_RXNE_FLAG))
				;

			*pRxBuffer = pSPIx->DR;

			pRxBuffer++;
			Len--;
		}
	}

	/* Wait until SPI not busy */
	while (pSPIx->SR & SPI_BUSY_FLAG)
		;
}

uint8_t SPI_Transfer(SPI_RegDef_t *pSPIx, uint8_t data) {
	while (!(pSPIx->SR & SPI_TXE_FLAG))
		;
	pSPIx->DR = data;

	while (!(pSPIx->SR & SPI_RXNE_FLAG))
		;
	return pSPIx->DR;
}

//IRQ Configuration and ISR handling
void SPI_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t En_or_Dis) {
	if (En_or_Dis) {
		if (IRQNumber <= 31)
			NVIC_ISER0 |= (1 << IRQNumber);
		else if (IRQNumber < 64)
			NVIC_ISER1 |= (1 << (IRQNumber % 32));
		else if (IRQNumber < 96)
			NVIC_ISER2 |= (1 << (IRQNumber % 32));
	} else {
		if (IRQNumber <= 31)
			NVIC_ICER0 |= (1 << IRQNumber);
		else if (IRQNumber < 64)
			NVIC_ICER1 |= (1 << (IRQNumber % 32));
		else if (IRQNumber < 96)
			NVIC_ICER2 |= (1 << (IRQNumber % 32));
	}

	/* Priority */
	uint8_t iprx = IRQNumber / 4;
	uint8_t section = IRQNumber % 4;

	NVIC_IPR[iprx] &= ~(0xFF << (8 * section));
	NVIC_IPR[iprx] |= (IRQPriority << (8 * section));
}

void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority) {
	uint8_t iprx = IRQNumber / 4;
	uint8_t section = IRQNumber % 4;

	uint8_t shift = (8 * section) + 4;

	NVIC_IPR[iprx] &= ~(0xF << shift);
	NVIC_IPR[iprx] |= (IRQPriority << shift);
}

uint8_t SPI_SendData_IT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer,
		uint32_t Len) {

	uint8_t state = pSPIHandle->TxState;
	/* Check if SPI is busy */
	if (state != SPI_BUSY_IN_TX) {
		/* Save buffer info */
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;
		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		/* Enable TXE interrupt */
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);
	}

	return state;
}

uint8_t SPI_ReceiveData_IT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer,
		uint32_t Len) {
	uint8_t state = pSPIHandle->RxState;

	if (state != SPI_BUSY_IN_RX) {
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = Len;
		pSPIHandle->RxState = SPI_BUSY_IN_RX;

		/* Enable RXNE interrupt */
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
	}
	return state;
}

void SPI_IRQHandling(SPI_Handle_t *pHandle) {
	uint8_t temp1, temp2;

	/* TXE */
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_TXE);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

	if (temp1 && temp2) {
		SPI_TXE_InterruptHandle(pHandle);
	}

	/* RXNE */
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);

	if (temp1 && temp2) {
		SPI_RXNE_InterruptHandle(pHandle);
	}

	if (pHandle->pSPIx->SR & (1 << SPI_SR_OVR)) {
		volatile uint8_t temp;
		temp = pHandle->pSPIx->DR;
		temp = pHandle->pSPIx->SR;
		(void) temp;
	}
}
static void SPI_TXE_InterruptHandle(SPI_Handle_t *pHandle) {
	if (pHandle->TxLen > 0) {
		pHandle->pSPIx->DR = *(pHandle->pTxBuffer);
		pHandle->pTxBuffer++;
		pHandle->TxLen--;
	}

	if (pHandle->TxLen == 0) {
		pHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
	}
}

static void SPI_RXNE_InterruptHandle(SPI_Handle_t *pHandle) {
	if (pHandle->RxLen > 0) {
		*(pHandle->pRxBuffer) = pHandle->pSPIx->DR;
		pHandle->pRxBuffer++;
		pHandle->RxLen--;
	}

	if (pHandle->RxLen == 0) {
		/* Disable RX interrupt */
		pHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);

		/* Wait until SPI not busy */
		while (pHandle->pSPIx->SR & SPI_BUSY_FLAG)
			;

		pHandle->TxRxState = SPI_READY;
	}
}

void SPI_TransmitReceive_IT(SPI_Handle_t *pHandle, uint8_t *pTxBuffer,
		uint8_t *pRxBuffer, uint32_t Len) {
	if (pHandle->TxRxState != SPI_BUSY_IN_TXRX) {
		pHandle->pTxBuffer = pTxBuffer;
		pHandle->pRxBuffer = pRxBuffer;
		pHandle->TxLen = Len;
		pHandle->RxLen = Len;

		pHandle->TxRxState = SPI_BUSY_IN_TXRX;

		/* Enable BOTH interrupts */
		pHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);
		pHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
	}
}
