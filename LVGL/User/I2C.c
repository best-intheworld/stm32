#include "I2C.h"

/**
 * @brief       I2C初始化函数
 * @param       无
 * @retval      无
 */
void IIC_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = I2C_SCL;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = I2C_SDA;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    I2C_end();
}

//state: 0或1
void SCL(uint8_t state)
{
    if(state)
    {
        GPIO_SetBits(GPIOB, I2C_SCL);
    }
    else
    {
        GPIO_ResetBits(GPIOB, I2C_SCL);
    }
}

//state: 0或1
void SDA(uint8_t state)
{
    if(state)
    {
        GPIO_SetBits(GPIOB, I2C_SDA);
    }
    else
    {
        GPIO_ResetBits(GPIOB, I2C_SDA);
    }
}

void I2C_delay(void)
{
    delay_us(2);
}

/**
 * @brief       I2C起始函数
 * @param       无
 * @retval      无
 */
void I2C_start(void)
{
    SDA(1);
    SCL(1);
    I2C_delay();
    SDA(0);
    I2C_delay();
    SCL(0);
    I2C_delay();
}

/**
 * @brief       I2C终止函数
 * @param       无
 * @retval      无
 */
void I2C_end(void)
{
    SDA(0);
    I2C_delay();
    SCL(1);
    I2C_delay();
    SDA(1);
    I2C_delay();
}

/**
 * @brief       I2C发送一字节函数
 * @param       data:要发送的一字节数据
 * @retval      无
 */
void I2C_sendByte(uint8_t data)
{
    for(int count = 0;count < 8;count++)
    {
        SDA((data >> (7 - count)) & 0x01);
        I2C_delay();
        SCL(1);
        I2C_delay();
        SCL(0);
    }
    SDA(1);//主机发送完释放SDA
}

/**
 * @brief       I2C接收一字节函数
 * @param       无
 * @retval      data:接收到的一字节数据
 */
uint8_t I2C_receiveByte(uint8_t ack)
{
    uint8_t data = 0;
    SDA(1);//主机释放SDA
    for(int count = 0;count < 8;count++)
    {
        SCL(1);//SCL置1
        I2C_delay();
        if(GPIO_ReadInputDataBit(GPIOB, I2C_SDA))
        {
            data |= (0x80 >> count);
        }
        SCL(0);//SCL置0
        I2C_delay();
    }
    if(ack == 0)
    {
        I2C_sendACK();
    }
    else
    {
        I2C_sendNACK();
    }
    return data;
}

/**
 * @brief       I2C接收应答函数
 * @param       无
 * @retval      ackbit:0表示应答；1表示非应答
 */
uint8_t I2C_receiveACK(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;
    SDA(1);//主机释放SDA
    I2C_delay();
    SCL(1);//SCL置0
    I2C_delay();

    while(GPIO_ReadInputDataBit(GPIOB, I2C_SDA))
    {
        waittime++;
        if(waittime > 250)
        {
            I2C_end();
            rack = 1;
            break;
        }
    }
    SCL(0);
    I2C_delay();
    return rack;
}

/**
 * @brief       I2C发送应答函数
 * @param       无
 * @retval      无
 */
void I2C_sendACK(void)
{
    SDA(0);
    I2C_delay();
    SCL(1);
    I2C_delay();
    SCL(0);
    I2C_delay();
    SDA(1);
    I2C_delay();
}

/**
 * @brief       I2C发送未应答函数
 * @param       无
 * @retval      无
 */
void I2C_sendNACK(void)
{
    SDA(1);
    I2C_delay();
    SCL(1);
    I2C_delay();
    SCL(0);
    I2C_delay();
}


