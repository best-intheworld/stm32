#include "lcd.h"
#include "ASCII_font.h"

LCD_dev lcddev;

typedef struct
{
    volatile uint16_t LCD_Reg;
    volatile uint16_t LCD_Ram;
} LCD_TypeDef;

#define LCD_BASE     (uint32_t)(((0x60000000 + 0x4000000 * (4 - 1)) | ((1 << 10) * 2)) - 2)
#define LCD         ((LCD_TypeDef *)LCD_BASE)

void LCD_DeInit(void)
{
    lcddev.dir = 0;//默认竖屏
    lcddev.id = 7789;
    lcddev.width = 240;
    lcddev.height = 320;
    lcddev.wramcmd = 0x2C;
    lcddev.setxcmd = 0x2A;
    lcddev.setycmd = 0x2B;
}

void LCD_writeCommand(uint16_t command)
{
    LCD->LCD_Reg = command;
}

void LCD_writeData(uint16_t data)
{
    LCD->LCD_Ram = data;
}

//适用于发送指令后只发送一条数据的情况
void LCD_writeCommandAndData(uint16_t command, uint16_t data)
{
    LCD->LCD_Reg = command;
    LCD->LCD_Ram = data;
}

void LCD_prepareWriteRam(void)
{
    LCD_writeCommand(lcddev.wramcmd);
}

/**
 * @brief       ST7789 寄存器初始化代码
 * @param       无
 * @retval      无
 */
void lcd_ex_st7789_reginit(void)
{
    LCD_writeCommand(0x11);

    delay_ms(120);

    LCD_writeCommand(0x36);
    LCD_writeData(0x08);

    LCD_writeCommand(0x3A);
    LCD_writeData(0X05);

    LCD_writeCommand(0xB2);
    LCD_writeData(0x0C);
    LCD_writeData(0x0C);
    LCD_writeData(0x00);
    LCD_writeData(0x33);
    LCD_writeData(0x33);

    LCD_writeCommand(0xB7);
    LCD_writeData(0x35);

    LCD_writeCommand(0xBB); /* vcom */
    LCD_writeData(0x32);  /* 30 */

    LCD_writeCommand(0xC0);
    LCD_writeData(0x0C);

    LCD_writeCommand(0xC2);
    LCD_writeData(0x01);

    LCD_writeCommand(0xC3); /* vrh */
    LCD_writeData(0x10);  /* 17 0D */

    LCD_writeCommand(0xC4); /* vdv */
    LCD_writeData(0x20);  /* 20 */

    LCD_writeCommand(0xC6);
    LCD_writeData(0x0f);

    LCD_writeCommand(0xD0);
    LCD_writeData(0xA4); 
    LCD_writeData(0xA1); 

    LCD_writeCommand(0xE0); /* Set Gamma  */
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

    LCD_writeCommand(0XE1);  /* Set Gamma */
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

    LCD_writeCommand(0x2A);
    LCD_writeData(0x00);
    LCD_writeData(0x00);
    LCD_writeData(0x00);
    LCD_writeData(0xef);

    LCD_writeCommand(0x2B);
    LCD_writeData(0x00);
    LCD_writeData(0x00);
    LCD_writeData(0x01);
    LCD_writeData(0x3f);

    LCD_writeCommand(0x29); /* display on */
}

void LCD_clear(uint16_t color)
{
    uint32_t total = lcddev.width * lcddev.height;
    LCD_setDisplayAera(0, lcddev.width - 1, 0, lcddev.height - 1);
    LCD_prepareWriteRam();
    for (uint32_t i = 0; i <= total; i++)
    {
        LCD_writeData(color);
    }
}

