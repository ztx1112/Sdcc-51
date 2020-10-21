#include "STC15Fxxxx.H"
#include <intrins.h>
#include "EEPROM.h"
#include "adc.h"
#include "timer.h"

#define TIMEOUT 50000
#define TIME_MAX_COUTER 1000

int timecounter;

sbit X0 = P1 ^ 2;
sbit X1 = P1 ^ 3;
sbit X2 = P1 ^ 4;
sbit X3 = P1 ^ 5;

sbit Y0 = P3 ^ 3;
sbit Y1 = P3 ^ 2;

static int Maxact = 4;
static char Makeup1, Makeup2;

int Act1(); //大先小后
int Act2(); //仅大
int Act3(); //小大小大
int Act4(); //小大大小

void AdcInit();
int AdcExcute();
int EEPROMend();


/********************* Timer0中断函数************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
	timecounter+=1;
	if(timecounter>=TIME_MAX_COUTER)
	{
		IAP_CONTR = 0x60;
	}
		

}

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
u16 a, b;
u8 act;
u8 n = 0;
int i = 0;
u8 X3X = 1;
u8 X0X = 1;
u16 addr;


int Act1()
{
	Y0 = 0;
	while (1)
	{
		if (X1 == 0)
		{
			Timer0_Stop();
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
			Timer0_Stop();
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
			Timer0_Stop();
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
			Timer0_Stop();
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

int Act5()
{
	Y1 = 0;
	while (1)
	{
		if (X2 == 0)
		{
			Timer0_Stop();
			delay_ms(result2);
			Y0 = 0;
			while (1)
			{
				if (X1 == 0)
				{
					delay_ms(result1);
					Y0 = 1;
					while (X1 == 1)
						;
					delay_ms(80);

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
	P1ASF = ADC_P10 | ADC_P11;
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
	a = Get_ADC10bitResult(0);
	delay_ms(50);
	a = Get_ADC10bitResult(0);
	result1 = a - 500;
	delay_ms(50);
	a = Get_ADC10bitResult(1);
	delay_ms(50);
	a = Get_ADC10bitResult(1);
	result2 = a - 500;
}

int EEPROMend()
{
	EEPROM_SectorErase(0x00);
	delay_ms(5);
	EEPROM_write_n(0x00, &act, 1);
}


TIM_InitTypeDef TIMdef;
int main()
{
	P1M1 = 0x03;
	P1M0 = 0x00;
	P3M0 = 0x80;
	P3M1 = 0x00;
	P3=0xff;
	P1=0xff;

	AdcInit();
	delay_ms(300);
	AdcExcute();
	while (result1 == (1024-500))
	{
		delay_ms(500);
		AdcExcute();
	}
	while (result2 == (1024-500))
	{
		delay_ms(500);
		AdcExcute();
	}

	EEPROM_read_n(0x0000, &act, 1);
	if (act > Maxact)
		act = 0;
	if (act < 0)
		act = 0;

	TIMdef.TIM_ClkOut=0;
	TIMdef.TIM_Mode=TIM_16BitAutoReload;
	TIMdef.TIM_Polity=PolityHigh;
	TIMdef.TIM_Interrupt=ENABLE;
	TIMdef.TIM_ClkOut=DISABLE;
	TIMdef.TIM_Run=DISABLE;
	TIMdef.TIM_ClkSource=TIM_CLOCK_1T;
	Timer_Inilize(0,&TIMdef);

	Timer0_Load(TIMEOUT);

	P1 = 0XFF;
	P10 = 0;
	P11 = 0;
	P37 = 1;

	EA = 1;

	timecounter=0;

	while (1)
	{
		if ((X1 != 0) && (X2 != 0))
		{
			delay_ms(5);
			if (X0 == 0)
			{
				Timer0_Stop();
				Timer0_Load(TIMEOUT);
				Timer0_Run();
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
				case 4:
					Act5();
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

