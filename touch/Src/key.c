#include "main.h"

/**
 * @brief       key初始化函数
 * @param       无
 * @retval      无
 */
void Key_Init(void)
{
    __HAL_RCC_GPIOE_CLK_ENABLE();//时钟使能

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin=GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
    GPIO_InitStruct.Mode=GPIO_MODE_INPUT;//输入模式
    GPIO_InitStruct.Pull=GPIO_PULLUP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;//高速

    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,SET);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,SET);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,SET);

    HAL_GPIO_Init(GPIOE,&GPIO_InitStruct);

    __HAL_RCC_GPIOA_CLK_ENABLE();//时钟使能

    GPIO_InitStruct.Pin=GPIO_PIN_0;
    GPIO_InitStruct.Mode=GPIO_MODE_INPUT;//输入模式
    GPIO_InitStruct.Pull=GPIO_PULLDOWN;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;//高速

    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,RESET);
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
}

/**
 * @brief       获取按键值函数
 * @param       无
 * @retval      按键对应值
 */
uint8_t getKeyNum(void)
{
    uint8_t keynum = 100;
    if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)==0)
    {
        HAL_Delay(10);//消抖
        while(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)==0)//持续按下不松手
        HAL_Delay(10);
        keynum=0;
    }
    if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3)==0)
    {
        HAL_Delay(10);
        while(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3)==0)
        HAL_Delay(10);
        keynum=1;
    }
    if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2)==0)
    {
        HAL_Delay(10);
        while(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2)==0)
        HAL_Delay(10);
        keynum=2;
    }
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1)
    {
        HAL_Delay(10);//消抖
        while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1)//持续按下不松手
        HAL_Delay(10);
        keynum=3;
    }
    return keynum;
}
