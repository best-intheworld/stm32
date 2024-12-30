#include "lcd.h"
#include "lcdfont.h"
#include <stm32f1xx_ll_fsmc.h>
#include <stm32f1xx_hal_sram.h>
//�������ź궨��
#define LCD_BL_Pin  GPIO_PIN_0
#define LCD_CS_Pin  GPIO_PIN_12
#define LCD_RS_Pin  GPIO_PIN_0
#define LCD_WR_Pin  GPIO_PIN_5
#define LCD_RD_Pin  GPIO_PIN_4
//�������ź궨��
#define LCD_Data_Port1 GPIOD
#define LCD_Data0_Pin  GPIO_PIN_14
#define LCD_Data1_Pin  GPIO_PIN_15
#define LCD_Data2_Pin  GPIO_PIN_0
#define LCD_Data3_Pin  GPIO_PIN_1
#define LCD_Data13_Pin GPIO_PIN_8
#define LCD_Data14_Pin GPIO_PIN_9
#define LCD_Data15_Pin GPIO_PIN_10

#define LCD_Data_Port2  GPIOE
#define LCD_Data4_Pin   GPIO_PIN_7
#define LCD_Data5_Pin   GPIO_PIN_8
#define LCD_Data6_Pin   GPIO_PIN_9
#define LCD_Data7_Pin   GPIO_PIN_10
#define LCD_Data8_Pin   GPIO_PIN_11
#define LCD_Data9_Pin   GPIO_PIN_12
#define LCD_Data10_Pin  GPIO_PIN_13
#define LCD_Data11_Pin  GPIO_PIN_14
#define LCD_Data12_Pin  GPIO_PIN_15



SRAM_HandleTypeDef SRAM_InitStruct;

typedef struct
{
    volatile uint16_t LCD_Reg;
    volatile uint16_t LCD_Ram;
} LCD_TypeDef;

/*����LCD_Base˵�����£�
 *0x6000 0000:��Ϊ�õ���sram����ѡ����bank1��bank1����ʼ��ַ�������
 *0x400 0000����16������Ϊ64MB��bank1��Ϊ4�飬ÿһ��64MB
 *3��Ӳ������NE4��Ҳ����ѡ���˵��Ŀ飬����Ϊ��������Ŀ����ʼ��ַ
 *1 << 10:���1������fsmc����lcd_rs����һλ��1����Ϊ0ʱ�������1ʱ�������ݣ�������ȷ��ram����ʼ��ַ������Ӧ����1.
 *��һ��2����Ϊʹ����16λ������ģʽfsmc��[24:0],���ǵ�ַ����[25:1],����Ҫ����һλ
 *�ڶ���2����Ϊ�ڽṹ����ֻ��ram�ĵ�ַ���ܹ�������ģ�����Ҫͨ��ram����������ģ�reg��ramǰ�棬16λ�����ֽڣ���ȥ�͵õ�lcd�ṹ���ַ
*/

#define LCD_BASE        (uint32_t)((0X60000000 + (0X4000000 * (4 - 1))) | (((1 << 10) * 2) -2))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

/**
 * @brief       lcdд�Ĵ������
 * @param       Reg���Ĵ������
 * @retval      ��
 */
void LCD_writeRegNum(uint16_t Reg)
{
    LCD->LCD_Reg = Reg;
}

/**
 * @brief       lcd������ʾ
 * @param       ��
 * @retval      ��
 */
void LCD_displayOn(void)
{
    LCD_writeRegNum(0x29);
}

/**
 * @brief       lcd�ر���ʾ
 * @param       ��
 * @retval      ��
 */
void LCD_displayOff(void)
{
    LCD_writeRegNum(0x28);
}

/**
 * @brief       lcdд����
 * @param       Data:����
 * @retval      ��
 */
void LCD_writeData(uint16_t Data)
{
    LCD->LCD_Ram = Data;
}

/**
 * @brief       lcdд�Ĵ���
 * @param       Data:����
 * @retval      ��
 */
void LCD_writeReg(uint16_t Reg,uint16_t Data)
{
    LCD->LCD_Reg = Reg;
    LCD->LCD_Ram = Data;
}

/**
 * @brief       lcd��������
 * @param       x:������ y:������
 * @retval      ��
 */