void LCD_Init(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOG, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    FSMC_NORSRAMInitTypeDef FSMC_NORSRAM_InitStructure;
    FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAM_writeTimingInitStructure;
    FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAM_readWriteTimingInitStructure;

    FSMC_NORSRAM_writeTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;
    FSMC_NORSRAM_writeTimingInitStructure.FSMC_AddressHoldTime = 1;
    FSMC_NORSRAM_writeTimingInitStructure.FSMC_AddressSetupTime = 0;
    FSMC_NORSRAM_writeTimingInitStructure.FSMC_BusTurnAroundDuration = 0;
    FSMC_NORSRAM_writeTimingInitStructure.FSMC_CLKDivision = 0;
    FSMC_NORSRAM_writeTimingInitStructure.FSMC_DataLatency = 0;
    FSMC_NORSRAM_writeTimingInitStructure.FSMC_DataSetupTime = 1;

    FSMC_NORSRAM_readWriteTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;
    FSMC_NORSRAM_readWriteTimingInitStructure.FSMC_AddressHoldTime = 1;
    FSMC_NORSRAM_readWriteTimingInitStructure.FSMC_AddressSetupTime = 0;
    FSMC_NORSRAM_readWriteTimingInitStructure.FSMC_BusTurnAroundDuration = 0;
    FSMC_NORSRAM_readWriteTimingInitStructure.FSMC_CLKDivision = 0;
    FSMC_NORSRAM_readWriteTimingInitStructure.FSMC_DataLatency = 0;
    FSMC_NORSRAM_readWriteTimingInitStructure.FSMC_DataSetupTime = 1;

    FSMC_NORSRAM_InitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAM_InitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
    FSMC_NORSRAM_InitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAM_InitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAM_InitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
    FSMC_NORSRAM_InitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAM_InitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAM_InitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAM_InitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAM_InitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAM_InitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAM_InitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAM_InitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAM_writeTimingInitStructure;
    FSMC_NORSRAM_InitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAM_readWriteTimingInitStructure;
    FSMC_NORSRAMInit(&FSMC_NORSRAM_InitStructure);

    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);

    lcd_ex_st7789_reginit();
    LCD_DeInit();
    GPIO_SetBits(GPIOB, GPIO_Pin_0);//开背光
    LCD_clear(WHITE);
}

void LCD_drawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_setDisplayAera(x, x, y, y);
    LCD_prepareWriteRam();
    LCD_writeData(color);
}

/*画水平线
    x : 0 ~ 239
    y : 0 ~ 319
    lineLength : 0 ~ 240
*/
void LCD_drawHorizontalLine(uint16_t x, uint16_t y, uint16_t color, uint16_t lineLength)
{
    LCD_setDisplayAera(x, x + lineLength - 1, y, y);
    LCD_prepareWriteRam();
    for(uint16_t i = 0; i < lineLength; i++)
    {
       LCD_writeData(color);
    }
}

void LCD_drawVerticalLine(uint16_t x, uint16_t y, uint16_t color, uint16_t lineLength)
{
    LCD_setDisplayAera(x, x, y, y + lineLength - 1);
    LCD_prepareWriteRam();
    for(uint16_t i = 0; i < lineLength; i++)
    {
       LCD_writeData(color);
    }
}

void LCD_drawRectangle(uint16_t x, uint16_t y, uint16_t color, uint16_t Width, uint16_t Height)
{
    LCD_drawHorizontalLine(x, y, color, Width);
    LCD_drawVerticalLine(x, y, color, Height);
    LCD_drawHorizontalLine(x, y + Height, color, Width);
    LCD_drawVerticalLine(x + Width, y, color, Height);
}

//画斜线: y = kx + b
void LCD_drawSlopingLine(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    if ((sx == ex) && (sy != ey))
    {
        if (sy > ey)
            LCD_drawVerticalLine(sx, sy, color, sy - ey + 1);
        else
            LCD_drawVerticalLine(sx, sy, color, ey - sy + 1);
    }
    else if ((sx != ex) && (sy == ey))
    {
        if (sx > ex)
            LCD_drawHorizontalLine(sx, sy, color, sx - ex + 1);
        else
            LCD_drawHorizontalLine(sx, sy, color, ex - sx + 1);
    }
    else if ((sx == ex) && (sy == ey))
        LCD_drawPoint(sx, sy, color);
    else //画任意斜率的线
    {
        double k = (double)(ey - sy) / (ex - sx);
        printf("k = %f\n", k);
        double b = (double)(ex * sy - sx * ey) / (ex - sx);
        printf("b = %f\n", b);
        if (sx > ex)
        {
            for (uint16_t i = 0; i < (sx - ex + 1); i++)
            {
                double y = k * (ex + i) + b;
                double dy = y - floor(y);
                if (dy >= 0.5)
                    y++;
                LCD_drawPoint(ex + i, y, color);
            }
        }
        else
        {
            for (uint16_t i = 0; i < (ex - sx + 1); i++)
            {
                double y = k * (sx + i) + b;
                double dy = y - floor(y);
                if (dy >= 0.5)
                    y++;
                LCD_drawPoint(sx + i, y, color);
            }
        }
    }
}

