#include "stm32l476xx.h"
#include "stm32l476xx_i2c.h"
#include "stm32l476xx_gpio.h"



#define OPT3001_ADDR               0x47

/* ================= REGISTER MAP ================= */

#define OPT3001_REG_RESULT         0x00
#define OPT3001_REG_CONFIG         0x01
#define OPT3001_REG_LOWLIMIT       0x02
#define OPT3001_REG_HIGHLIMIT      0x03
#define OPT3001_REG_MANFID         0x7E
#define OPT3001_REG_DEVICEID       0x7F

I2C_Handle_t I2C2Handle;
volatile uint8_t config_done = 0;
volatile uint8_t transfer_started = 0;
int lux;

void I2C2_GPIOInits(void)
{

		GPIO_Handle_t I2CPins;

		I2CPins.pGPIOx = GPIOB;

		I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
		I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
		I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
		I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
		I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

		/* PB10 -> SCL */
		I2CPins.GPIO_PinConfig.GPIO_PinNumber = 10;
		GPIO_Init(&I2CPins);

		/* PB11 -> SDA */
		I2CPins.GPIO_PinConfig.GPIO_PinNumber = 11;
		GPIO_Init(&I2CPins);


}

//WRITE REGISTER
void OPT3001_WriteReg(I2C_Handle_t *pI2CHandle, uint8_t reg, uint16_t value) {
	uint8_t data[3];

	data[0] = reg;

	data[1] = (value >> 8) & 0xFF;
	data[2] = value & 0xFF;

	I2C_MasterSendData(pI2CHandle, data, 3,
	OPT3001_ADDR);
}

//READ REGISTER
uint16_t OPT3001_ReadReg(I2C_Handle_t *pI2CHandle, uint8_t reg) {
	uint8_t rx[2];

	/* Clear flags */
	pI2CHandle->pI2Cx->ICR |=
	I2C_ICR_STOPCF |
	I2C_ICR_NACKCF;

	/* ================= WRITE REGISTER ================= */

	pI2CHandle->pI2Cx->CR2 = 0;

	pI2CHandle->pI2Cx->CR2 |= (OPT3001_ADDR << 1);

	pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_NBYTES_Pos);

	pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_START_Pos);

	while (!(pI2CHandle->pI2Cx->ISR &
	I2C_ISR_TXIS))
		;

	pI2CHandle->pI2Cx->TXDR = reg;

	while (!(pI2CHandle->pI2Cx->ISR &
	I2C_ISR_TC))
		;

	/* ================= REPEATED START READ ================= */

	pI2CHandle->pI2Cx->CR2 = 0;

	pI2CHandle->pI2Cx->CR2 |= (OPT3001_ADDR << 1);

	pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_RD_WRN_Pos);

	pI2CHandle->pI2Cx->CR2 |= (2 << I2C_CR2_NBYTES_Pos);

	pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_AUTOEND_Pos);

	pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_START_Pos);

	/* Read MSB */
	while (!(pI2CHandle->pI2Cx->ISR &
	I2C_ISR_RXNE))
		;

	rx[0] = pI2CHandle->pI2Cx->RXDR;

	/* Read LSB */
	while (!(pI2CHandle->pI2Cx->ISR &
	I2C_ISR_RXNE))
		;

	rx[1] = pI2CHandle->pI2Cx->RXDR;

	/* Wait STOP */
	while (!(pI2CHandle->pI2Cx->ISR &
	I2C_ISR_STOPF))
		;

	/* Clear STOP */
	pI2CHandle->pI2Cx->ICR |=
	I2C_ICR_STOPCF;

	return ((rx[0] << 8) | rx[1]);
}

//SENSOR INIT
void OPT3001_Init(I2C_Handle_t *pI2CHandle) {

	OPT3001_WriteReg(pI2CHandle,
	OPT3001_REG_CONFIG, 0xC410);
}

//READ LUX
int OPT3001_ReadLux(I2C_Handle_t *pI2CHandle) {
	uint16_t raw = 0x0000;
	uint16_t exponent;
	uint16_t mantissa;

	raw = OPT3001_ReadReg(pI2CHandle,
	OPT3001_REG_RESULT);

	exponent = (raw >> 12) & 0x0F;

	mantissa = raw & 0x0FFF;
	int l = mantissa * (0.01 * (1UL << exponent));

	return (int) l;
}