void LCD_setCursor(uint16_t x,uint16_t y)
{
    LCD_writeRegNum(setXCmd);
    LCD_writeData(x >> 8);
    LCD_writeData(x & 0XFF);
    LCD_writeRegNum(setYCmd);
    LCD_writeData(y >> 8);
    LCD_writeData(y & 0XFF);
}

/**
 * @brief       lcd׼��дram
 * @param       ��
 * @retval      ��
 */
void LCD_prepareWriteRam(void)
{
    LCD->LCD_Reg = writeRamCmd;
}

/**
 * @brief       ����
 * @param       x,y: ����
 * @param       color: �����ɫ(32λ��ɫ,�������LTDC)
 * @retval      ��
 */
void LCD_drawPoint(uint16_t x,uint16_t y,uint32_t color)
{
    LCD_setCursor(x,y);
    LCD_prepareWriteRam();
    LCD->LCD_Ram = color;
}

/**
 * @brief       LCD������
 * @param       ��
 * @retval      ��ȡ��������
 */
uint16_t LCD_readData(void)
{
    uint16_t data = 0;
    data = LCD->LCD_Ram;
    return data;
}

/**
 * @brief       ��ȡ��ĳ�����ɫֵ
 * @param       x,y:����
 * @retval      �˵����ɫ(32λ��ɫ,�������LTDC)
 */
uint32_t LCD_readPoint(uint16_t x,uint16_t y)
{
    uint16_t r = 0,g = 0,b = 0;
    if((x >= width) | (y >= length))return 0;
    LCD_writeRegNum(0x2E);
    r = LCD_readData();//dummy �ٶ�
    r = LCD_readData();//���²��ֲο�����ָ�ϣ��ܺ���⣬��Ҫ����Ϊһ�ζ�ȡ��������������˲�ͬ��ɫ������
    b = LCD_readData();
    g = r & 0XFF;
    g <<= 8;
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));
}

/**
 * @brief       ��������
 * @param       color: Ҫ��������ɫ
 * @retval      ��
 */
void LCD_clear(uint32_t color)
{
    uint32_t totalPoint = width * length;//ȫ����
    LCD_setCursor(0,0);
    LCD_prepareWriteRam();
    for(int i = 0;i <= totalPoint;i++)
    {
        LCD->LCD_Ram = color;
    }
}

/**
 * @brief       ƽ������, m^n
 * @param       m: ����
 * @param       n: ָ��
 * @retval      m��n�η�
 */
uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)result *= m;

    return result;
}

/**
 * @brief       ��ָ��λ����ʾһ���ַ�
 * @param       x,y  : ����
 * @param       chr  : Ҫ��ʾ���ַ�:" "--->"~"
 * @param       size : �����С 12/16/24/32
 * @param       mode : ���ӷ�ʽ(1); �ǵ��ӷ�ʽ(0);
 * @param       color : �ַ�����ɫ;
 * @param       backColor:��ǰ������ɫ
 * @retval      ��
 */
void LCD_showChar(uint16_t x,uint16_t y,char chr,uint8_t size,uint8_t mode, uint32_t color,uint32_t backColor)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* �õ�����һ���ַ���Ӧ������ռ���ֽ��� */
    chr = chr - ' ';    /* �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩ */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[chr];  /* ����1206���� */
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[chr];  /* ����1608���� */
            break;

        case 24:
            pfont = (uint8_t *)asc2_2412[chr];  /* ����2412���� */
            break;

        case 32:
            pfont = (uint8_t *)asc2_3216[chr];  /* ����3216���� */
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];    /* ��ȡ�ַ��ĵ������� */

        for (t1 = 0; t1 < 8; t1++)   /* һ���ֽ�8���� */
        {
            if (temp & 0x80)        /* ��Ч��,��Ҫ��ʾ */
            {
                LCD_drawPoint(x, y, color);        /* �������,Ҫ��ʾ����� */
            }
            else if (mode == 0)     /* ��Ч��,����ʾ */
            {
                LCD_drawPoint(x, y, backColor); /* ������ɫ,�൱������㲻��ʾ(ע�ⱳ��ɫ��ȫ�ֱ�������) */
            }

            temp <<= 1; /* ��λ, �Ա��ȡ��һ��λ��״̬ */
            y++;

            if (y >= length)return;  /* �������� */

            if ((y - y0) == size)   /* ��ʾ��һ����? */
            {
                y = y0; /* y���긴λ */
                x++;    /* x������� */

                if (x >= width)return;   /* x���곬������ */

                break;
            }
        }
    }
}

