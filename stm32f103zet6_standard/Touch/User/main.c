#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "Delay.h"
#include "Serial.h"
#include "lcd.h"
#include "at24c02.h"
#include "touch.h"

/**
 * @brief       清空屏幕并在右上角显示"RST"
 * @param       无
 * @retval      无
 */
void load_draw_dialog(void)
{
  LCD_clear(WHITE);                                                /* 清屏 */
  LCD_showString(width - 24, 0, 16, BLUE, WHITE, "RST"); /* 显示清屏区域 */
}

/**
 * @brief       电阻触摸屏测试函数
 * @param       无
 * @retval      无
 */
void rtp_test(void)
{
  uint8_t keynum;
  uint8_t i = 0;

  while (1)
  {
    keynum = Key_getValue();
    touch_dev.scan(0);

    if (touch_dev.sta & TP_PRES_DOWN)  /* 触摸屏被按下 */
    {
      if (touch_dev.x[0] < width && touch_dev.y[0] < length)
      {
        if (touch_dev.x[0] > (width - 24) && touch_dev.y[0] < 16)
        {
          load_draw_dialog(); /* 清除 */
        }
        else 
        {
          touch_drawBigPoint(touch_dev.x[0], touch_dev.y[0], RED);   /* 画点 */
        }
      }
    }
    else 
    {
      delay_ms(10);       /* 没有按键按下的时候 */
    }
    
    if (keynum == 4)   /* KEY0按下,则执行校准程序 */
    {
      LCD_clear(WHITE);   /* 清屏 */
      touchAdjust();        /* 屏幕校准 */
      touch_saveAdjustData();
      load_draw_dialog();
    }

    i++;

    if (i % 20 == 0)LED_Toggle(GPIOB, GPIO_Pin_5);
  }
}

int main(void)
{
	delay_init();
	LED_Init();
	Key_Init();
	Serial_Init();
	LCD_Init();
	AT24C02_Init();
	touch_dev.init();
	LCD_showString(30, 50, 16, RED, WHITE, "STM32");
	LCD_showString(30, 70, 16, RED, WHITE, "TOUCH TEST");
	LCD_showString(30, 90, 16, RED, WHITE, "ATOM@ALIENTEK");
	LCD_showString(30, 110, 16, RED, WHITE, "Press KEY0 to Adjust"); /* 电阻屏才显示 */
	delay_ms(1500);
	load_draw_dialog();
	rtp_test(); /* 电阻屏测试 */
} 
