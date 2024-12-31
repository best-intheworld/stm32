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

#define TP_READ_TIMES   5       /* ��ȡ���� */
#define TP_LOST_VAL     1       /* ����ֵ */

#define TP_PRES_DOWN    0x8000      /* ���������� */
#define TP_CATH_PRES    0x4000      /* �а��������� */
#define CT_MAX_TOUCH    10          /* ������֧�ֵĵ���,�̶�Ϊ5�� */

/* TP_SAVE_ADDR_BASE���崥����У׼����������EEPROM�����λ��(��ʼ��ַ)
 * ռ�ÿռ� : 13�ֽ�.
 */
#define TP_SAVE_ADDR_BASE   40

/* ������������ */
typedef struct
{
    uint8_t (*init)(void);      /* ��ʼ�������������� */
    uint8_t (*scan)(uint8_t);   /* ɨ�败����.0,��Ļɨ��;1,��������; */
    void (*adjust)(void);       /* ������У׼ */
    uint16_t x[CT_MAX_TOUCH];   /* ��ǰ���� */
    uint16_t y[CT_MAX_TOUCH];   /* �����������10������,����������x[0],y[0]����:�˴�ɨ��ʱ,����������,��
                                 * x[9],y[9]�洢��һ�ΰ���ʱ������.
                                 */

    uint16_t sta;               /* �ʵ�״̬
                                 * b15:����1/�ɿ�0;
                                 * b14:0,û�а�������;1,�а�������.
                                 * b13~b10:����
                                 * b9~b0:���ݴ��������µĵ���(0,��ʾδ����,1��ʾ����)
                                 */

    /* 5��У׼������У׼����(����������ҪУ׼) */
    float xfac;                 /* 5��У׼��x����������� */
    float yfac;                 /* 5��У׼��y����������� */
    short xc;                   /* ����X��������ֵ(ADֵ) */
    short yc;                   /* ����Y��������ֵ(ADֵ) */

    /* �����Ĳ���,��������������������ȫ�ߵ�ʱ��Ҫ�õ�.
     * b0:0, ����(�ʺ�����ΪX����,����ΪY�����TP)
     *    1, ����(�ʺ�����ΪY����,����ΪX�����TP)
     * b1~6: ����.
     * b7:0, ������
     *    1, ������
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