/**
 * @brief       ��ʾlen������
 * @param       x,y : ��ʼ����
 * @param       num : ��ֵ(0 ~ 2^32)
 * @param       len : ��ʾ���ֵ�λ��
 * @param       size: ѡ������ 12/16/24/32
 * @param       color : ���ֵ���ɫ;
 * @param       backColor   :��ǰ������ɫ
 * @retval      ��
 */
void LCD_showNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color,uint32_t backColor)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* ������ʾλ��ѭ�� */
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;   /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))   /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                LCD_showChar(x + (size / 2)*t, y, ' ', size, 0, color,backColor);/* ��ʾ�ո�,ռλ */
                continue;   /* �����¸�һλ */
            }
            else
            {
                enshow = 1; /* ʹ����ʾ */
            }

        }

        LCD_showChar(x + (size / 2)*t, y, temp + '0', size, 0, color,backColor); /* ��ʾ�ַ� */
    }
}

/**
 * @brief       ��ʾ�ַ���
 * @param       x,y         : ��ʼ����
 * @param       Width,height: �����С
 * @param       size        : ѡ������ 12/16/24/32
 * @param       p           : �ַ����׵�ַ
 * @param       color       : �ַ�������ɫ;
 * @param       backColor   :��ǰ������ɫ
 * @retval      ��
 */
void LCD_showString(uint16_t x, uint16_t y, uint16_t Width, uint16_t height, uint8_t size, char *p, uint16_t color,uint32_t backColor)
{
    uint8_t x0 = x;
    Width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   /* �ж��ǲ��ǷǷ��ַ�! */
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)break;  /* �˳� */

        LCD_showChar(x, y, *p, size, 0, color,backColor);
        x += size / 2;
        p++;
    }
}


/**
 * @brief       lcd��ʼ������
 * @param       ��
 * @retval      ��
 */
void LCD_Init(void)
{
    //����FSMC��GPIOʱ��
    __HAL_RCC_FSMC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    //�������ų�ʼ��
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin=LCD_Data0_Pin | LCD_Data1_Pin | LCD_Data2_Pin | LCD_Data3_Pin | LCD_Data13_Pin | LCD_Data14_Pin | LCD_Data15_Pin;
    GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;//�����������
    GPIO_InitStruct.Pull=GPIO_PULLUP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_Data_Port1,&GPIO_InitStruct);

    GPIO_InitStruct.Pin=LCD_Data4_Pin | LCD_Data5_Pin | LCD_Data6_Pin | LCD_Data7_Pin | LCD_Data8_Pin | LCD_Data9_Pin | LCD_Data10_Pin | LCD_Data11_Pin | LCD_Data12_Pin;
    GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;//�����������
    GPIO_InitStruct.Pull=GPIO_PULLUP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_Data_Port2,&GPIO_InitStruct);
    //�������ų�ʼ��
    GPIO_InitStruct.Pin=LCD_BL_Pin;
    GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;//ͨ���������
    GPIO_InitStruct.Pull=GPIO_PULLUP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);

    GPIO_InitStruct.Pin=LCD_CS_Pin;
    GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull=GPIO_PULLUP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOG,&GPIO_InitStruct);
    
    GPIO_InitStruct.Pin=LCD_RS_Pin;
    GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull=GPIO_PULLUP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOG,&GPIO_InitStruct);

    GPIO_InitStruct.Pin=LCD_WR_Pin;
    GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull=GPIO_PULLUP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);

    GPIO_InitStruct.Pin=LCD_RD_Pin;
    GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull=GPIO_PULLUP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);
    //FSMC��SRAM���ã�LCD��Sramʱ��������Խ�LCD����SRAM����
    //SRAM_HandleTypeDef SRAM_InitStruct;
    SRAM_InitStruct.Instance=FSMC_NORSRAM_DEVICE;
    SRAM_InitStruct.Extended=FSMC_NORSRAM_EXTENDED_DEVICE;
    SRAM_InitStruct.Init.NSBank = FSMC_NORSRAM_BANK4;                        /* ʹ��NE4 */
    SRAM_InitStruct.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;     /* ��ַ/�����߲����� */
    SRAM_InitStruct.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;    /* 16λ���ݿ�� */
    SRAM_InitStruct.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;   /* �Ƿ�ʹ��ͻ������,����ͬ��ͻ���洢����Ч,�˴�δ�õ� */
    SRAM_InitStruct.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW; /* �ȴ��źŵļ���,����ͻ��ģʽ���������� */
    SRAM_InitStruct.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;      /* �洢�����ڵȴ�����֮ǰ��һ��ʱ�����ڻ��ǵȴ������ڼ�ʹ��NWAIT */
    SRAM_InitStruct.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;       /* �洢��дʹ�� */
    SRAM_InitStruct.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;              /* �ȴ�ʹ��λ,�˴�δ�õ� */
    SRAM_InitStruct.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;           /* ��дʹ�ò�ͬ��ʱ�� */
    SRAM_InitStruct.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;  /* �Ƿ�ʹ��ͬ������ģʽ�µĵȴ��ź�,�˴�δ�õ� */
    SRAM_InitStruct.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;              /* ��ֹͻ��д */

    FSMC_NORSRAM_TimingTypeDef Read_Handle_InitStruct;
    Read_Handle_InitStruct.AccessMode=FSMC_ACCESS_MODE_A;
    Read_Handle_InitStruct.AddressHoldTime=0;
    Read_Handle_InitStruct.AddressSetupTime=0;
    Read_Handle_InitStruct.DataSetupTime=15;

    FSMC_NORSRAM_TimingTypeDef Write_Handle_InitStruct;
    Write_Handle_InitStruct.AccessMode=FSMC_ACCESS_MODE_A;
    Write_Handle_InitStruct.AddressHoldTime=0;
    Write_Handle_InitStruct.AddressSetupTime=0;
    Write_Handle_InitStruct.DataSetupTime=1;
    HAL_SRAM_Init(&SRAM_InitStruct,&Read_Handle_InitStruct,&Write_Handle_InitStruct);
    HAL_Delay(50);
    lcd_ex_st7789_reginit();
    //�򿪱���
    HAL_GPIO_WritePin(GPIOB,LCD_BL_Pin,SET);
    //����ɨ�跽��
    LCD_writeReg(0x36,0x08);
    LCD_clear(WHITE);
}

