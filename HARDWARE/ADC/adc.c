/*********************************************************************************************************
** 文件功能 ：STM32内置ADC驱动程序
** 工程作者 ：FengZhaoHong
** 创建时间	：20140828
*********************************************************************************************************/
#include "adc.h"

u16 adcx[N][M];				//M路通道各N个ADC转换值
u16 adcx_avg[M];			//M路通道各N个ADC转换值的平均值

/*********************************************************************************************************
** 函数功能 ：初始化内置ADC
** 函数说明 ：规则通道1、2
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************/																   
void ADC_INIT(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);			//使能ADC1通道时钟
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   																					//设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;																		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1); 																														//复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;															//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode=ENABLE;																		//模数转换工作在扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;															//模数转换工作在连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;							//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;													//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel=M;																					//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1,&ADC_InitStructure);																						//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_239Cycles5);			//ADC1,ADC_Channel_1通道,采样时间为239.5周期
	ADC_RegularChannelConfig(ADC1,ADC_Channel_16,2,ADC_SampleTime_239Cycles5);		//ADC1,ADC_Channel_16通道,采样时间为239.5周期
	
	ADC_TempSensorVrefintCmd(ENABLE); 																						//开启内部温度传感器
	ADC_Cmd(ADC1,ENABLE);																													//使能指定的ADC1
	ADC_DMACmd(ADC1,ENABLE);																											//使能ADC的DMA传输
	
	ADC_ResetCalibration(ADC1);																										//使能复位校准  
	while(ADC_GetResetCalibrationStatus(ADC1));																		//等待复位校准结束
	ADC_StartCalibration(ADC1);	 																									//开启AD校准
	while(ADC_GetCalibrationStatus(ADC1));	 																			//等待校准结束
}				

/*********************************************************************************************************
** 函数功能 ：读取DMA存储器存储的AD转换值
** 函数说明 ：ADC1的ADC_Channel_1和ADC_Channel_16两路通道的AD转换值通过DMA写到虚拟的adcx[N][M]存储器上
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************/
void adc_buffer(void)
{
	int sum=0;
	u8 i,j;
	for(i=0;i<M;i++)
	{
		for(j=0;j<N;j++)
			sum+=adcx[j][i];
    adcx_avg[i]=sum/N;
    sum=0;
  }
}
