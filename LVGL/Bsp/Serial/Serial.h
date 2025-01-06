#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>
#include "string.h"

#define USART1_PORT		GPIOA
#define USART1_TX			GPIO_Pin_9
#define USART1_RX			GPIO_Pin_10

extern char USART1_ReceiveBuf[1024];

void Serial_Init(void);
void USART1_sendByte(uint8_t byte);
void USART1_sendArray(uint8_t* array, uint16_t len);
void USART1_sendString(char *str);
void USART1_sendNum(uint32_t num);
void USART1_Printf(char *format, ...);
void TRACE(char *str);

#endif
