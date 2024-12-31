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
 * @brief       触摸屏初始化
 * @param       无
 * @retval      0,没有进行校准
 *              1,进行过校准
 */
uint8_t touchInit(void)
{
    touch_dev.touchtype = 0;                   /* 默认设置(电阻屏 & 竖屏) */
    touch_dev.touchtype |= 0 & 0x01;  /* 根据LCD判定是横屏还是竖屏 */

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
 * @brief       SPI写数据
 *   @note      向触摸屏IC写入1 byte数据
 * @param       data: 要写入的数据
 * @retval      无
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
 * @brief       SPI读数据
 *   @note      从触摸屏IC读取adc值
 * @param       cmd: 指令
 * @retval      读取到的数据,ADC值(12bit)
 */
uint16_t touch_readADC(uint8_t cmd)
{
    uint16_t data = 0;
    TOUCH_SCK(0);
    TOUCH_MOSI(0);
    TOUCH_CS(0);
    touch_writeByte(cmd);
    delay_us(6);//adc转换最多需要6us
    TOUCH_SCK(0);//产生一个时钟清除busy位
    delay_us(1);
    TOUCH_SCK(1);
    delay_us(1);
    TOUCH_SCK(0);
    for(uint8_t i = 0; i < 16; i++)
    {
        data <<= 1;
        TOUCH_SCK(0);//下降沿有效
        delay_us(1);
        TOUCH_SCK(1);
        if(GPIO_ReadInputDataBit(TOUCH_MISO_PORT, TOUCH_MISO_PIN))
        {
            data++;
        }
    }
    data >>= 4;//ADC12位有效
    TOUCH_CS(1);
    return data;
}

/**
 * @brief       读取一个坐标值(x或者y)
 *   @note      连续读取5次数据,对这些数据升序排列,
 *              然后去掉最低和最高, 取平均值
 *
 * @param       cmd : 指令
 *   @arg       0XD0: 读取X轴坐标(@竖屏状态,横屏状态和Y对调.)
 *   @arg       0X90: 读取Y轴坐标(@竖屏状态,横屏状态和X对调.)
 *
 * @retval      读取到的数据(滤波后的), ADC值(12bit)
 */
uint16_t touch_readXOY(uint8_t cmd)
{
    uint16_t temp = 0;
    uint16_t sum = 0;
    uint16_t data = 0;
    uint16_t buf[TP_READ_TIMES];
    for(uint8_t i = 0;i < TP_READ_TIMES;i++)//获取5次数据
    {
        buf[i] = touch_readADC(cmd);
    }
    for(uint8_t i = 0;i < TP_READ_TIMES - 1;i++)//进行排序:升序
    {
        for(uint8_t j = i + 1;j < TP_READ_TIMES;j++)//每一轮的buf[i]都是本轮次最小的数
        {
            if(buf[i] > buf[j])
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    for (uint16_t i = TP_LOST_VAL; i < TP_READ_TIMES - TP_LOST_VAL; i++)   /* 去掉两端的丢弃值 */
    {
        sum += buf[i];  /* 累加去掉丢弃值以后的数据. */
    }
    data = sum / (TP_READ_TIMES - 2 * TP_LOST_VAL);
    return data;
}

/**
 * @brief       读取x, y坐标
 * @param       x,y: 读取到的坐标值
 * @retval      无
 */
void touch_readXY(uint16_t *x,uint16_t *y)
{
    *x = touch_readXOY(0xD0);
    *y = touch_readXOY(0x90);
}

/**
 * @brief       连续读取2次触摸IC数据, 并滤波
 *   @note      连续2次读取触摸屏IC,且这两次的偏差不能超过ERR_RANGE,满足
 *              条件,则认为读数正确,否则读数错误.该函数能大大提高准确度.
 *
 * @param       x,y: 读取到的坐标值
 * @retval      0, 失败; 1, 成功;
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
 * @brief       画一个校准用的触摸点(十字架)
 * @param       x,y   : 坐标
 * @param       color : 颜色
 * @retval      无
 */
void touch_drawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_drawHorizontalLine(x - 12, y, color, 25); /* 横线 */
    LCD_drawVerticalLine(x, y - 12, color, 25); /* 竖线 */
    LCD_drawPoint(x + 1, y + 1, color);
    LCD_drawPoint(x - 1, y + 1, color);
    LCD_drawPoint(x + 1, y - 1, color);
    LCD_drawPoint(x - 1, y - 1, color);
    LCD_writeCircle(x, y, color, 6);            /* 画中心圈 */
}

/**
 * @brief       画一个大点(2*2的点)
 * @param       x,y   : 坐标
 * @param       color : 颜色
 * @retval      无
 */
void touch_drawBigPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_drawPoint(x, y, color);       /* 中心点 */
    LCD_drawPoint(x + 1, y, color);
    LCD_drawPoint(x, y + 1, color);
    LCD_drawPoint(x + 1, y + 1, color);
}

/**
 * @brief       触摸按键扫描
 * @param       mode: 坐标模式
 *   @arg       0, 屏幕坐标;
 *   @arg       1, 物理坐标(校准等特殊场合用)
 *
 * @retval      0, 触屏无触摸; 1, 触屏有触摸;
 */
uint8_t touchScan(uint8_t mode)
{
    if (GPIO_ReadInputDataBit(TOUCH_PEN_PORT,TOUCH_PEN_PIN) == 0)     /* 有按键按下 */
    {
        if (mode)       /* 读取物理坐标, 无需转换 */
        {
            touch_readXY2(&touch_dev.x[0], &touch_dev.y[0]);
        }
        else if (touch_readXY2(&touch_dev.x[0], &touch_dev.y[0]))     /* 读取屏幕坐标, 需要转换 */
        {
            /* 将X轴 物理坐标转换成逻辑坐标(即对应LCD屏幕上面的X坐标值) */
            touch_dev.x[0] = (signed short)(touch_dev.x[0] - touch_dev.xc) / touch_dev.xfac + width / 2;

            /* 将Y轴 物理坐标转换成逻辑坐标(即对应LCD屏幕上面的Y坐标值) */
            touch_dev.y[0] = (signed short)(touch_dev.y[0] - touch_dev.yc) / touch_dev.yfac + length / 2;
        }

        if ((touch_dev.sta & TP_PRES_DOWN) == 0)   /* 之前没有被按下 */
        {
            touch_dev.sta = TP_PRES_DOWN | TP_CATH_PRES;   /* 按键按下 */
            touch_dev.x[CT_MAX_TOUCH - 1] = touch_dev.x[0];   /* 记录第一次按下时的坐标 */
            touch_dev.y[CT_MAX_TOUCH - 1] = touch_dev.y[0];
        }
    }
    else
    {
        if (touch_dev.sta & TP_PRES_DOWN)      /* 之前是被按下的 */
        {
            touch_dev.sta &= ~TP_PRES_DOWN;    /* 标记按键松开 */
        }
        else     /* 之前就没有被按下 */
        {
            touch_dev.x[CT_MAX_TOUCH - 1] = 0;
            touch_dev.y[CT_MAX_TOUCH - 1] = 0;
            touch_dev.x[0] = 0xFFFF;
            touch_dev.y[0] = 0xFFFF;
        }
    }

    return touch_dev.sta & TP_PRES_DOWN; /* 返回当前的触屏状态 */
}

char *const TP_REMIND_MSG_TBL = "Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";

/**
 * @brief       触摸屏校准代码
 *   @note      使用五点校准法(具体原理请百度)
 *              本函数得到x轴/y轴比例因子xfac/yfac及物理中心坐标值(xc,yc)等4个参数
 *              我们规定: 物理坐标即AD采集到的坐标值,范围是0~4095.
 *                        逻辑坐标即LCD屏幕的坐标, 范围为LCD屏幕的分辨率.
 *
 * @param       无
 * @retval      无
 */
void touchAdjust(void)
{
    uint16_t pxy[5][2];     /* 物理坐标缓存值 */
    uint8_t  cnt = 0;
    short s1, s2, s3, s4;   /* 4个点的坐标差值 */
    double px, py;          /* X,Y轴物理坐标比例,用于判定是否校准成功 */
    uint16_t outtime = 0;
    cnt = 0;

    LCD_clear(WHITE);       /* 清屏 */
    //LCD_showString(40, 40, 16, RED, WHITE, TP_REMIND_MSG_TBL); /* 显示提示信息 */
    touch_drawPoint(20, 20, RED);   /* 画点1 */
    touch_dev.sta = 0;         /* 消除触发信号 */

    while (1)               /* 如果连续10秒钟没有按下,则自动退出 */
    {
        touch_dev.scan(1);     /* 扫描物理坐标 */

        if ((touch_dev.sta & 0xc000) == TP_CATH_PRES)   /* 按键按下了一次(此时按键松开了.) */
        {
            outtime = 0;
            touch_dev.sta &= ~TP_CATH_PRES;    /* 标记按键已经被处理过了. */

            pxy[cnt][0] = touch_dev.x[0];      /* 保存X物理坐标 */
            pxy[cnt][1] = touch_dev.y[0];      /* 保存Y物理坐标 */
            cnt++;

            switch (cnt)
            {
                case 1:
                    touch_drawPoint(20, 20, WHITE);                 /* 清除点1 */
                    touch_drawPoint(width - 20, 20, RED);    /* 画点2 */
                    break;

                case 2:
                    touch_drawPoint(width - 20, 20, WHITE);  /* 清除点2 */
                    touch_drawPoint(20, length - 20, RED);   /* 画点3 */
                    break;

                case 3:
                    touch_drawPoint(20, length - 20, WHITE); /* 清除点3 */
                    touch_drawPoint(width - 20, length - 20, RED);    /* 画点4 */
                    break;

                case 4:
                    LCD_clear(WHITE);   /* 画第五个点了, 直接清屏 */
                    touch_drawPoint(width / 2, length / 2, RED);  /* 画点5 */
                    break;

                case 5:     /* 全部5个点已经得到 */
                    s1 = pxy[1][0] - pxy[0][0]; /* 第2个点和第1个点的X轴物理坐标差值(AD值) */
                    s3 = pxy[3][0] - pxy[2][0]; /* 第4个点和第3个点的X轴物理坐标差值(AD值) */
                    s2 = pxy[3][1] - pxy[1][1]; /* 第4个点和第2个点的Y轴物理坐标差值(AD值) */
                    s4 = pxy[2][1] - pxy[0][1]; /* 第3个点和第1个点的Y轴物理坐标差值(AD值) */
                    LCD_showNum(160, 0, 16, RED, WHITE, s1);
                    LCD_showNum(160, 17, 16, RED, WHITE, s2);
                    LCD_showNum(160, 34, 16, RED, WHITE, s3);
                    LCD_showNum(160, 52, 16, RED, WHITE, s4);
                    px = (double)s1 / s3;       /* X轴比例因子 */
                    py = (double)s2 / s4;       /* Y轴比例因子 */
                    LCD_showNum(160, 100, 16, RED, WHITE, (double)px);
                    LCD_showNum(160, 120, 16, RED, WHITE, py);
                    if (px < 0)px = -px;        /* 负数改正数 */
                    if (py < 0)py = -py;        /* 负数改正数 */

                    if (px < 0.95 || px > 1.05 || py < 0.95 || py > 1.05 ||     /* 比例不合格 */
                            abs(s1) > 4095 || abs(s2) > 4095 || abs(s3) > 4095 || abs(s4) > 4095 || /* 差值不合格, 大于坐标范围 */
                            abs(s1) == 0 || abs(s2) == 0 || abs(s3) == 0 || abs(s4) == 0            /* 差值不合格, 等于0 */
                       )
                    {
                        cnt = 0;
                        touch_drawPoint(width / 2, length / 2, WHITE); /* 清除点5 */
                        touch_drawPoint(20, 20, RED);   /* 重新画点1 */
                        touch_adjustInfoShow(pxy, px, py);   /* 显示当前信息,方便找问题 */
                        continue;
                    }

                    touch_dev.xfac = (float)(s1 + s3) / (2 * (width - 40));
                    touch_dev.yfac = (float)(s2 + s4) / (2 * (length - 40));

                    touch_dev.xc = pxy[4][0];      /* X轴,物理中心坐标 */
                    touch_dev.yc = pxy[4][1];      /* Y轴,物理中心坐标 */

                    LCD_clear(WHITE);   /* 清屏 */
                    LCD_showString(35, 110, 16, BLUE, WHITE, "Touch Screen Adjust OK!"); /* 校正完成 */
                    delay_ms(1000);
                    touch_saveAdjustData();

                    LCD_clear(WHITE);/* 清屏 */
                    return;/* 校正完成 */
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
 * @brief       保存校准参数
 *   @note      参数保存在EEPROM芯片里面(24C02),起始地址为TP_SAVE_ADDR_BASE.
 *              占用大小为13字节
 * @param       无
 * @retval      无
 */
void touch_saveAdjustData(void)
{
    uint8_t *p = (uint8_t *)&touch_dev.xfac;   /* 指向首地址 */

    /* p指向tp_dev.xfac的地址, p+4则是tp_dev.yfac的地址
     * p+8则是tp_dev.xoff的地址,p+10,则是tp_dev.yoff的地址
     * 总共占用12个字节(4个参数)
     * p+12用于存放标记电阻触摸屏是否校准的数据(0X0A)
     * 往p[12]写入0X0A. 标记已经校准过.
     */
    AT24C02_write(TP_SAVE_ADDR_BASE, p, 12);                /* 保存12个字节数据(xfac,yfac,xc,yc) */
    AT24C02_writeByte(TP_SAVE_ADDR_BASE + 12, 0X0A);   /* 保存校准值 */
}

/**
 * @brief       获取保存在EEPROM里面的校准值
 * @param       无
 * @retval      0，获取失败，要重新校准
 *              1，成功获取数据
 */
uint8_t touch_getAdjustData(void)
{
    uint8_t *p = (uint8_t *)&touch_dev.xfac;
    uint8_t temp = 0;

    /* 由于我们是直接指向tp_dev.xfac地址进行保存的, 读取的时候,将读取出来的数据
     * 写入指向tp_dev.xfac的首地址, 就可以还原写入进去的值, 而不需要理会具体的数
     * 据类型. 此方法适用于各种数据(包括结构体)的保存/读取(包括结构体).
     */
    AT24C02_read(TP_SAVE_ADDR_BASE, p, 12);                 /* 读取12字节数据 */
    temp = AT24C02_receiveByte(TP_SAVE_ADDR_BASE + 12);   /* 读取校准状态标记 */
    if (temp == 0X0A)
    {
        return 1;
    }
    return 0;
}

/**
 * @brief       提示校准结果(各个参数)
 * @param       xy[5][2]: 5个物理坐标值
 * @param       px,py   : x,y方向的比例因子(约接近1越好)
 * @retval      无
 */
void touch_adjustInfoShow(uint16_t xy[5][2], double px, double py)
{
    uint8_t i;
    char sbuf[20];

    for (i = 0; i < 5; i++)   /* 显示5个物理坐标值 */
    {
        sprintf(sbuf, "x%d:%d", i + 1, xy[i][0]);
        LCD_showString(40, 160 + (i * 20), 16, RED, WHITE, sbuf);
        sprintf(sbuf, "y%d:%d", i + 1, xy[i][1]);
        LCD_showString(40 + 80, 160 + (i * 20), 16, RED, WHITE, sbuf);
    }

    /* 显示X/Y方向的比例因子 */
    LCD_fillRectangle(40, 160 + (i * 20), width - 1, 16, WHITE);  /* 清除之前的px,py显示 */
    sprintf(sbuf, "px:%0.2f", px);
    sbuf[7] = 0; /* 添加结束符 */
    LCD_showString(40, 160 + (i * 20), 16, RED, WHITE, sbuf);
    sprintf(sbuf, "py:%0.2f", py);
    sbuf[7] = 0; /* 添加结束符 */
    LCD_showString(40 + 80, 160 + (i * 20), 16, RED, WHITE, sbuf);
}
