#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"
#include "Serial.h"

void delay_init(void);
void delay_us(u32 nus);
void delay_ms(u16 nms);

#endif
