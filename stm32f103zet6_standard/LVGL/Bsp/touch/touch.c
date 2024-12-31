#include "touch.h"

void TOUCH_SCK(uint8_t state)
{
    if(state)
    {
        GPIO_SetBits(TOUCH_SCK_PORT, TOUCH_SCK_PIN);
    }
    else
    {
        GPIO_ResetBits(TOUCH_SCK_PORT, TOUCH_SCK_PIN);
    }
}

void TOUCH_MOSI(uint8_t state)
{
    if(state)
    {
        GPIO_SetBits(TOUCH_MOSI_PORT, TOUCH_MOSI_PIN);
    }
    else
    {
        GPIO_ResetBits(TOUCH_MOSI_PORT, TOUCH_MOSI_PIN);
    }
}

void TOUCH_CS(uint8_t state)
{
    if(state)
    {
        GPIO_SetBits(TOUCH_CS_PORT, TOUCH_CS_PIN);
    }
    else
    {
        GPIO_ResetBits(TOUCH_CS_PORT, TOUCH_CS_PIN);
    }
}

_m_touch_dev touch_dev =
{
    touchInit,
    touchScan,
    touchAdjust,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

/**
 * @brief       ��������ʼ��
 * @param       ��
 * @retval      0,û�н���У׼
 *              1,���й�У׼
 */
uint8_t touchInit(void)
{
    touch_dev.touchtype = 0;                   /* Ĭ������(������ & ����) */
    touch_dev.touchtype |= 0 & 0x01;  /* ����LCD�ж��Ǻ����������� */

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOF, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = TOUCH_PEN_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TOUCH_PEN_PORT,&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = TOUCH_MISO_PIN;
    GPIO_Init(TOUCH_MISO_PORT,&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = TOUCH_MOSI_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TOUCH_MOSI_PORT,&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = TOUCH_SCK_PIN;
    GPIO_Init(TOUCH_SCK_PORT,&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = TOUCH_CS_PIN;
    GPIO_Init(TOUCH_CS_PORT,&GPIO_InitStruct);
    touch_readXY(&touch_dev.x[0],&touch_dev.y[0]);
    AT24C02_Init();
    if(touch_getAdjustData())
    {
        return 0;
    }
    else
    {
        LCD_clear(WHITE);
        touchAdjust();
        touch_saveAdjustData();
    }
    touch_getAdjustData();
    return 1;
}


/**
 * @brief       SPIд����
 *   @note      ������ICд��1 byte����
 * @param       data: Ҫд�������
 * @retval      ��
 */
void touch_writeByte(uint8_t data)
{
    for(uint8_t i = 0;i < 8;i++)
    {
        if(data & 0x80)
        {
            TOUCH_MOSI(1);
        }
        else
        {
            TOUCH_MOSI(0);
        }
        data <<= 1;
        TOUCH_SCK(0);
        delay_us(1);
        TOUCH_SCK(1);
    }
}

/**
 * @brief       SPI������
 *   @note      �Ӵ�����IC��ȡadcֵ
 * @param       cmd: ָ��
 * @retval      ��ȡ��������,ADCֵ(12bit)
 */
uint16_t touch_readADC(uint8_t cmd)
{
    uint16_t data = 0;
    TOUCH_SCK(0);
    TOUCH_MOSI(0);
    TOUCH_CS(0);
    touch_writeByte(cmd);
    delay_us(6);//adcת�������Ҫ6us
    TOUCH_SCK(0);//����һ��ʱ�����busyλ
    delay_us(1);
    TOUCH_SCK(1);
    delay_us(1);
    TOUCH_SCK(0);
    for(uint8_t i = 0; i < 16; i++)
    {
        data <<= 1;
        TOUCH_SCK(0);//�½�����Ч
        delay_us(1);
        TOUCH_SCK(1);
        if(GPIO_ReadInputDataBit(TOUCH_MISO_PORT, TOUCH_MISO_PIN))
        {
            data++;
        }
    }
    data >>= 4;//ADC12λ��Ч
    TOUCH_CS(1);
    return data;
}

/**
 * @brief       ��ȡһ������ֵ(x����y)
 *   @note      ������ȡ5������,����Щ������������,
 *              Ȼ��ȥ����ͺ����, ȡƽ��ֵ
 *
 * @param       cmd : ָ��
 *   @arg       0XD0: ��ȡX������(@����״̬,����״̬��Y�Ե�.)
 *   @arg       0X90: ��ȡY������(@����״̬,����״̬��X�Ե�.)
 *
 * @retval      ��ȡ��������(�˲����), ADCֵ(12bit)
 */
uint16_t touch_readXOY(uint8_t cmd)
{
    uint16_t temp = 0;
    uint16_t sum = 0;
    uint16_t data = 0;
    uint16_t buf[TP_READ_TIMES];
    for(uint8_t i = 0;i < TP_READ_TIMES;i++)//��ȡ5������
    {
        buf[i] = touch_readADC(cmd);
    }
    for(uint8_t i = 0;i < TP_READ_TIMES - 1;i++)//��������:����
    {
        for(uint8_t j = i + 1;j < TP_READ_TIMES;j++)//ÿһ�ֵ�buf[i]���Ǳ��ִ���С����
        {
            if(buf[i] > buf[j])
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    for (uint16_t i = TP_LOST_VAL; i < TP_READ_TIMES - TP_LOST_VAL; i++)   /* ȥ�����˵Ķ���ֵ */
    {
        sum += buf[i];  /* �ۼ�ȥ������ֵ�Ժ������. */
    }
    data = sum / (TP_READ_TIMES - 2 * TP_LOST_VAL);
    return data;
}

/**
 * @brief       ��ȡx, y����
 * @param       x,y: ��ȡ��������ֵ
 * @retval      ��
 */
void touch_readXY(uint16_t *x,uint16_t *y)
{
    *x = touch_readXOY(0xD0);
    *y = touch_readXOY(0x90);
}

/**
 * @brief       ������ȡ2�δ���IC����, ���˲�
 *   @note      ����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���ERR_RANGE,����
 *              ����,����Ϊ������ȷ,�����������.�ú����ܴ�����׼ȷ��.
 *
 * @param       x,y: ��ȡ��������ֵ
 * @retval      0, ʧ��; 1, �ɹ�;
 */
uint8_t touch_readXY2(uint16_t *x,uint16_t *y)
{
    uint16_t x1,y1;
    uint16_t x2,y2;
    touch_readXY(&x1,&y1);
    touch_readXY(&x2,&y2);
    if (((x2 <= x1 && x1 < x2 + 50) || (x1 <= x2 && x2 < x1 + 50)) &&
            ((y2 <= y1 && y1 < y2 + 50) || (y1 <= y2 && y2 < y1 + 50)))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;
        return 1;
    }
    return 0;
}

/**
 * @brief       ��һ��У׼�õĴ�����(ʮ�ּ�)
 * @param       x,y   : ����
 * @param       color : ��ɫ
 * @retval      ��
 */
void touch_drawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_drawHorizontalLine(x - 12, y, color, 25); /* ���� */
    LCD_drawVerticalLine(x, y - 12, color, 25); /* ���� */
    LCD_drawPoint(x + 1, y + 1, color);
    LCD_drawPoint(x - 1, y + 1, color);
    LCD_drawPoint(x + 1, y - 1, color);
    LCD_drawPoint(x - 1, y - 1, color);
    LCD_writeCircle(x, y, color, 6);            /* ������Ȧ */
}

/**
 * @brief       ��һ�����(2*2�ĵ�)
 * @param       x,y   : ����
 * @param       color : ��ɫ
 * @retval      ��
 */
void touch_drawBigPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_drawPoint(x, y, color);       /* ���ĵ� */
    LCD_drawPoint(x + 1, y, color);
    LCD_drawPoint(x, y + 1, color);
    LCD_drawPoint(x + 1, y + 1, color);
}

/**
 * @brief       ��������ɨ��
 * @param       mode: ����ģʽ
 *   @arg       0, ��Ļ����;
 *   @arg       1, ��������(У׼�����ⳡ����)
 *
 * @retval      0, �����޴���; 1, �����д���;
 */
uint8_t touchScan(uint8_t mode)
{
    if (GPIO_ReadInputDataBit(TOUCH_PEN_PORT,TOUCH_PEN_PIN) == 0)     /* �а������� */
    {
        if (mode)       /* ��ȡ��������, ����ת�� */
        {
            touch_readXY2(&touch_dev.x[0], &touch_dev.y[0]);
        }
        else if (touch_readXY2(&touch_dev.x[0], &touch_dev.y[0]))     /* ��ȡ��Ļ����, ��Ҫת�� */
        {
            /* ��X�� ��������ת�����߼�����(����ӦLCD��Ļ�����X����ֵ) */
            touch_dev.x[0] = (signed short)(touch_dev.x[0] - touch_dev.xc) / touch_dev.xfac + width / 2;

            /* ��Y�� ��������ת�����߼�����(����ӦLCD��Ļ�����Y����ֵ) */
            touch_dev.y[0] = (signed short)(touch_dev.y[0] - touch_dev.yc) / touch_dev.yfac + length / 2;
        }

        if ((touch_dev.sta & TP_PRES_DOWN) == 0)   /* ֮ǰû�б����� */
        {
            touch_dev.sta = TP_PRES_DOWN | TP_CATH_PRES;   /* �������� */
            touch_dev.x[CT_MAX_TOUCH - 1] = touch_dev.x[0];   /* ��¼��һ�ΰ���ʱ������ */
            touch_dev.y[CT_MAX_TOUCH - 1] = touch_dev.y[0];
        }
    }
    else
    {
        if (touch_dev.sta & TP_PRES_DOWN)      /* ֮ǰ�Ǳ����µ� */
        {
            touch_dev.sta &= ~TP_PRES_DOWN;    /* ��ǰ����ɿ� */
        }
        else     /* ֮ǰ��û�б����� */
        {
            touch_dev.x[CT_MAX_TOUCH - 1] = 0;
            touch_dev.y[CT_MAX_TOUCH - 1] = 0;
            touch_dev.x[0] = 0xFFFF;
            touch_dev.y[0] = 0xFFFF;
        }
    }

    return touch_dev.sta & TP_PRES_DOWN; /* ���ص�ǰ�Ĵ���״̬ */
}

char *const TP_REMIND_MSG_TBL = "Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";

/**
 * @brief       ������У׼����
 *   @note      ʹ�����У׼��(����ԭ����ٶ�)
 *              �������õ�x��/y���������xfac/yfac��������������ֵ(xc,yc)��4������
 *              ���ǹ涨: �������꼴AD�ɼ���������ֵ,��Χ��0~4095.
 *                        �߼����꼴LCD��Ļ������, ��ΧΪLCD��Ļ�ķֱ���.
 *
 * @param       ��
 * @retval      ��
 */
void touchAdjust(void)
{
    uint16_t pxy[5][2];     /* �������껺��ֵ */
    uint8_t  cnt = 0;
    short s1, s2, s3, s4;   /* 4����������ֵ */
    double px, py;          /* X,Y�������������,�����ж��Ƿ�У׼�ɹ� */
    uint16_t outtime = 0;
    cnt = 0;

    LCD_clear(WHITE);       /* ���� */
    //LCD_showString(40, 40, 16, RED, WHITE, TP_REMIND_MSG_TBL); /* ��ʾ��ʾ��Ϣ */
    touch_drawPoint(20, 20, RED);   /* ����1 */
    touch_dev.sta = 0;         /* ���������ź� */

    while (1)               /* �������10����û�а���,���Զ��˳� */
    {
        touch_dev.scan(1);     /* ɨ���������� */

        if ((touch_dev.sta & 0xc000) == TP_CATH_PRES)   /* ����������һ��(��ʱ�����ɿ���.) */
        {
            outtime = 0;
            touch_dev.sta &= ~TP_CATH_PRES;    /* ��ǰ����Ѿ����������. */

            pxy[cnt][0] = touch_dev.x[0];      /* ����X�������� */
            pxy[cnt][1] = touch_dev.y[0];      /* ����Y�������� */
            cnt++;

            switch (cnt)
            {
                case 1:
                    touch_drawPoint(20, 20, WHITE);                 /* �����1 */
                    touch_drawPoint(width - 20, 20, RED);    /* ����2 */
                    break;

                case 2:
                    touch_drawPoint(width - 20, 20, WHITE);  /* �����2 */
                    touch_drawPoint(20, length - 20, RED);   /* ����3 */
                    break;

                case 3:
                    touch_drawPoint(20, length - 20, WHITE); /* �����3 */
                    touch_drawPoint(width - 20, length - 20, RED);    /* ����4 */
                    break;

                case 4:
                    LCD_clear(WHITE);   /* �����������, ֱ������ */
                    touch_drawPoint(width / 2, length / 2, RED);  /* ����5 */
                    break;

                case 5:     /* ȫ��5�����Ѿ��õ� */
                    s1 = pxy[1][0] - pxy[0][0]; /* ��2����͵�1�����X�����������ֵ(ADֵ) */
                    s3 = pxy[3][0] - pxy[2][0]; /* ��4����͵�3�����X�����������ֵ(ADֵ) */
                    s2 = pxy[3][1] - pxy[1][1]; /* ��4����͵�2�����Y�����������ֵ(ADֵ) */
                    s4 = pxy[2][1] - pxy[0][1]; /* ��3����͵�1�����Y�����������ֵ(ADֵ) */
                    LCD_showNum(160, 0, 16, RED, WHITE, s1);
                    LCD_showNum(160, 17, 16, RED, WHITE, s2);
                    LCD_showNum(160, 34, 16, RED, WHITE, s3);
                    LCD_showNum(160, 52, 16, RED, WHITE, s4);
                    px = (double)s1 / s3;       /* X��������� */
                    py = (double)s2 / s4;       /* Y��������� */
                    LCD_showNum(160, 100, 16, RED, WHITE, (double)px);
                    LCD_showNum(160, 120, 16, RED, WHITE, py);
                    if (px < 0)px = -px;        /* ���������� */
                    if (py < 0)py = -py;        /* ���������� */

                    if (px < 0.95 || px > 1.05 || py < 0.95 || py > 1.05 ||     /* �������ϸ� */
                            abs(s1) > 4095 || abs(s2) > 4095 || abs(s3) > 4095 || abs(s4) > 4095 || /* ��ֵ���ϸ�, �������귶Χ */
                            abs(s1) == 0 || abs(s2) == 0 || abs(s3) == 0 || abs(s4) == 0            /* ��ֵ���ϸ�, ����0 */
                       )
                    {
                        cnt = 0;
                        touch_drawPoint(width / 2, length / 2, WHITE); /* �����5 */
                        touch_drawPoint(20, 20, RED);   /* ���»���1 */
                        touch_adjustInfoShow(pxy, px, py);   /* ��ʾ��ǰ��Ϣ,���������� */
                        continue;
                    }

                    touch_dev.xfac = (float)(s1 + s3) / (2 * (width - 40));
                    touch_dev.yfac = (float)(s2 + s4) / (2 * (length - 40));

                    touch_dev.xc = pxy[4][0];      /* X��,������������ */
                    touch_dev.yc = pxy[4][1];      /* Y��,������������ */

                    LCD_clear(WHITE);   /* ���� */
                    LCD_showString(35, 110, 16, BLUE, WHITE, "Touch Screen Adjust OK!"); /* У����� */
                    delay_ms(1000);
                    touch_saveAdjustData();

                    LCD_clear(WHITE);/* ���� */
                    return;/* У����� */
            }
        }

        delay_ms(10);
        outtime++;

        if (outtime > 1000)
        {
            touch_getAdjustData();
            break;
        }
    }
}

/**
 * @brief       ����У׼����
 *   @note      ����������EEPROMоƬ����(24C02),��ʼ��ַΪTP_SAVE_ADDR_BASE.
 *              ռ�ô�СΪ13�ֽ�
 * @param       ��
 * @retval      ��
 */
void touch_saveAdjustData(void)
{
    uint8_t *p = (uint8_t *)&touch_dev.xfac;   /* ָ���׵�ַ */

    /* pָ��tp_dev.xfac�ĵ�ַ, p+4����tp_dev.yfac�ĵ�ַ
     * p+8����tp_dev.xoff�ĵ�ַ,p+10,����tp_dev.yoff�ĵ�ַ
     * �ܹ�ռ��12���ֽ�(4������)
     * p+12���ڴ�ű�ǵ��败�����Ƿ�У׼������(0X0A)
     * ��p[12]д��0X0A. ����Ѿ�У׼��.
     */
    AT24C02_write(TP_SAVE_ADDR_BASE, p, 12);                /* ����12���ֽ�����(xfac,yfac,xc,yc) */
    AT24C02_writeByte(TP_SAVE_ADDR_BASE + 12, 0X0A);   /* ����У׼ֵ */
}

/**
 * @brief       ��ȡ������EEPROM�����У׼ֵ
 * @param       ��
 * @retval      0����ȡʧ�ܣ�Ҫ����У׼
 *              1���ɹ���ȡ����
 */
uint8_t touch_getAdjustData(void)
{
    uint8_t *p = (uint8_t *)&touch_dev.xfac;
    uint8_t temp = 0;

    /* ����������ֱ��ָ��tp_dev.xfac��ַ���б����, ��ȡ��ʱ��,����ȡ����������
     * д��ָ��tp_dev.xfac���׵�ַ, �Ϳ��Ի�ԭд���ȥ��ֵ, ������Ҫ���������
     * ������. �˷��������ڸ�������(�����ṹ��)�ı���/��ȡ(�����ṹ��).
     */
    AT24C02_read(TP_SAVE_ADDR_BASE, p, 12);                 /* ��ȡ12�ֽ����� */
    temp = AT24C02_receiveByte(TP_SAVE_ADDR_BASE + 12);   /* ��ȡУ׼״̬��� */
    if (temp == 0X0A)
    {
        return 1;
    }
    return 0;
}

/**
 * @brief       ��ʾУ׼���(��������)
 * @param       xy[5][2]: 5����������ֵ
 * @param       px,py   : x,y����ı�������(Լ�ӽ�1Խ��)
 * @retval      ��
 */
void touch_adjustInfoShow(uint16_t xy[5][2], double px, double py)
{
    uint8_t i;
    char sbuf[20];

    for (i = 0; i < 5; i++)   /* ��ʾ5����������ֵ */
    {
        sprintf(sbuf, "x%d:%d", i + 1, xy[i][0]);
        LCD_showString(40, 160 + (i * 20), 16, RED, WHITE, sbuf);
        sprintf(sbuf, "y%d:%d", i + 1, xy[i][1]);
        LCD_showString(40 + 80, 160 + (i * 20), 16, RED, WHITE, sbuf);
    }

    /* ��ʾX/Y����ı������� */
    LCD_fillRectangle(40, 160 + (i * 20), width - 1, 16, WHITE);  /* ���֮ǰ��px,py��ʾ */
    sprintf(sbuf, "px:%0.2f", px);
    sbuf[7] = 0; /* ��ӽ����� */
    LCD_showString(40, 160 + (i * 20), 16, RED, WHITE, sbuf);
    sprintf(sbuf, "py:%0.2f", py);
    sbuf[7] = 0; /* ��ӽ����� */
    LCD_showString(40 + 80, 160 + (i * 20), 16, RED, WHITE, sbuf);
}
