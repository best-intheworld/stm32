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
#include "lv_port_indev_template.h"
#include "lv_port_disp_template.h"

int main(void)
{
	delay_init();
	LED_Init();
	Key_Init();
	Serial_Init();
	//LCD_Init();
	AT24C02_Init();
	touch_dev.init();
  Timer_Init();
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello world!");
  lv_obj_center(label);
  uint32_t count = 0;
  while (1) {
    count++;
    if (count % 100 == 0) {
      LED_Toggle(GPIOB, GPIO_Pin_5);
    }
    lv_timer_handler();
    delay_ms(5);
  }
} 