void LCD_setDisplayAera(uint16_t sx, uint16_t ex, uint16_t sy, uint16_t ey)
{
    LCD_writeCommand(lcddev.setxcmd);
    LCD_writeData(sx >> 8);
    LCD_writeData(sx & 0xFF);
    LCD_writeData(ex >> 8);
    LCD_writeData(ex & 0xFF);
    LCD_writeCommand(lcddev.setycmd);
    LCD_writeData(sy >> 8);
    LCD_writeData(sy & 0xFF);
    LCD_writeData(ey >> 8);
    LCD_writeData(ey & 0xFF);
}

void LCD_fillRectangle(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint32_t point = (ex - sx + 1) * (ey - sy + 1);
    LCD_setDisplayAera(sx, ex, sy, ey);
    LCD_prepareWriteRam();
    while (point--)
    {
        LCD_writeData(color);
    }
}

void LCD_dispFlush(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint32_t point = (ex - sx + 1) * (ey - sy + 1);
    LCD_setDisplayAera(sx, ex, sy, ey);
    LCD_prepareWriteRam();
    while (point--)
    {
        LCD_writeData(*color);
        color++;
    }
}

typedef struct
{
    uint16_t x;
    uint16_t y;
}Circle_Point;

void LCD_writeCircle(uint16_t x, uint16_t y, uint16_t color, uint16_t radius)
{
    //确定圆形四个点的坐标:圆心的上下左右四个点
    Circle_Point Point_up;
    Circle_Point Point_down;
    Circle_Point Point_left;
    Circle_Point Point_right;
    Point_up.x = x;
    Point_up.y = y - radius;
    Point_down.x = x;
    Point_down.y = y + radius;
    Point_left.x = x - radius;
    Point_left.y = y;
    Point_right.x = x + radius;
    Point_right.y = y;
    //圆形公式:(x - a) * (x - a) + (y - b) * (y - b) = r * r    圆心坐标:(a, b)
    for (uint16_t i = Point_left.x; i <= Point_right.x; i++)
    {
        LCD_drawPoint(i, y + sqrt(radius * radius - (i - x) * (i - x)), color);
        LCD_drawPoint(i, y - sqrt(radius * radius - (i - x) * (i - x)), color);
    }
    //这是画了两个圆，因为只画一次在起始绘画点和终止绘画点时，所绘制的点的间隔较大，这样画两个圆重叠在一起可以弥补一下这个缺点
    for (uint16_t i = Point_up.y; i <= Point_down.y; i++)
    {
        LCD_drawPoint(x + sqrt(radius * radius - (i - y) * (i - y)), i, color);
        LCD_drawPoint(x - sqrt(radius * radius - (i - y) * (i - y)), i, color);
    }
}

void LCD_fillCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color)
{
    Circle_Point Point_left;
    Circle_Point Point_right;
    Point_left.x = x - radius;
    Point_left.y = y;
    Point_right.x = x + radius;
    Point_right.y = y;
    for (uint16_t i = Point_left.x; i <= Point_right.x; i++)
    {
        LCD_drawVerticalLine(i, y - sqrt(radius * radius - (i - x) * (i - x)) + 1, color, 2 * sqrt(radius * radius - (i - x) * (i - x)));
    }
}

