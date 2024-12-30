#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "Delay.h"
#include "Serial.h"
#include "lcd.h"
#include "at24c02.h"

const uint8_t g_text_buf[] = {"STM32 IIC TEST"};
#define TEXT_SIZE       sizeof(g_text_buf)  /* TEXT×Ö·û´®³¤¶È */

int main(void)
{
	delay_init();
	LED_Init();
	Key_Init();
	Serial_Init();
	LCD_Init();
	AT24C02_Init();

	LCD_showString(30, 50, 16, RED, WHITE, "STM32");
	LCD_showString(30, 70, 16, RED, WHITE, "IIC TEST");
	LCD_showString(30, 90, 16, RED, WHITE, "ATOM@ALIENTEK");
	LCD_showString(30, 110, 16, RED, WHITE, "KEY1:Write  KEY0:Read");
	while(AT24C02_check())
	{
		LCD_showString(30, 130, 16, RED, WHITE, "24C02 Check Failed!");
		delay_ms(300);
		LCD_showString(30, 130, 16, RED, WHITE, "Please Check!      ");
		delay_ms(300);
		LED_Toggle(GPIOB, GPIO_Pin_5);
	}
	LCD_showString(30, 130, 16, RED, WHITE, "24C02 Ready!");
	uint8_t keynum;
	uint8_t datatemp[TEXT_SIZE];
	memset(datatemp, 0, TEXT_SIZE);
	while(1)
	{
		keynum = Key_getValue();
		if(keynum == 3)
		{
			LCD_fillRectangle(0, 150, 239, 319, WHITE);
			LCD_showString(30, 150, 16, BLUE, WHITE, "Start Write 24C02....");
			AT24C02_write(0, (uint8_t *)g_text_buf, TEXT_SIZE);
			LCD_showString(30, 150, 16, BLUE, WHITE, "24C02 Write Finished!");
		}
		if(keynum == 4)
		{
			LCD_showString(30, 150, 16, BLUE, WHITE, "Start Read 24C02.... ");
			AT24C02_read(0, datatemp, TEXT_SIZE);
			LCD_showString(30, 150, 16, BLUE, WHITE, "The Data Readed Is:  ");
			LCD_showString(30, 170, 16, BLUE, WHITE, (char *)datatemp);
		}
		LED_Toggle(GPIOE, GPIO_Pin_5);
		delay_ms(200);
	}
} 
