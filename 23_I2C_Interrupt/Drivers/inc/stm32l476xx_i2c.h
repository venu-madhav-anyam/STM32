
#ifndef INC_STM32L476XX_I2C_DRIVERS_H_
#define INC_STM32L476XX_I2C_DRIVERS_H_


#include "stm32l476xx.h"

typedef struct {
	uint32_t SCLSpeed;
	uint8_t DeviceAddress;
	uint8_t ACKControl;
	uint16_t FM_DutyCycle;

} I2C_Config_t;

typedef struct {
	I2C_TypeDef *pI2Cx;
	I2C_Config_t I2C_Config;
	uint8_t      *pTxBuffer;   // To store app Tx buffer address
    uint8_t      *pRxBuffer;   // To store app Rx buffer address

    uint32_t     TxLen;        // To store Tx len
	uint32_t     RxLen;        // To store Rx len

    uint8_t      TxRxState;    // To store communication state
    uint8_t      DevAddr;      // To store slave/device address

	uint32_t     RxSize;       // To store Rx size
    uint8_t      Sr;

} I2C_Handle_t;

#define I2C_ISR_TXIS      (1 << 1)
#define I2C_ISR_RXNE      (1 << 2)
#define I2C_ISR_ADDR      (1 << 3)
#define I2C_ISR_NACKF     (1 << 4)
#define I2C_ISR_STOPF     (1 << 5)
#define I2C_ISR_TC        (1 << 6)
#define I2C_ISR_BUSY      (1 << 15)

//SPEED MODES
#define I2C_SCL_SPEED_SM            100000U      /* Standard mode 100KHz */
#define I2C_SCL_SPEED_FM4K          400000U      /* Fast mode 400KHz */

//ACK CONTROL
#define I2C_ACK_DISABLE             0
#define I2C_ACK_ENABLE              1

//FM_DUTY
#define I2C_FM_DUTY_2				0
#define I2C_FM_DUTY_16_9			1

#define I2C_READY      0
#define I2C_BUSY_IN_RX 1
#define I2C_BUSY_IN_TX 2

#define I2C_ENABLE_SR   ENABLE
#define I2C_DISABLE_SR  DISABLE

#define NO_PR_BITS_IMPLEMENTED 4

//FUNCTIONS

void I2C_PeriClockControl(I2C_TypeDef *pI2Cx, uint8_t EnOrDi);

void I2C_Init(I2C_Handle_t *pI2CHandle);

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer,
		uint32_t Len, uint8_t SlaveAddr);

void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer,
		uint32_t Len, uint8_t SlaveAddr);

void I2C_ScanBus(I2C_Handle_t *pI2CHandle);

void delay_ms(uint32_t time);

void I2C_MasterWriteRead(I2C_Handle_t *pI2CHandle, uint8_t SlaveAddr,
		uint8_t *pTxBuffer, uint32_t TxLen, uint8_t *pRxBuffer, uint32_t RxLen);

void I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle,
                          uint8_t SlaveAddr,
                          uint8_t *pTxBuffer,
                          uint32_t Len);

uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer, uint32_t Len);

/* IRQ Configuration and ISR handling */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi);

void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);

#endif /* INC_STM32L476XX_I2C_DRIVERS_H_ */
