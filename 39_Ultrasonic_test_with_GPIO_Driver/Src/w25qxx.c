/*
 * w25qxx.c
 *
 *  Created on: May 5, 2026
 *      Author: Venu Madhav Anyam
 */

#include "w25qxx.h"

/* ================= CS ================= */

void W25Q_CS_LOW(void) {
	GPIO_WriteToOutputPin(W25Q_CS_PORT, W25Q_CS_PIN, 0);
}

void W25Q_CS_HIGH(void) {
	GPIO_WriteToOutputPin(W25Q_CS_PORT, W25Q_CS_PIN, 1);
}

/* ================= INIT ================= */

void W25Q_Init(void) {
	W25Q_CS_HIGH();
}

/* ================= WRITE ENABLE ================= */

void W25Q_WriteEnable(void) {
	uint8_t cmd = W25Q_CMD_WRITE_ENABLE;

	W25Q_CS_LOW();

	SPI_SendData(SPI1, &cmd, 1);

	W25Q_CS_HIGH();
}

/* ================= READ STATUS ================= */

uint8_t W25Q_ReadStatus1(void) {
	uint8_t cmd = W25Q_CMD_READ_STATUS1;
	uint8_t status;

	W25Q_CS_LOW();

	SPI_SendData(SPI1, &cmd, 1);

	SPI_ReceiveData(SPI1, &status, 1);

	W25Q_CS_HIGH();

	return status;
}

/* ================= WAIT BUSY ================= */

void W25Q_WaitBusy(void) {
	while (W25Q_ReadStatus1() & 0x01)
		;
}

/* ================= READ ID ================= */

void W25Q_ReadID(uint8_t *id) {
	uint8_t cmd = W25Q_CMD_JEDEC_ID;

	W25Q_CS_LOW();

	SPI_SendData(SPI1, &cmd, 1);

	SPI_ReceiveData(SPI1, id, 3);

	W25Q_CS_HIGH();
}

/* ================= READ DATA ================= */

void W25Q_Read(uint32_t addr, uint8_t *buf, uint32_t len) {
	uint8_t cmd[4];

	cmd[0] = W25Q_CMD_READ_DATA;
	cmd[1] = (addr >> 16) & 0xFF;
	cmd[2] = (addr >> 8) & 0xFF;
	cmd[3] = addr & 0xFF;

	W25Q_CS_LOW();

	SPI_SendData(SPI1, cmd, 4);

	SPI_ReceiveData(SPI1, buf, len);

	W25Q_CS_HIGH();
}

/* ================= PAGE WRITE ================= */

void W25Q_WritePage(uint32_t addr, uint8_t *buf, uint16_t len) {
	W25Q_WriteEnable();

	uint8_t cmd[4];

	cmd[0] = W25Q_CMD_PAGE_PROGRAM;
	cmd[1] = (addr >> 16) & 0xFF;
	cmd[2] = (addr >> 8) & 0xFF;
	cmd[3] = addr & 0xFF;

	W25Q_CS_LOW();

	SPI_SendData(SPI1, cmd, 4);

	SPI_SendData(SPI1, buf, len);

	W25Q_CS_HIGH();

	W25Q_WaitBusy();
}

/* ================= MULTI PAGE WRITE ================= */

void W25Q_Write(uint32_t addr, uint8_t *buf, uint32_t len) {
	while (len > 0) {
		uint16_t chunk;

		chunk = 256 - (addr % 256);

		if (chunk > len)
			chunk = len;

		W25Q_WritePage(addr, buf, chunk);

		addr += chunk;
		buf += chunk;
		len -= chunk;
	}
}

/* ================= ERASE SECTOR ================= */

void W25Q_EraseSector(uint32_t addr) {
	W25Q_WriteEnable();

	uint8_t cmd[4];

	cmd[0] = W25Q_CMD_SECTOR_ERASE;
	cmd[1] = (addr >> 16) & 0xFF;
	cmd[2] = (addr >> 8) & 0xFF;
	cmd[3] = addr & 0xFF;

	W25Q_CS_LOW();

	SPI_SendData(SPI1, cmd, 4);

	W25Q_CS_HIGH();

	W25Q_WaitBusy();
}
