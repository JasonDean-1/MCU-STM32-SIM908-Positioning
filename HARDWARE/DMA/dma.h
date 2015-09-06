/*********************************************************************************************************
** 文件功能 ：STM32 DMA驱动程序
** 工程作者 ：FengZhaoHong
** 创建时间	：20140830
*********************************************************************************************************/
#ifndef __DMA_H
#define	__DMA_H	   

#include "sys.h"

void ADC1_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);	//DMA1的各通道配置
void ADC1_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx);															//开启DMA传输

#endif
