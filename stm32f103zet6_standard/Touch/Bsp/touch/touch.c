#include "touch.h"

void touch_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = T_SCK_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(T_SCK_Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = T_MISO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(T_MISO_Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = T_MOSI_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(T_MOSI_Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = T_PEN_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(T_PEN_Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = T_CS_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(T_CS_Port, &GPIO_InitStructure);
}
