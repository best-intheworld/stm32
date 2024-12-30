#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "Delay.h"
#include "Serial.h"
#include "lcd.h"

int main(void)
{
	delay_init();
	LED_Init();
	Key_Init();
	Serial_Init();
	LCD_Init();

	LCD_writeCircle(119, 159, BLACK, 100);
	LCD_fillCircle(119, 159, 100, RED);
	LCD_drawRectangle(59, 79, BLACK, 120, 160);
	LCD_fillRectangle(60, 80, 177, 237, YELLOW);
	LCD_writeCircle(119, 159, BLACK, 60);
	LCD_fillCircle(119, 159, 60, BLUE);
	LCD_drawHorizontalLine(0, 159, BLACK, 240);
	LCD_drawSlopingLine(0, 0, 239, 319, BLACK);
	LCD_drawSlopingLine(0, 319, 239, 0, BLACK);
	LCD_drawPoint(119, 159, RED);

	LCD_showChar(30, 0, 16, BLACK, WHITE, 'A');
	LCD_showChar(40, 0, 16, BLACK, WHITE, 'B');
	LCD_showChar(50, 0, 16, BLACK, WHITE, 'C');
	LCD_showString(130, 0, 16, BLACK, WHITE, "Hello world!");
	LCD_showNum(40, 20, 32, BLACK, WHITE, 1234567890);
	LCD_drawVerticalLine(119, 0, BLACK, 320);

	while(1)
	{
		LED_Toggle(GPIOB, GPIO_Pin_5);
		delay_ms(500);
	}
} 
