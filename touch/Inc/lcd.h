#ifndef __LCD_H
#define __LCD_H

#include "main.h"


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
