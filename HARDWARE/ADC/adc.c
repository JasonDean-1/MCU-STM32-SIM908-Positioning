/*********************************************************************************************************
** �ļ����� ��STM32����ADC��������
** �������� ��FengZhaoHong
** ����ʱ��	��20140828
*********************************************************************************************************/
#include "adc.h"

u16 adcx[N][M];				//M·ͨ����N��ADCת��ֵ
u16 adcx_avg[M];			//M·ͨ����N��ADCת��ֵ��ƽ��ֵ

/*********************************************************************************************************
** �������� ����ʼ������ADC
** ����˵�� ������ͨ��1��2
** ��ڲ��� ����
** ���ڲ��� ����
*********************************************************************************************************/																   
void ADC_INIT(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);			//ʹ��ADC1ͨ��ʱ��
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   																					//����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PA1 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;																		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1); 																														//��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;															//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode=ENABLE;																		//ģ��ת��������ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;															//ģ��ת������������ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;							//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;													//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel=M;																					//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1,&ADC_InitStructure);																						//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_239Cycles5);			//ADC1,ADC_Channel_1ͨ��,����ʱ��Ϊ239.5����
	ADC_RegularChannelConfig(ADC1,ADC_Channel_16,2,ADC_SampleTime_239Cycles5);		//ADC1,ADC_Channel_16ͨ��,����ʱ��Ϊ239.5����
	
	ADC_TempSensorVrefintCmd(ENABLE); 																						//�����ڲ��¶ȴ�����
	ADC_Cmd(ADC1,ENABLE);																													//ʹ��ָ����ADC1
	ADC_DMACmd(ADC1,ENABLE);																											//ʹ��ADC��DMA����
	
	ADC_ResetCalibration(ADC1);																										//ʹ�ܸ�λУ׼  
	while(ADC_GetResetCalibrationStatus(ADC1));																		//�ȴ���λУ׼����
	ADC_StartCalibration(ADC1);	 																									//����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1));	 																			//�ȴ�У׼����
}				

/*********************************************************************************************************
** �������� ����ȡDMA�洢���洢��ADת��ֵ
** ����˵�� ��ADC1��ADC_Channel_1��ADC_Channel_16��·ͨ����ADת��ֵͨ��DMAд�������adcx[N][M]�洢����
** ��ڲ��� ����
** ���ڲ��� ����
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
