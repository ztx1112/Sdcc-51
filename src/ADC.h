
#ifndef	__ADC_H
#define	__ADC_H

#include	"config.h"
#include "STC15Fxxxx.H"

#define	ADC_P10		0x01	//IO���� Px.0
#define	ADC_P11		0x02	//IO���� Px.1
#define	ADC_P12		0x04	//IO���� Px.2
#define	ADC_P13		0x08	//IO���� Px.3
#define	ADC_P14		0x10	//IO���� Px.4
#define	ADC_P15		0x20	//IO���� Px.5
#define	ADC_P16		0x40	//IO���� Px.6
#define	ADC_P17		0x80	//IO���� Px.7
#define	ADC_P1_All	0xFF	//IO��������

#define ADC_90T		(3<<5)
#define ADC_180T	(2<<5)
#define ADC_360T	(1<<5)
#define ADC_540T	0
#define ADC_FLAG	(1<<4)	//������0
#define ADC_START	(1<<3)	//�Զ���0
#define ADC_CH0		0
#define ADC_CH1		1
#define ADC_CH2		2
#define ADC_CH3		3
#define ADC_CH4		4
#define ADC_CH5		5
#define ADC_CH6		6
#define ADC_CH7		7

#define ADC_RES_H2L8	1
#define ADC_RES_H8L2	0


typedef struct
{
	u8	ADC_Px;			//����Ҫ��ADC��IO,	ADC_P10 ~ ADC_P17,ADC_P1_All
	u8	ADC_Speed;		//ADC�ٶ�			ADC_90T,ADC_180T,ADC_360T,ADC_540T
	u8	ADC_Power;		//ADC��������/�ر�	ENABLE,DISABLE
	u8	ADC_AdjResult;	//ADC�������,	ADC_RES_H2L8,ADC_RES_H8L2
	u8	ADC_Polity;		//���ȼ�����	PolityHigh,PolityLow
	u8	ADC_Interrupt;	//�ж�����		ENABLE,DISABLE
} ADC_InitTypeDef;

void	ADC_Inilize(ADC_InitTypeDef *ADCx);
void	ADC_PowerControl(u8 pwr);
u16		Get_ADC10bitResult(u8 channel);	//channel = 0~7

#endif
