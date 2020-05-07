#include "C:\\Users\\Administrator\\Documents\\Source\\MCU\\Sdcc-51\\src\\STC15Fxxxx.H"
#include <intrins.h>
#include "C:\\Users\\Administrator\\Documents\\Source\\MCU\\Sdcc-51\\src\\EEPROM.h"
#include "C:\\Users\\Administrator\\Documents\\Source\\MCU\\Sdcc-51\\src\\adc.h"

sbit X0 = P1^2;
sbit X1 = P1^3;
sbit X2 = P1^4;
sbit X3 = P1^5;

sbit Y0 = P3^3;
sbit Y1 = P3^2;


int UartHandle(char);
#define BRT             (65536 - MAIN_Fosc / 115200 / 4)
bit busy;
char wptr;
char rptr;
char buffer[16];

unsigned char databit;

void UartIsr() interrupt 4
{
    if (TI)
    {
        TI = 0;
        busy = 0;
    }
    if (RI)
    {
        RI = 0;
        buffer[wptr++] = SBUF;
        wptr &= 0x0f;
    }
}

void UartInit()
{
    SCON = 0x50;
	TMOD = 0x00;
    TL1 = BRT;
	TH1 = BRT >> 8;
    TR1 = 1;
	AUXR = 0x40;
    wptr = 0x00;
    rptr = 0x00;
    busy = 0;
}

void UartSend(char dat)
{
    while (busy);
    busy = 1;
    SBUF = dat;
}

void UartSendStr(char *p)
{
    while (*p)
    {
        UartSend(*p++);
    }
}

char UartRead()
{
	Y0=0;
	if (rptr != wptr)
	{
		Y1=0;
        rptr &= 0x0f;
		UartHandle(buffer[rptr++]);
	}
	return 0;
}

static int Maxact=3;
static char Makeup1,Makeup2;

int Act1();	//大先小后
int Act2();	//仅大
int Act3();	//小大小大
int Act4();	//小大大小

void AdcInit();
int AdcExcute();
int EEPROMend();



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
u8 X0X=1;
u16 addr;
	
		
		
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
			return 0;
		}
	}
	return 0;
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
			return 0;
		}
	}
	return 0;
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
					return 0;
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
	Get_ADC10bitResult(0);
	delay_ms(50);
	result1=Get_ADC10bitResult(0);
	delay_ms(50);
	Get_ADC10bitResult(1);
	delay_ms(50);
	result2=Get_ADC10bitResult(1);
	//result1=result1-Makeup1;
	//result2=result2-Makeup2;
}


int UartHandle(char dat)
{
	Y1=0;
	switch(dat)
	{
		case 'D':
			IAP_CONTR=0x60;
			break;
		case 'o':
			Makeup1+=10;
			break;
		case 'p':
			Makeup1-=10;
			break;
		case 'k':
			Makeup2+=10;
			break;
		case 'l':
			Makeup2-=10;
			break;
		case 't':
		{
			UartSend(Makeup1);
			delay_ms(100);
			UartSend(Makeup2);
			break;
		}
		default:
			break;
	}
}

int EEPROMend()
{
	EEPROM_SectorErase(0x00);
	delay_ms(5);
	EEPROM_write_n(0x00,&act,1);
}


int main()
{
	P1M1=0x03;
	P1M0=0x00;
	P3M0=0x00;
	P3M1=0x00;

	AdcInit();
	delay_ms(300);
	AdcExcute();
	while(result1==1024)
	{
		delay_ms(500);
		AdcExcute();
	}
	while(result2==1024)
	{
		delay_ms(500);
		AdcExcute();
	}
	if(result1<600)result1=650;
	if(result2<600)result2=650;
	result1=result1-600;
	result2=result2-600;
	EEPROM_read_n(0x0000,&act,1);
	if(act>Maxact)act=0;
	if(act<0)act=0;
	P1=0XFF;
	while(1)
	{
		if((X1!=0)&&(X2!=0))
		{
			delay_ms(5);
			if(X0==0)
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
				if(X0==0)
				{
					while(1)
					{
						if(X0!=0)break;
					}
				}
			}
			if(X3==0)
			{
				if(X3X==1)
				{
					act++;
					if(act>Maxact)act=0;
					EEPROM_SectorErase(0x0000);
					delay_ms(10);
					EEPROM_write_n(0x0000,&act,1);
					delay_ms(100);
					IAP_CONTR=0x60;
				}
			}
		}
		X3X=X3;
	}
}
	