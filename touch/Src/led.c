#include "main.h"

#define LED0_Port GPIOB
#define LED1_Port GPIOE

/**
 * @brief       LED初始化函数
 * @param       无
 * @retval      无
 */
void LED_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();//时钟使能
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin=GPIO_PIN_5;
    GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;//推挽输出模式
    GPIO_InitStruct.Pull=GPIO_NOPULL;//不上拉也不下拉
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;//低速

    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,SET);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,SET);

    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);//LED0
    HAL_GPIO_Init(GPIOE,&GPIO_InitStruct);//LED1
}

/**
 * @brief       LED状态翻转函数
 * @param       无
 * @retval      无
 */
void LED_Toggle(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    HAL_GPIO_TogglePin(GPIOx,GPIO_Pin);
}

/**
 * @brief       LED点亮函数
 * @param       GPIOx:哪个端口；GPIO_Pin:具体引脚
 * @retval      无
 */
void LED_Light(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    HAL_GPIO_WritePin(GPIOx,GPIO_Pin,RESET);
}

/**
 * @brief       LED熄灭函数
 * @param       GPIOx:哪个端口；GPIO_Pin:具体引脚
 * @retval      无
 */
void LED_Die(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    HAL_GPIO_WritePin(GPIOx,GPIO_Pin,SET);
}
