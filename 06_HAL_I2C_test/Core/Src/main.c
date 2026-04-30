#include "main.h"
#include <stdio.h>
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart2;

int _write(int file, char *ptr, int len) {
	HAL_UART_Transmit(&huart2, (uint8_t*) ptr, len, 100);
	return len;
}

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C2_Init(void);

#define OPT3001_CONFIG_REG  0x01
#define OPT3001_RESULT_REG  0x00

static void OPT3001_Write(uint8_t devAddr, uint8_t reg, uint16_t data) {
	uint8_t buf[3];

	buf[0] = reg;
	buf[1] = (data >> 8) & 0xFF;  // MSB
	buf[2] = data & 0xFF;         // LSB

	HAL_I2C_Master_Transmit(&hi2c2, devAddr << 1, buf, 3, HAL_MAX_DELAY);
}

static uint16_t OPT3001_ReadReg(uint8_t devAddr, uint8_t reg) {
	uint8_t data[2];

	HAL_I2C_Master_Transmit(&hi2c2, devAddr << 1, &reg, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c2, devAddr << 1, data, 2, HAL_MAX_DELAY);

	return ((uint16_t) data[0] << 8) | data[1];
}

static void opt3001_init(void) {
	OPT3001_Write(0x47, OPT3001_CONFIG_REG, 0xC410);
}

static float opt3001_read_lux(void) {
	uint16_t raw = OPT3001_ReadReg(0x47, OPT3001_RESULT_REG);

	uint16_t exp = (raw >> 12) & 0x0F;
	uint16_t mant = raw & 0x0FFF;

	return (0.01f * (1 << exp) * mant);
}

void I2C2_Scan(void) {
	printf("Scanning I2C2 bus...\r\n");

	for (uint8_t addr = 1; addr < 127; addr++) {
		if (HAL_I2C_IsDeviceReady(&hi2c2, addr << 1, 2, 10) == HAL_OK) {
			printf("Found device at 0x%02X\r\n", addr);
		}
	}

	printf("Scan Complete.\r\n\r\n");
}



void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		Error_Handler();
	}

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler();
	}
}

static void MX_I2C2_Init(void) {

	hi2c2.Instance = I2C2;
	hi2c2.Init.Timing = 0x10D19CE4;
	hi2c2.Init.OwnAddress1 = 0;
	hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c2.Init.OwnAddress2 = 0;
	hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c2) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE)
			!= HAL_OK) {
		Error_Handler();
	}

	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK) {
		Error_Handler();
	}

}

static void MX_USART2_UART_Init(void) {

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}

}

static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

void Error_Handler(void) {

	__disable_irq();
	while (1) {
	}
}

int lux;

int main(void) {

	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_I2C2_Init();

	printf("System Start...\n");

	I2C2_Scan();

	opt3001_init();
	printf("OPT3001 Initialized\r\n");

	while (1) {
		 lux = opt3001_read_lux();
		printf("Light = %d lux\r\n", lux);

		HAL_Delay(500);
	}

}
