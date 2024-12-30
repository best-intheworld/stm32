#ifndef __LED_H
#define __LED_H

#define LED0_Port GPIOB
#define LED1_Port GPIOE

void LED_Init(void);
void LED_Toggle(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void LED_Light(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void LED_Die(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

#endif
