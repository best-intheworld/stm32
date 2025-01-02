#ifndef __AT24C02_H
#define __AT24C02_H

#include "stm32f10x.h"
#include "I2C.h"
#include "Delay.h"
#include "Serial.h"

#define AT24C02     255

void AT24C02_Init(void);
uint8_t AT24C02_receiveByte(uint8_t addr);
void AT24C02_writeByte(uint8_t addr, uint8_t data);
uint8_t AT24C02_check(void);
void AT24C02_read(uint16_t addr, uint8_t *pbuf, uint16_t len);
void AT24C02_write(uint16_t addr, uint8_t *pbuf, uint16_t len);

#endif