/**
 * @brief       ST7789 �Ĵ�����ʼ������
 * @param       ��
 * @retval      ��
 */
void lcd_ex_st7789_reginit(void)
{
    LCD_writeRegNum(0x11);

    HAL_Delay(120); 

    LCD_writeRegNum(0x36);
    LCD_writeData(0x00);


    LCD_writeRegNum(0x3A);
    LCD_writeData(0X05);

    LCD_writeRegNum(0xB2);
    LCD_writeData(0x0C);
    LCD_writeData(0x0C);
    LCD_writeData(0x00);
    LCD_writeData(0x33);
    LCD_writeData(0x33);

    LCD_writeRegNum(0xB7);
    LCD_writeData(0x35);

    LCD_writeRegNum(0xBB); /* vcom */
    LCD_writeData(0x32);  /* 30 */

    LCD_writeRegNum(0xC0);
    LCD_writeData(0x0C);

    LCD_writeRegNum(0xC2);
    LCD_writeData(0x01);

    LCD_writeRegNum(0xC3); /* vrh */
    LCD_writeData(0x10);  /* 17 0D */

    LCD_writeRegNum(0xC4); /* vdv */
    LCD_writeData(0x20);  /* 20 */

    LCD_writeRegNum(0xC6);
    LCD_writeData(0x0f);

    LCD_writeRegNum(0xD0);
    LCD_writeData(0xA4); 
    LCD_writeData(0xA1); 

    LCD_writeRegNum(0xE0); /* Set Gamma  */
    LCD_writeData(0xd0);
    LCD_writeData(0x00);
    LCD_writeData(0x02);
    LCD_writeData(0x07);
    LCD_writeData(0x0a);
    LCD_writeData(0x28);
    LCD_writeData(0x32);
    LCD_writeData(0X44);
    LCD_writeData(0x42);
    LCD_writeData(0x06);
    LCD_writeData(0x0e);
    LCD_writeData(0x12);
    LCD_writeData(0x14);
    LCD_writeData(0x17);


    LCD_writeRegNum(0XE1);  /* Set Gamma */
    LCD_writeData(0xd0);
    LCD_writeData(0x00);
    LCD_writeData(0x02);
    LCD_writeData(0x07);
    LCD_writeData(0x0a);
    LCD_writeData(0x28);
    LCD_writeData(0x31);
    LCD_writeData(0x54);
    LCD_writeData(0x47);
    LCD_writeData(0x0e);
    LCD_writeData(0x1c);
    LCD_writeData(0x17);
    LCD_writeData(0x1b); 
    LCD_writeData(0x1e);


    LCD_writeRegNum(0x2A);
    LCD_writeData(0x00);
    LCD_writeData(0x00);
    LCD_writeData(0x00);
    LCD_writeData(0xef);

    LCD_writeRegNum(0x2B);
    LCD_writeData(0x00);
    LCD_writeData(0x00);
    LCD_writeData(0x01);
    LCD_writeData(0x3f);

    LCD_writeRegNum(0x29); /* display on */
}

