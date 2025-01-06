#ifndef __TOUCH_H
#define __TOUCH_H

#include "stm32f10x.h"
#include "delay.h"
#include "lcd.h"
#include "at24c02.h"
#include "stdio.h"
#include "stdlib.h"

#define TOUCH_CS_PORT       GPIOF
#define TOUCH_CS_PIN        GPIO_Pin_11
#define TOUCH_PEN_PORT      GPIOF
#define TOUCH_PEN_PIN       GPIO_Pin_10
#define TOUCH_MOSI_PORT     GPIOF
#define TOUCH_MOSI_PIN      GPIO_Pin_9
#define TOUCH_MISO_PORT     GPIOB
#define TOUCH_MISO_PIN      GPIO_Pin_2
#define TOUCH_SCK_PORT      GPIOB
#define TOUCH_SCK_PIN       GPIO_Pin_1

#define TP_READ_TIMES   5       /* 读取次数 */
#define TP_LOST_VAL     1       /* 丢弃值 */

#define TP_PRES_DOWN    0x8000      /* 触屏被按下 */
#define TP_CATH_PRES    0x4000      /* 有按键按下了 */
#define CT_MAX_TOUCH    10          /* 电容屏支持的点数,固定为5点 */

/* TP_SAVE_ADDR_BASE定义触摸屏校准参数保存在EEPROM里面的位置(起始地址)
 * 占用空间 : 13字节.
 */
#define TP_SAVE_ADDR_BASE   40

/* 触摸屏控制器 */
typedef struct
{
    uint8_t (*init)(void);      /* 初始化触摸屏控制器 */
    uint8_t (*scan)(uint8_t);   /* 扫描触摸屏.0,屏幕扫描;1,物理坐标; */
    void (*adjust)(void);       /* 触摸屏校准 */
    uint16_t x[CT_MAX_TOUCH];   /* 当前坐标 */
    uint16_t y[CT_MAX_TOUCH];   /* 电容屏有最多10组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
                                 * x[9],y[9]存储第一次按下时的坐标.
                                 */

    uint16_t sta;               /* 笔的状态
                                 * b15:按下1/松开0;
                                 * b14:0,没有按键按下;1,有按键按下.
                                 * b13~b10:保留
                                 * b9~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
                                 */

    /* 5点校准触摸屏校准参数(电容屏不需要校准) */
    float xfac;                 /* 5点校准法x方向比例因子 */
    float yfac;                 /* 5点校准法y方向比例因子 */
    short xc;                   /* 中心X坐标物理值(AD值) */
    short yc;                   /* 中心Y坐标物理值(AD值) */

    /* 新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
     * b0:0, 竖屏(适合左右为X坐标,上下为Y坐标的TP)
     *    1, 横屏(适合左右为Y坐标,上下为X坐标的TP)
     * b1~6: 保留.
     * b7:0, 电阻屏
     *    1, 电容屏
     */
    uint8_t touchtype;
} _m_touch_dev;

extern _m_touch_dev touch_dev;

void TOUCH_SCK(uint8_t state);
void TOUCH_MOSI(uint8_t state);
void TOUCH_CS(uint8_t state);
uint8_t touchInit(void);
void touch_writeByte(uint8_t data);
uint16_t touch_readADC(uint8_t cmd);
uint16_t touch_readXOY(uint8_t cmd);
void touch_readXY(uint16_t *x,uint16_t *y);
uint8_t touch_readXY2(uint16_t *x,uint16_t *y);
void touch_drawPoint(uint16_t x, uint16_t y, uint16_t color);
void touch_drawBigPoint(uint16_t x, uint16_t y, uint16_t color);
uint8_t touchScan(uint8_t mode);
void touchAdjust(void);
void touch_saveAdjustData(void);
uint8_t touch_getAdjustData(void);
void touch_adjustInfoShow(uint16_t xy[5][2], double px, double py);

#endif
