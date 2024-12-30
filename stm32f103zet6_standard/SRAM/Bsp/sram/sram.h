#ifndef __SRAM_H
#define __SRAM_H

#include "stm32f10x.h"

#define SRAM_ADDR   0x68000000

void sram_Init(void);
void sram_Write(uint8_t *pbuf, uint32_t addr, uint32_t datalen);
void sram_Read(uint8_t *pbuf, uint32_t addr, uint32_t datalen);

#endif