void I2C2_Init(void)
{
	I2C2Handle.pI2Cx = I2C2;

		I2C2Handle.I2C_Config.SCLSpeed = 100000;
		I2C2Handle.I2C_Config.DeviceAddress = 0x61;
		I2C2Handle.I2C_Config.ACKControl = ENABLE;

		I2C_Init(&I2C2Handle);
}
/*
void I2C2_EV_IRQHandler(void)
{
    I2C_EV_IRQHandling(&I2C2Handle);
}*/

/*
void I2C2_ER_IRQHandler(void)
{
    I2C_ER_IRQHandling(&I2C2Handle);
}
*/
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
    uint32_t isr;

    isr = pI2CHandle->pI2Cx->ISR;

    /* ================= TXIS ================= */

    if(isr & I2C_ISR_TXIS)
    {
        if(pI2CHandle->TxLen > 0)
        {
            pI2CHandle->pI2Cx->TXDR =
                    *(pI2CHandle->pTxBuffer);

            pI2CHandle->pTxBuffer++;

            pI2CHandle->TxLen--;
        }
    }

    /* ================= TC ================= */

    if(isr & I2C_ISR_TC)
    {
        /* Transfer complete */
    }

    /* ================= STOP ================= */

    if(isr & I2C_ISR_STOPF)
    {
        /* Clear STOP flag */
        pI2CHandle->pI2Cx->ICR |=
                I2C_ICR_STOPCF;

        /* Disable interrupts */
        pI2CHandle->pI2Cx->CR1 &=
                ~(1 << I2C_CR1_TXIE_Pos);

        pI2CHandle->pI2Cx->CR1 &=
                ~(1 << I2C_CR1_TCIE_Pos);

        pI2CHandle->pI2Cx->CR1 &=
                ~(1 << I2C_CR1_STOPIE_Pos);

        /* READY state */
        pI2CHandle->TxRxState =
                I2C_READY;

        /* Notify main */
        config_done = 1;
    }
}

void I2C2_EV_IRQHandler(void)
{
    lux = 99;
    I2C_EV_IRQHandling(&I2C2Handle);
}
/*****************
void  I2C_ER_IRQHandling(void)
{
//    NACK
//    if(I2C2->ISR & (1 << I2C_ISR_NACKF_Pos))
//    {
//        I2C2->ICR |= I2C_ICR_NACKCF;
//    }
//
//     Bus Error
//    if(I2C2->ISR & (1 << 8))
//    {
//        I2C2->ICR |= I2C_ICR_BERRCF;
//    }
//
//     Arbitration Lost
//    if(I2C2->ISR & (1 << 9))
//    {
//        I2C2->ICR |= I2C_ICR_ARLOCF;
//    }
//}
*************/


uint8_t txBuffer[3];
int main(void)
{
	I2C2_GPIOInits();

    I2C2_Init();

    I2C2Handle.TxRxState = I2C_READY;


    /* Enable NVIC Interrupts */
    I2C_IRQInterruptConfig(33, ENABLE);
 //   I2C_IRQInterruptConfig(34, ENABLE);

    /* Priority */
    I2C_IRQPriorityConfig(33, 1);
  //  I2C_IRQPriorityConfig(34, 0);

    /* Prepare OPT3001 config write */
    txBuffer[0] = OPT3001_REG_CONFIG;
    txBuffer[1] = 0xC4;
    txBuffer[2] = 0x10;
    //OPT3001_Init(&I2C2Handle);


    	while(1)
    	{
    	    /* Start interrupt transfer ONLY ONCE */
    	    if((transfer_started == 0) &&
    	       (I2C2Handle.TxRxState == I2C_READY))
    	    {
    	        I2C_MasterSendDataIT(&I2C2Handle,
    	                             OPT3001_ADDR,
    	                             txBuffer,
    	                             3);

    	        transfer_started = 1;
    	    }

    	    /* ISR completed transfer */
    	    if(config_done)
    	    {
    	        lux = OPT3001_ReadLux(&I2C2Handle);

    	        config_done = 0;

    	        transfer_started = 0;

    	        delay_ms(500);
    	    }
    	}

}

