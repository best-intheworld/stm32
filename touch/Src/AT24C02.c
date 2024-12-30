#include "AT24C02.h"

/**
 * @brief       AT24C02初始化函数
 * @param       无
 * @retval      无
 */
void AT24C02_Init(void)
{
    I2C_Init();
}

/**
 * @brief       AT24C02接收一字节函数
 * @param       addr：指定地址
 * @retval      接收到的一字节数据
 */
uint8_t AT24C02_receiveByte(uint8_t addr)
{
    uint8_t temp;
    uint8_t data;
    I2C_start();
    I2C_sendByte(0xA0);
    temp = I2C_receiveACK();
    if(temp == 0)
    {
        I2C_sendByte(addr);
        temp = I2C_receiveACK();
        if(temp == 0)
        {
            I2C_start();
            I2C_sendByte(0xA1);
            temp = I2C_receiveACK();
            if(temp == 0)
            {
                data = I2C_receiveByte(1);
                I2C_end();
            }
        }
    }
    return data;
}

/**
 * @brief       AT24C02发送一字节函数
 * @param       addr：指定地址
 * @param       data:要发送的数据
 * @retval      无
 */
void AT24C02_writeByte(uint8_t addr, uint8_t data)
{
    uint8_t temp;
    I2C_start();
    I2C_sendByte(0xA0);
    temp = I2C_receiveACK();
    if(temp == 0)
    {
        I2C_sendByte(addr);
        temp = I2C_receiveACK();
        if(temp == 0)
        {
            I2C_sendByte(data);
            temp = I2C_receiveACK();
            if(temp == 0)
            {
                I2C_end();
                delay_ms(10);
            }
        }
    }
}

/**
 * @brief       检查AT24C02是否工作正常
 * @param       无
 * @retval      0：正常；1：不正常
 */
uint8_t AT24C02_check(void)
{
    uint8_t temp;
    temp = AT24C02_receiveByte(AT24C02);
    if(temp == 0x55)
    {
        return 0;
    }
    else
    {
        AT24C02_writeByte(AT24C02,0x55);
        delay_ms(10);
        temp = AT24C02_receiveByte(AT24C02);
        if(temp == 0x55)
        {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief       在AT24CXX里面的指定地址开始读出指定个数的数据
 * @param       addr    : 开始读出的地址 对24c02为0~255
 * @param       pbuf    : 数据数组首地址
 * @param       datalength : 要读出数据的个数
 * @retval      无
 */
void AT24C02_read(uint16_t addr, uint8_t *pbuf, uint16_t datalength)
{
    while(datalength--)
    {
        *pbuf++ = AT24C02_receiveByte(addr++);
    }
}

/**
 * @brief       在AT24CXX里面的指定地址开始写入指定个数的数据
 * @param       addr    : 开始写入的地址 对24c02为0~255
 * @param       pbuf    : 数据数组首地址
 * @param       datalength : 要写入数据的个数
 * @retval      无
 */
void AT24C02_write(uint16_t addr, uint8_t *pbuf, uint16_t datalength)
{
    while (datalength--)
    {
        AT24C02_writeByte(addr, *pbuf);
        addr++;
        pbuf++;
    }
}
