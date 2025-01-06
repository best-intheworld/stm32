#include "Serial.h"

char USART1_ReceiveBuf[1024] = {0};
uint16_t buffHead = 0;

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = USART1_TX;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART1_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = USART1_RX;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART1_PORT, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
}

void USART1_sendByte(uint8_t byte)
{
	USART_SendData(USART1, byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void USART1_sendArray(uint8_t *array, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++)
	{
		USART1_sendByte(array[i]);
	}
}

void USART1_sendString(char *str)
{
	for (uint16_t i = 0; str[i] != '\0'; i++)
	{
		USART1_sendByte(str[i]);
	}
}

void USART1_sendNum(uint32_t num)
{
	uint8_t length = 1;
	uint32_t data = num;
	while (data)
	{
		if ((data /= 10) != 0)
			length++;
	}
	char array[12] = {0};
	for (uint8_t i = 0; i < length; i++)
	{
		array[length - 1 - i] = num % 10 + '0';
		num /= 10;
	}
	array[length] = '\0';
	USART1_sendString(array);
}

void USART1_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	USART1_sendString(String);
}

void USART1_IRQHandler(void)
{
	while (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		USART1_ReceiveBuf[buffHead] = USART_ReceiveData(USART1);
		buffHead++;
		if (buffHead >= 1023)
			buffHead = 0;
	}
}

int fputc(int ch, FILE *f)
{
	USART1_sendByte(ch);
	return ch;
}

void TRACE(char *str)
{
	USART1_sendString(str);
}
