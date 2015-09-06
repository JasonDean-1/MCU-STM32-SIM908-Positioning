/*********************************************************************************************************
** 文件功能 ：STM32内置ADC驱动程序
** 工程作者 ：FengZhaoHong
** 创建时间	：20140828
*********************************************************************************************************/
#ifndef __ADC_H
#define __ADC_H	

#include "sys.h"

#define M 2 		//2路ADC
#define N 50 		//取50次平均值

void ADC_INIT(void);												//初始化内置ADC
u16 Get_ADC(u8 channel); 										//获取ADC对应通道的电压值
u16 Get_ADC_Average(u8 channel,u8 times); 	//获取ADC对应通道的平均电压值
void adc_buffer(void);											//读取DMA存储器存储的AD转换值

#endif 
