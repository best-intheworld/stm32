#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x.h"
#include "Delay.h"
#include "lcd.h"

#define I2C_SCL     GPIO_Pin_6
#define I2C_SDA     GPIO_Pin_7

void IIC_Init(void);
void SCL(uint8_t state);
void SDA(uint8_t state);
void I2C_delay(void);
void I2C_start(void);
void I2C_end(void);
void I2C_sendByte(uint8_t data);
uint8_t I2C_receiveByte(uint8_t ack);
uint8_t I2C_receiveACK(void);
void I2C_sendACK(void);
void I2C_sendNACK(void);

#endif
