#include "lcd.h"
#include "lcdfont.h"
#include <stm32f1xx_ll_fsmc.h>
#include <stm32f1xx_hal_sram.h>
//控制引脚宏定义
#define LCD_BL_Pin  GPIO_PIN_0
#define LCD_CS_Pin  GPIO_PIN_12
#define LCD_RS_Pin  GPIO_PIN_0
#define LCD_WR_Pin  GPIO_PIN_5
#define LCD_RD_Pin  GPIO_PIN_4
//数据引脚宏定义
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

/*对于LCD_Base说明如下：
 *0x6000 0000:因为用的是sram所以选择了bank1，bank1的起始地址就是这个
 *0x400 0000：在16进制下为64MB，bank1分为4块，每一块64MB
 *3：硬件连接NE4，也就是选择了第四块，这是为了算出第四块的起始地址
 *1 << 10:这个1代表着fsmc连接lcd_rs的那一位置1，因为0时操作命令，1时操作数据，现在在确定ram的起始地址，所以应该置1.
 *第一个2：因为使用了16位的数据模式fsmc是[24:0],但是地址线是[25:1],所以要左移一位
 *第二个2：因为在结构体中只有ram的地址是能够算出来的，所以要通过ram来算出其他的，reg在ram前面，16位两个字节，减去就得到lcd结构体地址
*/

#define LCD_BASE        (uint32_t)((0X60000000 + (0X4000000 * (4 - 1))) | (((1 << 10) * 2) -2))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

/**
 * @brief       lcd写寄存器编号
 * @param       Reg：寄存器编号
 * @retval      无
 */
void LCD_writeRegNum(uint16_t Reg)
{
    LCD->LCD_Reg = Reg;
}

/**
 * @brief       lcd开启显示
 * @param       无
 * @retval      无
 */
void LCD_displayOn(void)
{
    LCD_writeRegNum(0x29);
}

/**
 * @brief       lcd关闭显示
 * @param       无
 * @retval      无
 */
void LCD_displayOff(void)
{
    LCD_writeRegNum(0x28);
}

/**
 * @brief       lcd写数据
 * @param       Data:数据
 * @retval      无
 */
void LCD_writeData(uint16_t Data)
{
    LCD->LCD_Ram = Data;
}

/**
 * @brief       lcd写寄存器
 * @param       Data:数据
 * @retval      无
 */
void LCD_writeReg(uint16_t Reg,uint16_t Data)
{
    LCD->LCD_Reg = Reg;
    LCD->LCD_Ram = Data;
}

/**
 * @brief       lcd设置坐标
 * @param       x:横坐标 y:纵坐标
 * @retval      无
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
 * @brief       lcd准备写ram
 * @param       无
 * @retval      无
 */
void LCD_prepareWriteRam(void)
{
    LCD->LCD_Reg = writeRamCmd;
}

/**
 * @brief       画点
 * @param       x,y: 坐标
 * @param       color: 点的颜色(32位颜色,方便兼容LTDC)
 * @retval      无
 */
void LCD_drawPoint(uint16_t x,uint16_t y,uint32_t color)
{
    LCD_setCursor(x,y);
    LCD_prepareWriteRam();
    LCD->LCD_Ram = color;
}

/**
 * @brief       LCD读数据
 * @param       无
 * @retval      读取到的数据
 */
uint16_t LCD_readData(void)
{
    uint16_t data = 0;
    data = LCD->LCD_Ram;
    return data;
}

/**
 * @brief       读取个某点的颜色值
 * @param       x,y:坐标
 * @retval      此点的颜色(32位颜色,方便兼容LTDC)
 */
uint32_t LCD_readPoint(uint16_t x,uint16_t y)
{
    uint16_t r = 0,g = 0,b = 0;
    if((x >= width) | (y >= length))return 0;
    LCD_writeRegNum(0x2E);
    r = LCD_readData();//dummy 假读
    r = LCD_readData();//以下部分参考开发指南，很好理解，主要是因为一次读取的数据里面包含了不同颜色的数据
    b = LCD_readData();
    g = r & 0XFF;
    g <<= 8;
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));
}

/**
 * @brief       清屏函数
 * @param       color: 要清屏的颜色
 * @retval      无
 */
void LCD_clear(uint32_t color)
{
    uint32_t totalPoint = width * length;//全部点
    LCD_setCursor(0,0);
    LCD_prepareWriteRam();
    for(int i = 0;i <= totalPoint;i++)
    {
        LCD->LCD_Ram = color;
    }
}

