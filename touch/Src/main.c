#include "main.h"
#include "led.h"
#include "key.h"
#include "touch.h"


/**
 * @brief       清空屏幕并在右上角显示"RST"
 * @param       无
 * @retval      无
 */
void load_draw_dialog(void)
{
  LCD_clear(WHITE);                                                /* 清屏 */
  LCD_showString(width - 24, 0, 200, 16, 16, "RST", BLUE,WHITE); /* 显示清屏区域 */
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
    keynum = getKeyNum();
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
    
    if (keynum == 0)   /* KEY0按下,则执行校准程序 */
    {
      LCD_clear(WHITE);   /* 清屏 */
      touchAdjust();        /* 屏幕校准 */
      touch_saveAdjustData();
      load_draw_dialog();
    }

    i++;

    if (i % 20 == 0)LED_Toggle(LED0_Port,GPIO_PIN_5);
  }
}

int main(void)
{
  HAL_Init();
  sys_stm32_clock_init(RCC_CFGR_PLLMULL9);
  delay_init(72);
  Key_Init();
  LCD_Init();
  LED_Init();
  touch_dev.init();
  LCD_showString(30, 50, 200, 16, 16, "STM32", RED,WHITE);
  LCD_showString(30, 70, 200, 16, 16, "TOUCH TEST", RED,WHITE);
  LCD_showString(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED,WHITE);
  LCD_showString(30, 110, 200, 16, 16, "Press KEY0 to Adjust", RED,WHITE); /* 电阻屏才显示 */
  delay_ms(1500);
  load_draw_dialog();
  rtp_test(); /* 电阻屏测试 */
}


/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
