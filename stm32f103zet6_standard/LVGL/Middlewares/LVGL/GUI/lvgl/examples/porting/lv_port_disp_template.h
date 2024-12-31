/**
 * @file lv_port_disp_templ.h
 *
 */

 /*Copy this file as "lv_port_disp.h" and set this value to "1" to enable content*/
#if 1

#ifndef LV_PORT_DISP_TEMPL_H
#define LV_PORT_DISP_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include "lcd.h"

/*********************
 *      DEFINES
 *********************/
#define USE_SRAM 0 /* 使用外部 sram 为 1，否则为 0 */
#define SRAMEX   1 /* SRAMEX内存池, 外扩SRAM, 共1024KB */
#define MY_DISP_HOR_RES (240) /* 屏幕宽度 */
#define MY_DISP_VER_RES (320) /* 屏幕高度 */
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_port_disp_init(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_DISP_TEMPL_H*/

#endif /*Disable/Enable content*/
