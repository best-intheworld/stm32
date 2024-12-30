#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "Delay.h"
#include "Serial.h"
#include "lcd.h"
#include "timer.h"

int main(void)
{
	delay_init();
	LED_Init();
	Key_Init();
	Serial_Init();
	LCD_Init();
	Timer_Init();

	LCD_showString(130, 0, 16, BLACK, WHITE, "Hello world!");

	while(1)
	{
		LED_Toggle(GPIOB, GPIO_Pin_5);
		delay_ms(1000);
	}
} 
