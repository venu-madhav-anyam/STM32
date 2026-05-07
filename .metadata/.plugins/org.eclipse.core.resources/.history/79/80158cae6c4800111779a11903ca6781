/*
 * w25qxx.h
 *
 *  Created on: May 5, 2026
 *      Author: Venu Madhav Anyam
 */

#ifndef INC_W25QXX_H_
#define INC_W25QXX_H_

#include "stm32l476xx_spi.h"
#include "stm32l476xx_gpio.h"

/* -------- COMMANDS -------- */
#define W25Q_CMD_WRITE_ENABLE     0x06
#define W25Q_CMD_READ_STATUS      0x05
#define W25Q_CMD_PAGE_PROGRAM     0x02
#define W25Q_CMD_READ_DATA        0x03
#define W25Q_CMD_SECTOR_ERASE     0x20
#define W25Q_CMD_READ_ID          0x9F

/* -------- API -------- */
void W25Q_Init(void);
void W25Q_WriteEnable(void);
uint8_t W25Q_ReadStatus(void);
void W25Q_WaitBusy(void);

void W25Q_ReadID(uint8_t *id);
void W25Q_Read(uint32_t addr, uint8_t *data, uint16_t len);
void W25Q_Write(uint32_t addr, uint8_t *data, uint16_t len);
void W25Q_EraseSector(uint32_t addr);

#endif