/**
 * @brief       平方函数, m^n
 * @param       m: 底数
 * @param       n: 指数
 * @retval      m的n次方
 */
uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)result *= m;

    return result;
}

/**
 * @brief       在指定位置显示一个字符
 * @param       x,y  : 坐标
 * @param       chr  : 要显示的字符:" "--->"~"
 * @param       size : 字体大小 12/16/24/32
 * @param       mode : 叠加方式(1); 非叠加方式(0);
 * @param       color : 字符的颜色;
 * @param       backColor:当前背景颜色
 * @retval      无
 */
void LCD_showChar(uint16_t x,uint16_t y,char chr,uint8_t size,uint8_t mode, uint32_t color,uint32_t backColor)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* 得到字体一个字符对应点阵集所占的字节数 */
    chr = chr - ' ';    /* 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库） */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[chr];  /* 调用1206字体 */
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[chr];  /* 调用1608字体 */
            break;

        case 24:
            pfont = (uint8_t *)asc2_2412[chr];  /* 调用2412字体 */
            break;

        case 32:
            pfont = (uint8_t *)asc2_3216[chr];  /* 调用3216字体 */
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];    /* 获取字符的点阵数据 */

        for (t1 = 0; t1 < 8; t1++)   /* 一个字节8个点 */
        {
            if (temp & 0x80)        /* 有效点,需要显示 */
            {
                LCD_drawPoint(x, y, color);        /* 画点出来,要显示这个点 */
            }
            else if (mode == 0)     /* 无效点,不显示 */
            {
                LCD_drawPoint(x, y, backColor); /* 画背景色,相当于这个点不显示(注意背景色由全局变量控制) */
            }

            temp <<= 1; /* 移位, 以便获取下一个位的状态 */
            y++;

            if (y >= length)return;  /* 超区域了 */

            if ((y - y0) == size)   /* 显示完一列了? */
            {
                y = y0; /* y坐标复位 */
                x++;    /* x坐标递增 */

                if (x >= width)return;   /* x坐标超区域了 */

                break;
            }
        }
    }
}

/**
 * @brief       显示len个数字
 * @param       x,y : 起始坐标
 * @param       num : 数值(0 ~ 2^32)
 * @param       len : 显示数字的位数
 * @param       size: 选择字体 12/16/24/32
 * @param       color : 数字的颜色;
 * @param       backColor   :当前背景颜色
 * @retval      无
 */
void LCD_showNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color,uint32_t backColor)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* 按总显示位数循环 */
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;   /* 获取对应位的数字 */

        if (enshow == 0 && t < (len - 1))   /* 没有使能显示,且还有位要显示 */
        {
            if (temp == 0)
            {
                LCD_showChar(x + (size / 2)*t, y, ' ', size, 0, color,backColor);/* 显示空格,占位 */
                continue;   /* 继续下个一位 */
            }
            else
            {
                enshow = 1; /* 使能显示 */
            }

        }

        LCD_showChar(x + (size / 2)*t, y, temp + '0', size, 0, color,backColor); /* 显示字符 */
    }
}

/**
 * @brief       显示字符串
 * @param       x,y         : 起始坐标
 * @param       Width,height: 区域大小
 * @param       size        : 选择字体 12/16/24/32
 * @param       p           : 字符串首地址
 * @param       color       : 字符串的颜色;
 * @param       backColor   :当前背景颜色
 * @retval      无
 */
void LCD_showString(uint16_t x, uint16_t y, uint16_t Width, uint16_t height, uint8_t size, char *p, uint16_t color,uint32_t backColor)
{
    uint8_t x0 = x;
    Width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   /* 判断是不是非法字符! */
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)break;  /* 退出 */

        LCD_showChar(x, y, *p, size, 0, color,backColor);
        x += size / 2;
        p++;
    }
}


/**
 * @brief       lcd初始化函数
 * @param       无
 * @retval      无
 */
