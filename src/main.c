#include "C:\\Users\\Administrator\\Documents\\Source\\MCU\\Sdcc-51\\src\\STC15Fxxxx.H"
#include "C:\\Users\\Administrator\\Documents\\Source\\MCU\\Sdcc-51\\src\\USART.h"
#include <intrins.h>
#include "C:\\Users\\Administrator\\Documents\\Source\\MCU\\Sdcc-51\\src\\EEPROM.h"
#include "C:\\Users\\Administrator\\Documents\\Source\\MCU\\Sdcc-51\\src\\adc.h"

static int Maxact=3;
static u8 Makeup1,Makeup2;

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
int UARTInit();
int UARTRead();
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
u16 addr;

int main()
{
	AdcInit();
	delay_ms(100);
	
	if(result1==1024)
	{
		while(1)
		{
			delay_ms(200);
			//TxSend(0x99);
			delay_ms(50);
			//TxSend((u8)result1);
			delay_ms(50);
			//TxSend((u8)result2);
		}
	}
	if(result2==1024)
	{
		while(1)
		{
			delay_ms(200);
			//TxSend(0x99);
			delay_ms(50);
			//TxSend((u8)result1);
			delay_ms(50);
			//TxSend((u8)result2);
		}
	}
	EEPROM_read_n(0x0010,&Makeup1,1);
	EEPROM_read_n(0x11,&Makeup2,1);
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
				AdcExcute();
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
		EEPROMend();
		UARTRead();
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
	Get_ADC10bitResult(0);
	delay_ms(50);
	result1=Get_ADC10bitResult(0);
	delay_ms(50);
	Get_ADC10bitResult(1);
	delay_ms(50);
	result2=Get_ADC10bitResult(1);
	result1=result1-Makeup1;
	result2=result2-Makeup2;
}


int UARTInit()
{
	COMx_InitDefine		COMx_InitStructure;					
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			
	COMx_InitStructure.UART_BaudRate  = 115200ul;			
	COMx_InitStructure.UART_RxEnable  = ENABLE;				
	COMx_InitStructure.BaudRateDouble = DISABLE;			
	COMx_InitStructure.UART_Interrupt = ENABLE;				
	COMx_InitStructure.UART_Polity    = PolityLow;			
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		
	USART_Configuration(USART1, &COMx_InitStructure);		
}


int UARTRead()
{
	if(COM1.RX_TimeOut > 0)	
	{
		if(--COM1.RX_TimeOut == 0)
		{
			switch (RX1_Buffer[0])
			{
			case 'D':
				IAP_CONTR=0x60;
				break;
			case 'u':
				Makeup1+=10;
				break;
			case 'i':
				Makeup1-+10;
				break;
			case 'j':
				Makeup2+=10;
				break;
			case 'k':
				Makeup2-+10;
				break;
			default:
				break;
			}

			COM1.RX_Cnt = 0;
		}
	}
}

int EEPROMend()
{
	EEPROM_SectorErase(0x00);
	delay_ms(5);
	EEPROM_write_n(0x10,&Makeup1,1);
	delay_ms(5);
	EEPROM_write_n(0x11,&Makeup2,1);
	delay_ms(5);
	EEPROM_write_n(0x00,&act,1);
}