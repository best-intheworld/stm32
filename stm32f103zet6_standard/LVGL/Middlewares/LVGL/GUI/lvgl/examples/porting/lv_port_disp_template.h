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
#define USE_SRAM 0 /* ʹ���ⲿ sram Ϊ 1������Ϊ 0 */
#define SRAMEX   1 /* SRAMEX�ڴ��, ����SRAM, ��1024KB */
#define MY_DISP_HOR_RES (240) /* ��Ļ���� */
#define MY_DISP_VER_RES (320) /* ��Ļ�߶� */
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