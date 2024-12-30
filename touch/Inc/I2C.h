#ifndef __I2C_H
#define __I2C_H

#include "main.h"
#include "delay.h"

#define I2C_SCL     GPIO_PIN_6
#define I2C_SDA     GPIO_PIN_7


void I2C_Init(void);
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
