/*********************************************************************************************************
** �ļ����� ��STM32 DMA��������
** �������� ��FengZhaoHong
** ����ʱ��	��20140830
*********************************************************************************************************/
#ifndef __DMA_H
#define	__DMA_H	   

#include "sys.h"

void ADC1_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);	//DMA1�ĸ�ͨ������
void ADC1_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx);															//����DMA����

#endif
