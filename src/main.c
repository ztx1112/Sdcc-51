#include "C:\\Users\\Administrator\\Documents\\Source\\MCU\\Sdcc-51\\src\\STC15Fxxxx.H"
#include <intrins.h>
#include "C:\\Users\\Administrator\\Documents\\Source\\MCU\\Sdcc-51\\src\\EEPROM.h"
#include "C:\\Users\\Administrator\\Documents\\Source\\MCU\\Sdcc-51\\src\\adc.h"

#define FOSC 11059200L //系统频率
#define BAUD 115200	   //串口波特率

sbit X0 = P1 ^ 4;
sbit X1 = P1 ^ 5;
sbit X2 = P5 ^ 4;
sbit X3 = P5 ^ 5;

sbit Y0 = P3 ^ 3;
sbit Y1 = P3 ^ 2;

static int Maxact = 3;
static char Makeup1, Makeup2;

int Act1(); //大先小后
int Act2(); //仅大
int Act3(); //小大小大
int Act4(); //小大大小

void AdcInit();
int AdcExcute();
int EEPROMend();

void delay_ms(unsigned int ms)
{
	unsigned int i;
	do
	{
		i = 12000000L / 13000;
		while (--i)
			; //14T per loop
	} while (--ms);
}

u16 result1 = 0;
u16 result2 = 0;
u8 a, b;
u8 act;
u8 n = 0;
int i = 0;
u8 X3X = 1;
u8 X0X = 1;
u16 addr;

bit busy;

void SendData(u8 dat);
void SendString(char *s);

int Act1()
{
	Y0 = 0;
	while (1)
	{
		if (X1 == 0)
		{
			delay_ms(result1);
			Y1 = 0;
			while (1)
			{
				if (X2 == 0)
				{
					delay_ms(result2);
					Y1 = 1;
					delay_ms(50);
					Y0 = 1;
					return 0;
				}
			}
			return 0;
		}
	}
	return 0;
}

int Act2()
{
	Y0 = 0;
	while (1)
	{
		if (X1 == 0)
		{
			delay_ms(result1);
			Y0 = 1;
			while (1)
			{
				if (X1 == 1)
				{
					return 0;
				}
			}
			return 0;
		}
	}
	return 0;
}

int Act3()
{
	Y1 = 0;
	while (1)
	{
		if (X2 == 0)
		{
			delay_ms(result2);
			Y0 = 0;
			while (1)
			{
				if (X1 == 0)
				{
					delay_ms(result1);
					Y1 = 1;
					delay_ms(30);
					Y0 = 1;
					while (1)
					{
						if (X1 == 1 && X2 == 1)
						{
							return;
						}
					}
					return 0;
				}
			}
		}
	}
}

int Act4()
{
	Y1 = 0;
	while (1)
	{
		if (X2 == 0)
		{
			delay_ms(result2);
			Y0 = 0;
			while (1)
			{
				if (X1 == 0)
				{
					delay_ms(result1);
					Y0 = 1;
					delay_ms(30);
					Y1 = 1;
					while (1)
					{
						if (X1 == 1 && X2 == 1)
						{
							return;
						}
					}
				}
			}
		}
	}
}

void AdcInit()
{
	P1ASF = ADC_P12 | ADC_P13;
	ADC_CONTR = (ADC_CONTR & ~ADC_90T) | ADC_360T;
	ADC_CONTR |= 0x80;
	CLK_DIV |= (1 << 5); //10位AD结果的高2位放ADC_RES的低2位，低8位在ADC_RESL。
	EADC = 1;			 //中断允许		ENABLE,DISABLE
	PADC = 1;			 //优先级设置	PolityHigh,PolityLow
	ADC_CONTR |= 0x80;
	delay_ms(200);
}

int AdcExcute()
{
	// Get_ADC10bitResult(2);
	// delay_ms(50);
	// result1 = Get_ADC10bitResult(2);
	// delay_ms(50);
	// Get_ADC10bitResult(3);
	// delay_ms(50);
	// result2 = Get_ADC10bitResult(3);
	a = Get_ADC8bitResult(2);
	delay_ms(50);
	a = Get_ADC8bitResult(2);
	result1 = a * 5;
	delay_ms(50);
	a = Get_ADC8bitResult(3);
	delay_ms(50);
	a = Get_ADC8bitResult(3);
	delay_ms(50);
	result2 = a * 5;
}

int EEPROMend()
{
	EEPROM_SectorErase(0x00);
	delay_ms(5);
	EEPROM_write_n(0x00, &act, 1);
}

int main()
{
	P1M1 = 0x0f;
	P1M0 = 0x00;
	P3M0 = 0x00;
	P3M1 = 0x00;

	AdcInit();
	delay_ms(300);
	AdcExcute();
	while (result1 == 1024)
	{
		delay_ms(500);
		AdcExcute();
	}
	while (result2 == 1024)
	{
		delay_ms(500);
		AdcExcute();
	}

	EEPROM_read_n(0x0000, &act, 1);
	if (act > Maxact)
		act = 0;
	if (act < 0)
		act = 0;
	P1 = 0XFF;

	SCON = 0x50;					   //8位可变波特率
	T2L = (65536 - (FOSC / 4 / BAUD)); //设置波特率重装值
	T2H = (65536 - (FOSC / 4 / BAUD)) >> 8;
	AUXR = 0x14;  //T2为1T模式, 并启动定时器2
	AUXR |= 0x01; //选择定时器2为串口1的波特率发生器
	ES = 1;		  //使能串口1中断

	SendData(a);

	while (1)
	{
		if ((X1 != 0) && (X2 != 0))
		{
			delay_ms(5);
			if (X0 == 0)
			{
				switch (act)
				{
				case 0:
					Act1();
					break;
				case 1:
					Act2();
					break;
				case 2:
					Act3();
					break;
				case 3:
					Act4();
					break;
				default:
					break;
				}
				if (X0 == 0)
				{
					while (1)
					{
						if (X0 != 0)
							break;
					}
				}
			}
		}
		if (X3 == 0)
		{
			delay_ms(10);
			if (X3 == 0)
			{
				act++;
				if (act > Maxact)
					act = 0;
				EEPROM_SectorErase(0x0000);
				delay_ms(100);
				EEPROM_write_n(0x0000, &act, 1);
				delay_ms(100);
				IAP_CONTR = 0x60;
			}
		}
	}
}

/*----------------------------
UART 中断服务程序
-----------------------------*/
void Uart() interrupt 4
{
	if (RI)
	{
		RI = 0;	   //清除RI位
		P0 = SBUF; //P0显示串口数据
	}
	if (TI)
	{
		TI = 0;	  //清除TI位
		busy = 0; //清忙标志
	}
}

/*----------------------------
发送串口数据
----------------------------*/
void SendData(u8 dat)
{
	while (busy)
		;	   //等待前面的数据发送完成
	ACC = dat; //获取校验位P (PSW.0)

	busy = 1;
	SBUF = ACC; //写数据到UART数据寄存器
}

/*----------------------------
发送字符串
----------------------------*/
void SendString(char *s)
{
	while (*s) //检测字符串结束标志
	{
		SendData(*s++); //发送当前字符
	}
}
