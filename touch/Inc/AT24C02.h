#ifndef __AT24C02_H
#define __AT24C02_H

#include "main.h"
#include "I2C.h"
#include "delay.h"

#define AT24C02     255

void AT24C02_Init(void);
uint8_t AT24C02_receiveByte(uint8_t addr);
void AT24C02_writeByte(uint8_t addr, uint8_t data);
uint8_t AT24C02_check(void);
void AT24C02_read(uint16_t addr, uint8_t *pbuf, uint16_t datalength);
void AT24C02_write(uint16_t addr, uint8_t *pbuf, uint16_t datalength);

#endif