//size:字体大小 16 24 32
void LCD_showChar(uint16_t x, uint16_t y, uint8_t size, uint16_t color, uint16_t backcolor, char chr)
{
    LCD_setDisplayAera(x, x + size / 2 - 1, y, y + size - 1);
    LCD_prepareWriteRam();
    u8 *pfont;
    uint8_t realSize = 0;
    u8 temp;
    switch (size)
    {
        case 16:
        {
            realSize = 16;
            pfont = (u8*)&ASCII_1608[(chr - ' ') * realSize];
            for (int i = 0; i < size; i++)
            {
                temp = *pfont++;
                for (int j = 0; j < 8; j++)
                {
                    if (temp & 0x80)
                        LCD_writeData(color);
                    else
                        LCD_writeData(backcolor);
                    temp = temp << 1;
                }
            }
            break;
        }
        case 24:
        {
            realSize = 48;
            pfont = (u8*)&ASCII_2412[(chr - ' ') * realSize];
            for (int i = 0; i < size; i++)
            {
                temp = *pfont++;
                for (int j = 0; j < 8; j++)
                {
                    if (temp & 0x80)
                        LCD_writeData(color);
                    else
                        LCD_writeData(backcolor);
                    temp = temp << 1;
                }
                temp = *pfont++;
                for (int j = 0; j < 4; j++)
                {
                    if (temp & 0x80)
                        LCD_writeData(color);
                    else
                        LCD_writeData(backcolor);
                    temp = temp << 1;
                }
            }
            break;
        }
        case 32:
        {
            realSize = 64;
            pfont = (u8*)&ASCII_3216[(chr - ' ') * realSize];
            for (int i = 0; i < size; i++)
            {
                temp = *pfont++;
                for (int j = 0; j < 8; j++)
                {
                    if (temp & 0x80)
                        LCD_writeData(color);
                    else
                        LCD_writeData(backcolor);
                    temp = temp << 1;
                }
                temp = *pfont++;
                for (int j = 0; j < 8; j++)
                {
                    if (temp & 0x80)
                        LCD_writeData(color);
                    else
                        LCD_writeData(backcolor);
                    temp = temp << 1;
                }
            }
            break;
        }
        default:
            return;
    }
}

void LCD_showString(uint16_t x, uint16_t y, uint8_t size, uint16_t color, uint16_t backcolor, char* str)
{
    for (uint64_t i = 0; str[i] != '\0'; i++)
    {
        LCD_showChar(x + (i * size / 2), y, size, color, backcolor, str[i]);
    }
}

void LCD_showNum(uint16_t x, uint16_t y, uint8_t size, uint16_t color, uint16_t backcolor, uint64_t num)
{
    char str[21];
    uint64_t data = num;
    uint8_t numLength = 0;
    while (data != 0)
    {
        numLength++;
        data /= 10;
    }
    for (uint8_t i = 0; i < numLength; i++)
    {
        str[numLength - 1 - i] = num % 10 + '0';
        num /= 10;
    }
    str[numLength] = '\0';
    LCD_showString(x, y, size, color, backcolor, str);
}

void LCD_scanDir(uint8_t dir)
{
    if (dir)
    {
        lcddev.dir = 1;
        lcddev.width = 320;
        lcddev.height = 240;
        lcddev.setxcmd = 0x2A;
        lcddev.setycmd = 0x2B;
    }
    else
    {
        lcddev.dir = 0;
        lcddev.width = 240;
        lcddev.height = 320;
        lcddev.setxcmd = 0x2A;
        lcddev.setycmd = 0x2B;
    }
    uint16_t regval = 0;
    uint16_t dirreg = 0;
 
    /* 根据扫描方式 设置 0X36/0X3600 寄存器 bit 5,6,7 位的值 */
    switch (dir)
    {
        case 0:/* 从左到右,从上到下 */
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;

        case 1:/* 从下到上,从左到右 */
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;
        default:
            break;
    }
    dirreg = 0X36;  /* 对绝大部分驱动IC, 由0X36寄存器控制 */
    regval |= 0X08;
    LCD_writeCommandAndData(dirreg, regval);
    LCD_writeCommand(lcddev.setxcmd);
    LCD_writeData(0);
    LCD_writeData(0);
    LCD_writeData((lcddev.width - 1) >> 8);
    LCD_writeData((lcddev.width - 1) & 0XFF);
    LCD_writeCommand(lcddev.setycmd);
    LCD_writeData(0);
    LCD_writeData(0);
    LCD_writeData((lcddev.height - 1) >> 8);
    LCD_writeData((lcddev.height - 1) & 0XFF);
}
