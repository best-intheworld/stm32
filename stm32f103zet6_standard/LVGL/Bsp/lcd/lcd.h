#ifndef __LCD_H
#define __LCD_H

#include "stm32f10x.h"
#include <math.h>
#include "Delay.h"

//LCD参数宏定义
#define width       240
#define length      320
#define writeRamCmd 0x2C
#define setXCmd     0x2A
#define setYCmd     0x2B

/* 常用画笔颜色 */
#define WHITE           0xFFFF      /* 白色 */
#define BLACK           0x0000      /* 黑色 */
#define RED             0xF800      /* 红色 */
#define GREEN           0x07E0      /* 绿色 */
#define BLUE            0x001F      /* 蓝色 */ 
#define MAGENTA         0XF81F      /* 品红色/紫红色 = BLUE + RED */
#define YELLOW          0XFFE0      /* 黄色 = GREEN + RED */
#define CYAN            0X07FF      /* 青色 = GREEN + BLUE */  

/* 非常用颜色 */
#define BROWN           0XBC40      /* 棕色 */
#define BRRED           0XFC07      /* 棕红色 */
#define GRAY            0X8430      /* 灰色 */ 
#define DARKBLUE        0X01CF      /* 深蓝色 */
#define LIGHTBLUE       0X7D7C      /* 浅蓝色 */ 
#define GRAYBLUE        0X5458      /* 灰蓝色 */ 
#define LIGHTGREEN      0X841F      /* 浅绿色 */  
#define LGRAY           0XC618      /* 浅灰色(PANNEL),窗体背景色 */ 
#define LGRAYBLUE       0XA651      /* 浅灰蓝色(中间层颜色) */ 
#define LBBLUE          0X2B12      /* 浅棕蓝色(选择条目的反色) */ 

void LCD_writeCommand(uint16_t command);
void LCD_writeData(uint16_t data);
void LCD_writeCommandAndData(uint16_t command, uint16_t data);
void LCD_prepareWriteRam(void);
void lcd_ex_st7789_reginit(void);
void LCD_Init(void);
void LCD_clear(uint16_t color);
void LCD_drawPoint(uint16_t x, uint16_t y, uint16_t color);
void LCD_drawHorizontalLine(uint16_t x, uint16_t y, uint16_t color, uint16_t lineLength);
void LCD_drawVerticalLine(uint16_t x, uint16_t y, uint16_t color, uint16_t lineLength);
void LCD_drawRectangle(uint16_t x, uint16_t y, uint16_t color, uint16_t Width, uint16_t Height);
void LCD_setCursor(uint16_t x, uint16_t y);
void LCD_setDisplayAera(uint16_t sx, uint16_t ex, uint16_t sy, uint16_t ey);
void LCD_fillRectangle(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void LCD_fillColor(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);
void LCD_drawSlopingLine(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void LCD_writeCircle(uint16_t x, uint16_t y, uint16_t color, uint16_t radius);
void LCD_fillCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color);

void LCD_showChar(uint16_t x, uint16_t y, uint8_t size, uint16_t color, uint16_t backcolor, char chr);
void LCD_showString(uint16_t x, uint16_t y, uint8_t size, uint16_t color, uint16_t backcolor, char* str);
void LCD_showNum(uint16_t x, uint16_t y, uint8_t size, uint16_t color, uint16_t backcolor, uint64_t num);

#endif
