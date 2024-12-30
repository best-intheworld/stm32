#include "main.h"

#define LED0_Port GPIOB
#define LED1_Port GPIOE

/**
 * @brief       LED��ʼ������
 * @param       ��
 * @retval      ��
 */
void LED_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();//ʱ��ʹ��
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin=GPIO_PIN_5;
    GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;//�������ģʽ
    GPIO_InitStruct.Pull=GPIO_NOPULL;//������Ҳ������
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;//����

    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,SET);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,SET);

    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);//LED0
    HAL_GPIO_Init(GPIOE,&GPIO_InitStruct);//LED1
}

/**
 * @brief       LED״̬��ת����
 * @param       ��
 * @retval      ��
 */
void LED_Toggle(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    HAL_GPIO_TogglePin(GPIOx,GPIO_Pin);
}

/**
 * @brief       LED��������
 * @param       GPIOx:�ĸ��˿ڣ�GPIO_Pin:��������
 * @retval      ��
 */
void LED_Light(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    HAL_GPIO_WritePin(GPIOx,GPIO_Pin,RESET);
}

/**
 * @brief       LEDϨ����
 * @param       GPIOx:�ĸ��˿ڣ�GPIO_Pin:��������
 * @retval      ��
 */
void LED_Die(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    HAL_GPIO_WritePin(GPIOx,GPIO_Pin,SET);
}
