#include <stdint.h>
#include <stm32l4xx_hal.h>
#include <string.h>

const uint32_t MSIRangeTable[] = { 100000, 200000, 400000, 800000, 1000000,
		2000000, 4000000, 8000000, 16000000, 24000000, 32000000, 48000000 };


const uint8_t AHBPrescTable[16] = {
    0, 0, 0, 0, 1, 2, 3, 4,
    6, 7, 8, 9, 0, 0, 0, 0
};

/* -------- HANDLES -------- */
DAC_HandleTypeDef hdac1;

/* -------- SYSTICK -------- */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/* -------- CLOCK -------- */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    osc.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    osc.MSIState = RCC_MSI_ON;
    osc.MSIClockRange = RCC_MSIRANGE_8; // 16 MHz
    osc.PLL.PLLState = RCC_PLL_NONE;

    HAL_RCC_OscConfig(&osc);

    clk.ClockType = RCC_CLOCKTYPE_SYSCLK |
                    RCC_CLOCKTYPE_HCLK |
                    RCC_CLOCKTYPE_PCLK1 |
                    RCC_CLOCKTYPE_PCLK2;

    clk.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV1;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;

    HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_0);
}

/* -------- DAC INIT -------- */
void DAC1_Init(void)
{
    __HAL_RCC_DAC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // PA4 → DAC OUT
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    hdac1.Instance = DAC1;
    HAL_DAC_Init(&hdac1);

    DAC_ChannelConfTypeDef sConfig = {0};

    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

    HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1);

    HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
}

/* -------- MAIN -------- */
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    DAC1_Init();

    /* -------- TEST 1: CONSTANT VOLTAGE -------- */
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2048);

    while (1)
    {
        /* -------- TEST 2: RAMP WAVE (OPTIONAL) -------- */
        for (uint16_t i = 0; i < 4095; i++)
        {
            HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, i);
            HAL_Delay(10);
        }
    }
}
