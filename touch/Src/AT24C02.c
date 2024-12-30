#include "AT24C02.h"

/**
 * @brief       AT24C02��ʼ������
 * @param       ��
 * @retval      ��
 */
void AT24C02_Init(void)
{
    I2C_Init();
}

/**
 * @brief       AT24C02����һ�ֽں���
 * @param       addr��ָ����ַ
 * @retval      ���յ���һ�ֽ�����
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
 * @brief       AT24C02����һ�ֽں���
 * @param       addr��ָ����ַ
 * @param       data:Ҫ���͵�����
 * @retval      ��
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
 * @brief       ���AT24C02�Ƿ�������
 * @param       ��
 * @retval      0��������1��������
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
 * @brief       ��AT24CXX�����ָ����ַ��ʼ����ָ������������
 * @param       addr    : ��ʼ�����ĵ�ַ ��24c02Ϊ0~255
 * @param       pbuf    : ���������׵�ַ
 * @param       datalength : Ҫ�������ݵĸ���
 * @retval      ��
 */
void AT24C02_read(uint16_t addr, uint8_t *pbuf, uint16_t datalength)
{
    while(datalength--)
    {
        *pbuf++ = AT24C02_receiveByte(addr++);
    }
}

/**
 * @brief       ��AT24CXX�����ָ����ַ��ʼд��ָ������������
 * @param       addr    : ��ʼд��ĵ�ַ ��24c02Ϊ0~255
 * @param       pbuf    : ���������׵�ַ
 * @param       datalength : Ҫд�����ݵĸ���
 * @retval      ��
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
