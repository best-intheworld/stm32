#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "Delay.h"


int main(void)
{
	delay_init();
	LED_Init();
	Key_Init();
	
	int8_t keyNum;

	while(1)
	{
		keyNum = Key_getValue();
		switch (keyNum)
		{
			case 2:
			{
				LED_ON(GPIOB, GPIO_Pin_5);
				break;
			}
			case 4:
			{
				LED_OFF(GPIOB, GPIO_Pin_5);
				break;
			}
			case 3:
			{
				LED_ON(GPIOE, GPIO_Pin_5);
				break;
			}
			case 0:
			{
				LED_OFF(GPIOE, GPIO_Pin_5);
				break;
			}
			default:
				break;
		}
	}
} 