/**
 * @brief       ��ָ����������䵥����ɫ
 * @param       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex - sx + 1) * (ey - sy + 1)
 * @param       color:Ҫ������ɫ(32λ��ɫ,�������LTDC)
 * @retval      ��
 */
void LCD_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
    uint16_t i, j;
    uint16_t xlen = 0;
    xlen = ex - sx + 1;

    for (i = sy; i <= ey; i++)
    {
        LCD_setCursor(sx, i);      /* ���ù��λ�� */
        LCD_prepareWriteRam();    /* ��ʼд��GRAM */

        for (j = 0; j < xlen; j++)
        {
            LCD->LCD_Ram = color;   /* ��ʾ��ɫ */
        }
    }
}

/**
 * @brief       ����
 * @param       x1,y1: �������
 * @param       x2,y2: �յ�����
 * @param       color: �ߵ���ɫ
 * @retval      ��
 */
void LCD_drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    delta_x = x2 - x1;          /* ������������ */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)incx = 1;   /* ���õ������� */
    else if (delta_x == 0)incx = 0; /* ��ֱ�� */
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; /* ˮƽ�� */
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x;  /* ѡȡ�������������� */
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )   /* ������� */
    {
        LCD_drawPoint(row, col, color); /* ���� */
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief       ��Բ
 * @param       x,y  : Բ��������
 * @param       r    : �뾶
 * @param       color: Բ����ɫ
 * @retval      ��
 */
void LCD_drawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       /* �ж��¸���λ�õı�־ */

    while (a <= b)
    {
        LCD_drawPoint(x0 + a, y0 - b, color);  /* 5 */
        LCD_drawPoint(x0 + b, y0 - a, color);  /* 0 */
        LCD_drawPoint(x0 + b, y0 + a, color);  /* 4 */
        LCD_drawPoint(x0 + a, y0 + b, color);  /* 6 */
        LCD_drawPoint(x0 - a, y0 + b, color);  /* 1 */
        LCD_drawPoint(x0 - b, y0 + a, color);
        LCD_drawPoint(x0 - a, y0 - b, color);  /* 2 */
        LCD_drawPoint(x0 - b, y0 - a, color);  /* 7 */
        a++;

        /* ʹ��Bresenham�㷨��Բ */
        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/**
 * @brief       ��ˮƽ��
 * @param       x,y: �������
 * @param       len  : �߳���
 * @param       color: ���ε���ɫ
 * @retval      ��
 */
void LCD_drawHline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x > width) || (y > length))return;

    LCD_fill(x, y, x + len - 1, y, color);
}

/**
 * @brief       ���ʵ��Բ
 * @param       x,y: Բ��������
 * @param       r    : �뾶
 * @param       color: Բ����ɫ
 * @retval      ��
 */
void LCD_fillCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    uint32_t i;
    uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
    uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    uint32_t xr = r;

    LCD_drawHline(x - r, y, 2 * r, color);

    for (i = 1; i <= imax; i++)
    {
        if ((i * i + xr * xr) > sqmax)
        {
            /* draw lines from outside */
            if (xr > imax)
            {
                LCD_drawHline (x - i + 1, y + xr, 2 * (i - 1), color);
                LCD_drawHline (x - i + 1, y - xr, 2 * (i - 1), color);
            }

            xr--;
        }

        /* draw lines from inside (center) */
        LCD_drawHline(x - xr, y + i, 2 * xr, color);
        LCD_drawHline(x - xr, y - i, 2 * xr, color);
    }
}
