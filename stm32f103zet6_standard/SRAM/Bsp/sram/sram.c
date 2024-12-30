#include "sram.h"

void sram_Init(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_10;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    FSMC_NORSRAMInitTypeDef FSMC_NORSRAM_InitStructure;
    FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAM_writeTimingInitStructure;
    FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAM_readWriteTimingInitStructure;

    FSMC_NORSRAM_writeTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;
    FSMC_NORSRAM_writeTimingInitStructure.FSMC_AddressHoldTime = 1;
    FSMC_NORSRAM_writeTimingInitStructure.FSMC_AddressSetupTime = 0;
    FSMC_NORSRAM_writeTimingInitStructure.FSMC_BusTurnAroundDuration = 0;
    FSMC_NORSRAM_writeTimingInitStructure.FSMC_CLKDivision = 0;
    FSMC_NORSRAM_writeTimingInitStructure.FSMC_DataLatency = 0;
    FSMC_NORSRAM_writeTimingInitStructure.FSMC_DataSetupTime = 1;

    FSMC_NORSRAM_readWriteTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;
    FSMC_NORSRAM_readWriteTimingInitStructure.FSMC_AddressHoldTime = 1;
    FSMC_NORSRAM_readWriteTimingInitStructure.FSMC_AddressSetupTime = 0;
    FSMC_NORSRAM_readWriteTimingInitStructure.FSMC_BusTurnAroundDuration = 0;
    FSMC_NORSRAM_readWriteTimingInitStructure.FSMC_CLKDivision = 0;
    FSMC_NORSRAM_readWriteTimingInitStructure.FSMC_DataLatency = 0;
    FSMC_NORSRAM_readWriteTimingInitStructure.FSMC_DataSetupTime = 1;

    FSMC_NORSRAM_InitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAM_InitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
    FSMC_NORSRAM_InitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAM_InitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAM_InitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
    FSMC_NORSRAM_InitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAM_InitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAM_InitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAM_InitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAM_InitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAM_InitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAM_InitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAM_InitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAM_writeTimingInitStructure;
    FSMC_NORSRAM_InitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAM_readWriteTimingInitStructure;
    FSMC_NORSRAMInit(&FSMC_NORSRAM_InitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);
}

void sram_Write(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (int i = 0; i < datalen; i++)
    {
        *(volatile uint8_t *)(SRAM_ADDR + addr) = *pbuf;
        pbuf++;
        addr++;
    }
}

void sram_Read(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (int i = 0; i < datalen; i++)
    {
        *pbuf = *(volatile uint8_t *)(SRAM_ADDR + addr);
        addr++;
        pbuf++;
    }
}
