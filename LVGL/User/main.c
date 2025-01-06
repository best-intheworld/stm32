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
  // ��ť
  lv_obj_t *myBtn = lv_btn_create(lv_scr_act());                               // ������ť; �����󣺵�ǰ���Ļ
  lv_obj_set_pos(myBtn, 10, 10);                                               // ��������
  lv_obj_set_size(myBtn, 120, 50);                                             // ���ô�С

  // ��ť�ϵ��ı�
  lv_obj_t *label_btn = lv_label_create(myBtn);                                // �����ı���ǩ�������������btn��ť
  lv_obj_align(label_btn, LV_ALIGN_CENTER, 0, 0);                              // �����ڣ�������
  lv_label_set_text(label_btn, "Test");                                        // ���ñ�ǩ���ı�

  // �����ı�ǩ
  lv_obj_t *myLabel = lv_label_create(lv_scr_act());                           // �����ı���ǩ; �����󣺵�ǰ���Ļ
  lv_label_set_text(myLabel, "Hello world!");                                  // ���ñ�ǩ���ı�
  lv_obj_align(myLabel, LV_ALIGN_CENTER, 0, 0);                                // �����ڣ�������
  lv_obj_align_to(myBtn, myLabel, LV_ALIGN_OUT_TOP_MID, 0, -20);               // �����ڣ�ĳ����
  
  while (1)
  {
    int8_t keynum = Key_getValue();
    if (keynum == 4)   /* KEY0����,��ִ��У׼���� */
    {
      LCD_clear(WHITE);   /* ���� */
      touchAdjust();        /* ��ĻУ׼ */
      touch_saveAdjustData();
    }
    lv_task_handler();
    delay_ms(5);
  }
} 
