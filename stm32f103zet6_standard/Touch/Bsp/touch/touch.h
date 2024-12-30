#ifndef __TOUCH_H
#define __TOUCH_H

#include "stm32f10x.h"

#define T_SCK_Port  GPIOB
#define T_SCK_Pin   GPIO_Pin_1

#define T_MISO_Port  GPIOB
#define T_MISO_Pin   GPIO_Pin_2

#define T_MOSI_Port  GPIOF
#define T_MOSI_Pin   GPIO_Pin_9

#define T_PEN_Port  GPIOF
#define T_PEN_Pin   GPIO_Pin_10

#define T_CS_Port  GPIOF
#define T_CS_Pin   GPIO_Pin_11

void touch_init(void);

#endif
