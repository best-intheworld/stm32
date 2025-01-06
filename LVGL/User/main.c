#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "Delay.h"
#include "Serial.h"
#include "lcd.h"
#include "at24c02.h"
#include "touch.h"
#include "timer.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

int main(void)
{
  //Timer_Init();
	delay_init();
	LED_Init();
	Key_Init();
	Serial_Init();
	LCD_Init();
  LCD_scanDir(1);
	AT24C02_Init();
	touch_dev.init();
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  Timer_Init();
  // 按钮
  lv_obj_t *myBtn = lv_btn_create(lv_scr_act());                               // 创建按钮; 父对象：当前活动屏幕
  lv_obj_set_pos(myBtn, 10, 10);                                               // 设置坐标
  lv_obj_set_size(myBtn, 120, 50);                                             // 设置大小

  // 按钮上的文本
  lv_obj_t *label_btn = lv_label_create(myBtn);                                // 创建文本标签，父对象：上面的btn按钮
  lv_obj_align(label_btn, LV_ALIGN_CENTER, 0, 0);                              // 对齐于：父对象
  lv_label_set_text(label_btn, "Test");                                        // 设置标签的文本

  // 独立的标签
  lv_obj_t *myLabel = lv_label_create(lv_scr_act());                           // 创建文本标签; 父对象：当前活动屏幕
  lv_label_set_text(myLabel, "Hello world!");                                  // 设置标签的文本
  lv_obj_align(myLabel, LV_ALIGN_CENTER, 0, 0);                                // 对齐于：父对象
  lv_obj_align_to(myBtn, myLabel, LV_ALIGN_OUT_TOP_MID, 0, -20);               // 对齐于：某对象
  
  while (1)
  {
    int8_t keynum = Key_getValue();
    if (keynum == 4)   /* KEY0按下,则执行校准程序 */
    {
      LCD_clear(WHITE);   /* 清屏 */
      touchAdjust();        /* 屏幕校准 */
      touch_saveAdjustData();
    }
    lv_task_handler();
    delay_ms(5);
  }
} 
