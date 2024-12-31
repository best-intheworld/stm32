#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "timer.h"
#include "Delay.h"

int main(void)
{
  Timer_Init();
  lv_init();
  delay_init();
  lv_port_disp_init();
  lv_port_indev_init();
  lv_obj_t * label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello World!");
  lv_obj_center(label);
  while(1) {
    lv_task_handler();
    delay_ms(5);
  }
}
