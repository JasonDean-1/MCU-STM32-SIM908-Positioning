/*********************************************************************************************************
** 文件功能 ：STM32 DMA驱动程序
** 工程作者 ：FengZhaoHong
** 创建时间	：20140830
*********************************************************************************************************/
#include "dma.h"

/*********************************************************************************************************
** 函数功能 ：DMA1的各通道配置
** 函数说明 ：这里的传输形式是固定的,这点要根据不同的情况来修改
**            从存储器->外设模式/8位数据宽度/存储器增量模式
**						DMA_CHx:DMA通道CHx
**						cpar:外设地址
**						cmar:存储器地址
**						cndtr:数据传输量 
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************/	
void ADC1_DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
	
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);													//使能DMA传输
	
  DMA_DeInit(DMA_CHx);   																										//将DMA的通道1寄存器重设为缺省值
	
	DMA_InitStructure.DMA_PeripheralBaseAddr=cpar;  													//DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr=cmar;  															//DMA内存基地址
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;  												//数据传输方向，从外设读取发送到内存
	DMA_InitStructure.DMA_BufferSize=cndtr;  																	//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;  					//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;  										//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord; //数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord; 				//数据宽度为16位
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;  														//工作在循环模式
	DMA_InitStructure.DMA_Priority=DMA_Priority_High; 												//DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;  															//DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx,&DMA_InitStructure);  																		//根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器	
} 

/*********************************************************************************************************
** 函数功能 ：开启DMA传输
** 函数说明 ：无
** 入口参数 ：DMA_CHx：DMA通道
** 出口参数 ：无
*********************************************************************************************************/	
void ADC1_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 	
	DMA_Cmd(DMA_CHx,DISABLE);  							//关闭ADC1 DMA1 所指示的通道
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);	//使能指定的ADC1的软件转换启动功能	
 	DMA_Cmd(DMA_CHx,ENABLE);  							//使能ADC1 DMA1 所指示的通道
}	  
