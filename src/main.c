#include "STC15Fxxxx.H"
#include "soft_uart.h"
#include <intrins.h>
#include "EEPROM.h"
#include "D:\\SDK\\msc51\\adc.h"

static int Maxact=3;

sbit X0 = P1^2;
sbit X1 = P1^3;
sbit X2 = P1^4;
sbit X3 = P1^5;

sbit Y0 = P3^3;
sbit Y1 = P3^2;

int Act1();	//大先小后
int Act2();	//仅大
int Act3();	//小大小大
int Act4();	//小大大小

void AdcInit();
int AdcExcute();




void  delay_ms(unsigned int ms)
{
     unsigned int i;
	 do{
	      i = 12000000L / 13000;
		  while(--i)	;   //14T per loop
     }while(--ms);
}

u16 result1=0;
u16 result2=0;
u8 act;
u8 n=0;
int i=0;
u8 X3X=1;
int main()
{
	AdcInit();
	delay_ms(100);
	Get_ADC10bitResult(0);
	delay_ms(100);
	result1=Get_ADC10bitResult(0);
	delay_ms(100);
	Get_ADC10bitResult(1);
	delay_ms(100);
	result2=Get_ADC10bitResult(1);
	if(result1==1024)
	{
		while(1)
		{
			delay_ms(200);
			TxSend(0x99);
			delay_ms(50);
			TxSend((u8)result1);
			delay_ms(50);
			TxSend((u8)result2);
		}
	}
	if(result2==1024)
	{
		while(1)
		{
			delay_ms(200);
			TxSend(0x99);
			delay_ms(50);
			TxSend((u8)result1);
			delay_ms(50);
			TxSend((u8)result2);
		}
	}
	result1=result1-500;
	result2=result2-400;
	EEPROM_read_n(0x0000,&act,1);
	if(act>Maxact)act=0;
	P1=0XFF;
	P3=0XFF;
	while(1)
	{
		if((X1!=0)&&(X2!=0))
		{
			if(X0==0)
			{
				delay_ms(5);
				if(X0!=0)break;
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
			}
			if(X3==0)
			{
				if(X3X==1)
				{
					act++;
					if(act>Maxact)act=0;
					EEPROM_SectorErase(0x00);
					delay_ms(5);
					EEPROM_write_n(0x0000,&act,1);
					delay_ms(5);
				}
			}
		}
		X3X=X3;
	}
}
		
		
		
int Act1()
{
	Y0=0;
	while(1)
	{
		if(X1==0)
		{
			delay_ms(result1);
			Y1=0;
			while(1)
			{
				if(X2==0)
				{
					delay_ms(result2);
					Y1=1;
					delay_ms(30);
					Y0=1;
					return 0;
				}
			}
		}
	}
}

int Act2()
{
	Y0=0;
	while(1)
	{
		if(X1==0)
		{
			delay_ms(result1);
			Y0=1;
			while(1)
			{
				if(X1==1)
				{
					return 0;
				}
			}
		}
	}
}

int Act3()
{
	Y1=0;
	while(1)
	{
		if(X2==0)
		{
			delay_ms(result2);
			Y0=0;
			while(1)
			{
				if(X1==0)
				{
					delay_ms(result1);
					Y1=1;
					delay_ms(30);
					Y0=1;
					while (1)
					{
						if(X1==1&&X2==1)
						{
							return;
						}
					}
					
				}
			}
		}
	}
}

int Act4()
{
	Y1=0;
	while(1)
	{
		if(X2==0)
		{
			delay_ms(result2);
			Y0=0;
			while(1)
			{
				if(X1==0)
				{
					delay_ms(result1);
					Y0=1;
					delay_ms(30);
					Y1=1;
					while (1)
					{
						if(X1==1&&X2==1)
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
	P1ASF = ADC_P11 | ADC_P10;
	ADC_CONTR = (ADC_CONTR & ~ADC_90T) | ADC_360T;
	ADC_CONTR |= 0x80;
	CLK_DIV |=  (1<<5);	//10位AD结果的高2位放ADC_RES的低2位，低8位在ADC_RESL。
	EADC = 1;			//中断允许		ENABLE,DISABLE
	PADC = 1;		//优先级设置	PolityHigh,PolityLow
	ADC_CONTR |=0x80;
	delay_ms(200);
}

int AdcExcute()
{

}