void LCD_Init(void)
{
    //开启FSMC和GPIO时钟
    __HAL_RCC_FSMC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    //数据引脚初始化
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin=LCD_Data0_Pin | LCD_Data1_Pin | LCD_Data2_Pin | LCD_Data3_Pin | LCD_Data13_Pin | LCD_Data14_Pin | LCD_Data15_Pin;
    GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;//复用推挽输出
    GPIO_InitStruct.Pull=GPIO_PULLUP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_Data_Port1,&GPIO_InitStruct);

    GPIO_InitStruct.Pin=LCD_Data4_Pin | LCD_Data5_Pin | LCD_Data6_Pin | LCD_Data7_Pin | LCD_Data8_Pin | LCD_Data9_Pin | LCD_Data10_Pin | LCD_Data11_Pin | LCD_Data12_Pin;
    GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;//复用推挽输出
    GPIO_InitStruct.Pull=GPIO_PULLUP;
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_Data_Port2,&GPIO_InitStruct);
    //控制引脚初始化
    GPIO_InitStruct.Pin=LCD_BL_Pin;
    GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;//通用推挽输出
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
    //FSMC和SRAM配置，LCD和Sram时序近似所以将LCD当作SRAM来用
    //SRAM_HandleTypeDef SRAM_InitStruct;
    SRAM_InitStruct.Instance=FSMC_NORSRAM_DEVICE;
    SRAM_InitStruct.Extended=FSMC_NORSRAM_EXTENDED_DEVICE;
    SRAM_InitStruct.Init.NSBank = FSMC_NORSRAM_BANK4;                        /* 使用NE4 */
    SRAM_InitStruct.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;     /* 地址/数据线不复用 */
    SRAM_InitStruct.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;    /* 16位数据宽度 */
    SRAM_InitStruct.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;   /* 是否使能突发访问,仅对同步突发存储器有效,此处未用到 */
    SRAM_InitStruct.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW; /* 等待信号的极性,仅在突发模式访问下有用 */
    SRAM_InitStruct.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;      /* 存储器是在等待周期之前的一个时钟周期还是等待周期期间使能NWAIT */
    SRAM_InitStruct.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;       /* 存储器写使能 */
    SRAM_InitStruct.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;              /* 等待使能位,此处未用到 */
    SRAM_InitStruct.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;           /* 读写使用不同的时序 */
    SRAM_InitStruct.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;  /* 是否使能同步传输模式下的等待信号,此处未用到 */
    SRAM_InitStruct.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;              /* 禁止突发写 */

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
    //打开背光
    HAL_GPIO_WritePin(GPIOB,LCD_BL_Pin,SET);
    //设置扫描方向
    LCD_writeReg(0x36,0x08);
    LCD_clear(WHITE);
}

/**
 * @brief       ST7789 寄存器初始化代码
 * @param       无
 * @retval      无
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
 * @brief       在指定区域内填充单个颜色
 * @param       (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex - sx + 1) * (ey - sy + 1)
 * @param       color:要填充的颜色(32位颜色,方便兼容LTDC)
 * @retval      无
 */
void LCD_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
    uint16_t i, j;
    uint16_t xlen = 0;
    xlen = ex - sx + 1;

    for (i = sy; i <= ey; i++)
    {
        LCD_setCursor(sx, i);      /* 设置光标位置 */
        LCD_prepareWriteRam();    /* 开始写入GRAM */

        for (j = 0; j < xlen; j++)
        {
            LCD->LCD_Ram = color;   /* 显示颜色 */
        }
    }
}

/**
 * @brief       画线
 * @param       x1,y1: 起点坐标
 * @param       x2,y2: 终点坐标
 * @param       color: 线的颜色
 * @retval      无
 */
void LCD_drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    delta_x = x2 - x1;          /* 计算坐标增量 */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)incx = 1;   /* 设置单步方向 */
    else if (delta_x == 0)incx = 0; /* 垂直线 */
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; /* 水平线 */
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x;  /* 选取基本增量坐标轴 */
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )   /* 画线输出 */
    {
        LCD_drawPoint(row, col, color); /* 画点 */
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
 * @brief       画圆
 * @param       x,y  : 圆中心坐标
 * @param       r    : 半径
 * @param       color: 圆的颜色
 * @retval      无
 */
void LCD_drawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       /* 判断下个点位置的标志 */

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

        /* 使用Bresenham算法画圆 */
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
 * @brief       画水平线
 * @param       x,y: 起点坐标
 * @param       len  : 线长度
 * @param       color: 矩形的颜色
 * @retval      无
 */
void LCD_drawHline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x > width) || (y > length))return;

    LCD_fill(x, y, x + len - 1, y, color);
}

/**
 * @brief       填充实心圆
 * @param       x,y: 圆中心坐标
 * @param       r    : 半径
 * @param       color: 圆的颜色
 * @retval      无
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
