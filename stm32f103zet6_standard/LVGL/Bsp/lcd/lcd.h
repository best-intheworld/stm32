#ifndef __LCD_H
#define __LCD_H

#include "stm32f10x.h"
#include <math.h>
#include "Delay.h"

//LCD�����궨��
#define width       240
#define length      320
#define writeRamCmd 0x2C
#define setXCmd     0x2A
#define setYCmd     0x2B

/* ���û�����ɫ */
#define WHITE           0xFFFF      /* ��ɫ */
#define BLACK           0x0000      /* ��ɫ */
#define RED             0xF800      /* ��ɫ */
#define GREEN           0x07E0      /* ��ɫ */
#define BLUE            0x001F      /* ��ɫ */ 
#define MAGENTA         0XF81F      /* Ʒ��ɫ/�Ϻ�ɫ = BLUE + RED */
#define YELLOW          0XFFE0      /* ��ɫ = GREEN + RED */
#define CYAN            0X07FF      /* ��ɫ = GREEN + BLUE */  

/* �ǳ�����ɫ */
#define BROWN           0XBC40      /* ��ɫ */
#define BRRED           0XFC07      /* �غ�ɫ */
#define GRAY            0X8430      /* ��ɫ */ 
#define DARKBLUE        0X01CF      /* ����ɫ */
#define LIGHTBLUE       0X7D7C      /* ǳ��ɫ */ 
#define GRAYBLUE        0X5458      /* ����ɫ */ 
#define LIGHTGREEN      0X841F      /* ǳ��ɫ */  
#define LGRAY           0XC618      /* ǳ��ɫ(PANNEL),���屳��ɫ */ 
#define LGRAYBLUE       0XA651      /* ǳ����ɫ(�м����ɫ) */ 
#define LBBLUE          0X2B12      /* ǳ����ɫ(ѡ����Ŀ�ķ�ɫ) */ 

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
