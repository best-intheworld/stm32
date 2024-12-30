#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "Delay.h"
#include "Serial.h"
#include "lcd.h"
#include "sram.h"
#include "timer.h"

#if (__ARMCC_VERSION >= 6010050)
uint32_t g_test_buffer[5000] __attribute__((section(".bss.ARM.__at_0x68000000")));
#else
uint32_t g_test_buffer[5000] __attribute__((at(SRAM_ADDR)));
#endif

void fsmc_sram_test(uint16_t x, uint16_t y)
{
    uint32_t i = 0;
    uint8_t temp = 0;
    uint8_t sval = 0; /* �ڵ�ַ0���������� */

    LCD_showString(x, y, 16, BLUE, WHITE, "Ex Memory Test:   0KB");

    /* ÿ��4K�ֽ�,д��һ������,�ܹ�д��256������,�պ���1M�ֽ� */
    for (i = 0; i < 1024 * 1024; i += 4096)
    {
        sram_Write(&temp, i, 1);
        temp++;
    }

    /* ���ζ���֮ǰд�������,����У�� */
    for (i = 0; i < 1024 * 1024; i += 4096)
    {
        sram_Read(&temp, i, 1);

        if (i == 0)
        {
            sval = temp;
        }
        else if (temp <= sval)
        {
            break; /* �������������һ��Ҫ�ȵ�һ�ζ��������ݴ� */
        }
    }
    LCD_showString(x, y, 16, BLUE, WHITE, "Ex Memory Test:    KB");
    LCD_showNum(x + 15 * 8, y, 16, BLUE, WHITE, (uint16_t)(temp - sval + 1) * 4); /* ��ʾ�ڴ����� */
}

int main(void)
{
	delay_init();
	LED_Init();
	Key_Init();
	Serial_Init();
	LCD_Init();
	sram_Init();

	LCD_showString(47, 0, 32, BLACK, WHITE, "Sram Test");

	for (uint32_t i = 0; i < 5000; i++)
	{
		g_test_buffer[i] = i;
	}
	fsmc_sram_test(30, 150);
	delay_ms(3000);
	uint32_t num = 0;
	while(1)
	{
		LCD_showChar(150, 200, 32, WHITE, WHITE, ' ');
		LCD_showNum(150, 200, 32, RED, WHITE, g_test_buffer[num]);
		num++;
		if (num >= 5000)
		num = 0;
		LED_Toggle(GPIOB, GPIO_Pin_5);
		delay_ms(1000);
	}
} 
