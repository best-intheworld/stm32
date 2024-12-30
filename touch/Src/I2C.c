#include "I2C.h"


/**
 * @brief       I2C初始化函数
 * @param       无
 * @retval      无
 */
void I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();//开启时钟

    GPIO_InitStruct.Pin = I2C_SCL;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
    GPIO_InitStruct.Pin = I2C_SDA;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
    I2C_end();
}

//state: 0或1
void SCL(uint8_t state)
{
    if(state)
    {
        HAL_GPIO_WritePin(GPIOB,I2C_SCL,GPIO_PIN_SET);//SCL置1
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB,I2C_SCL,GPIO_PIN_RESET);//SCL置0
    }
}

//state: 0或1
void SDA(uint8_t state)
{
    if(state)
    {
        HAL_GPIO_WritePin(GPIOB,I2C_SDA,GPIO_PIN_SET);//SCL置1
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB,I2C_SDA,GPIO_PIN_RESET);//SCL置0
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
    uint8_t data;
    for(int count = 0;count < 8;count++)
    {
        SCL(1);//SCL置1
        I2C_delay();
        if(HAL_GPIO_ReadPin(GPIOB,I2C_SDA))
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

    while(HAL_GPIO_ReadPin(GPIOB,I2C_SDA))
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


