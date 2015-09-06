/*********************************************************************************************************
** �ļ����� ��STM32 DMA��������
** �������� ��FengZhaoHong
** ����ʱ��	��20140830
*********************************************************************************************************/
#include "dma.h"

/*********************************************************************************************************
** �������� ��DMA1�ĸ�ͨ������
** ����˵�� ������Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
**            �Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
**						DMA_CHx:DMAͨ��CHx
**						cpar:�����ַ
**						cmar:�洢����ַ
**						cndtr:���ݴ����� 
** ��ڲ��� ����
** ���ڲ��� ����
*********************************************************************************************************/	
void ADC1_DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
	
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);													//ʹ��DMA����
	
  DMA_DeInit(DMA_CHx);   																										//��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	
	DMA_InitStructure.DMA_PeripheralBaseAddr=cpar;  													//DMA����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr=cmar;  															//DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;  												//���ݴ��䷽�򣬴������ȡ���͵��ڴ�
	DMA_InitStructure.DMA_BufferSize=cndtr;  																	//DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;  					//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;  										//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord; //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord; 				//���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;  														//������ѭ��ģʽ
	DMA_InitStructure.DMA_Priority=DMA_Priority_High; 												//DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;  															//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx,&DMA_InitStructure);  																		//����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���	
} 

/*********************************************************************************************************
** �������� ������DMA����
** ����˵�� ����
** ��ڲ��� ��DMA_CHx��DMAͨ��
** ���ڲ��� ����
*********************************************************************************************************/	
void ADC1_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 	
	DMA_Cmd(DMA_CHx,DISABLE);  							//�ر�ADC1 DMA1 ��ָʾ��ͨ��
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);	//ʹ��ָ����ADC1�����ת����������	
 	DMA_Cmd(DMA_CHx,ENABLE);  							//ʹ��ADC1 DMA1 ��ָʾ��ͨ��
}	  
