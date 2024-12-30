#ifndef __LCD_H
#define __LCD_H

#include "main.h"


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

void LCD_Init(void);
void lcd_ex_st7789_reginit(void);
void LCD_writeRegNum(uint16_t regNum);
void LCD_displayOn(void);
void LCD_displayOff(void);
void LCD_writeData(uint16_t Data);
void LCD_writeReg(uint16_t Reg,uint16_t Data);
void LCD_setCursor(uint16_t x,uint16_t y);
void LCD_prepareWriteRam(void);
void LCD_drawPoint(uint16_t x,uint16_t y,uint32_t color);
void LCD_drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_drawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
uint16_t LCD_readData(void);
uint32_t LCD_readPoint(uint16_t x,uint16_t y);
void LCD_clear(uint32_t color);
uint32_t LCD_Pow(uint8_t m, uint8_t n);
void LCD_showChar(uint16_t x,uint16_t y,char chr,uint8_t size,uint8_t mode, uint32_t color,uint32_t backColor);
void LCD_showNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color,uint32_t backColor);
void LCD_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);
void LCD_showString(uint16_t x, uint16_t y, uint16_t Width, uint16_t height, uint8_t size, char *p, uint16_t color,uint32_t backColor);
void LCD_drawHline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);
void LCD_fillCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);

#endif
