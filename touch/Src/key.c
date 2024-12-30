#include "main.h"

/**
 * @brief       key��ʼ������
 * @param       ��
 * @retval      ��
 */
void Key_Init(void)
{
    __HAL_RCC_GPIOE_CLK_ENABLE();//ʱ��ʹ��

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin=GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
    GPIO_InitStruct.Mode=GPIO_MODE_INPUT;//����ģʽ
    GPIO_InitStruct.Pull=GPIO_PULLUP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;//����

    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,SET);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,SET);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,SET);

    HAL_GPIO_Init(GPIOE,&GPIO_InitStruct);

    __HAL_RCC_GPIOA_CLK_ENABLE();//ʱ��ʹ��

    GPIO_InitStruct.Pin=GPIO_PIN_0;
    GPIO_InitStruct.Mode=GPIO_MODE_INPUT;//����ģʽ
    GPIO_InitStruct.Pull=GPIO_PULLDOWN;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;//����

    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,RESET);
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
}

/**
 * @brief       ��ȡ����ֵ����
 * @param       ��
 * @retval      ������Ӧֵ
 */
uint8_t getKeyNum(void)
{
    uint8_t keynum = 100;
    if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)==0)
    {
        HAL_Delay(10);//����
        while(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)==0)//�������²�����
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
        HAL_Delay(10);//����
        while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1)//�������²�����
        HAL_Delay(10);
        keynum=3;
    }
    return keynum;
}
