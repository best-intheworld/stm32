#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "Delay.h"
#include "Serial.h"


int main(void)
{
	delay_init();
	LED_Init();
	Key_Init();
	Serial_Init();
	
	int8_t keyNum;

	while(1)
	{
		keyNum = Key_getValue();
		switch (keyNum)
		{
			case 2:
			{
				LED_ON(GPIOB, GPIO_Pin_5);
				TRACE("KEY2 Push Down!\r\n");
				delay_ms(1000);
				break;
			}
			case 4:
			{
				LED_OFF(GPIOB, GPIO_Pin_5);
				TRACE("KEY0 Push Down!\r\n");
				delay_ms(1000);
				break;
			}
			case 3:
			{
				LED_ON(GPIOE, GPIO_Pin_5);
				TRACE("KEY1 Push Down!\r\n");
				delay_ms(1000);
				break;
			}
			case 0:
			{
				LED_OFF(GPIOE, GPIO_Pin_5);
				TRACE("KEY_UP Push Down!\r\n");
				delay_ms(1000);
				break;
			}
			default:
			{
				printf("USART1_ReceiveBuf = %s\r\n", USART1_ReceiveBuf);
				delay_ms(1000);
				break;
			}
		}
	}
} 
