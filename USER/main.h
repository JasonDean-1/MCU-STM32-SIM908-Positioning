#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "adc.h"
#include "timer.h"
#include "oled.h"
#include "hc05.h"
#include "usart2.h"
#include "rtc.h"
#include "usmart.h"
#include "tsensor.h"
#include "dma.h"

//�����ⲿ����
volatile u8 QS = false;    	//becomes true when Arduoino finds a beat
extern u8 Data_Buffer[4];		//used to store the pulse rate which is transformed to the char
extern u16 adcx[N][M];			//M·ͨ����N��ADCת��ֵ
extern u16 adcx_avg[M];			//M·ͨ����N��ADCת��ֵ��ƽ��ֵ

void System_Init(void)
{
	delay_init();	    	 																					//��ʱ������ʼ��	  
	NVIC_Configuration(); 	 																			//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 																						//���ڳ�ʼ��Ϊ9600
	LED_Init();			     																					//LED�˿ڳ�ʼ��
	KEY_Init();																										//������ʼ��
	ADC_INIT();		  																							//ADC��ʼ��
	TIM3_Init(19,7199);																						//��ʱ����ʼ����2ms��ʱ���ж�һ��
	OLED_Init();																									//��ʼ��OLED
	RTC_Init();	  																								//RTC��ʼ��
	ADC1_DMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)adcx,M*N);	//DMA1�ĸ�ͨ������
	ADC1_DMA_Enable(DMA1_Channel1);																//����DMA����
	usmart_dev.init(SystemCoreClock/1000000);											//��ʼ��USMART
// 	while(HC05_Init());	 																					//��ʼ��ATK-HC05ģ��
// 	while(HC05_Set_Cmd("AT+ROLE=0"));															//����ATK-HC05ģ��Ϊ�ӻ�
}

void Display_Static_Time()
{	
	OLED_Show_Chinese(0,0,0,1);			//��
	OLED_Show_Chinese(16,0,1,1);		//��
	
	OLED_ShowString(32,0,"SIM908"); //SIM908
	
	OLED_Show_Chinese(80,0,2,1);		//��
	OLED_Show_Chinese(96,0,3,1);		//��
	OLED_Show_Chinese(112,0,4,1);		//��
	OLED_Show_Chinese(0,16,5,1);		//��
	OLED_Show_Chinese(16,16,6,1);		//��
	OLED_Show_Chinese(32,16,7,1);		//��
	OLED_Show_Chinese(48,16,8,1);		//��
	OLED_Show_Chinese(64,16,9,1);		//��
	OLED_Show_Chinese(80,16,10,1);	//λ
	OLED_Show_Chinese(96,16,11,1);	//ϵ
	OLED_Show_Chinese(112,16,12,1);	//ͳ
	
	OLED_ShowChar(32,32,'-',16,1);	//-
	OLED_ShowChar(56,32,'-',16,1);	//-
	
	OLED_Show_Chinese(80,32,15,1);	//��
	OLED_Show_Chinese(96,32,16,1);	//��
	
	OLED_ShowChar(16,48,':',16,1);	//:
	OLED_ShowChar(40,48,':',16,1);	//:
	
	OLED_Show_Chinese(64,48,24,1);	//��
	OLED_Show_Chinese(80,48,25,1);	//��
	OLED_ShowChar(96,48,':',16,1);	//:	
	OLED_Special_Char(120,48,0,1);	//���϶ȷ���
	
// 	OLED_Show_Chinese(24,48,8,1);		//��
// 	OLED_Show_Chinese(40,48,9,1);		//��
// 	OLED_ShowChar(56,48,':',16,1);	//:
// 	OLED_ShowString(88,48,"bpm");		//bpm
}

void Display_Static_Heart_Rate(void)
{
	OLED_Show_Chinese(16,0,13,1);	 	//��
	OLED_Show_Chinese(32,0,14,1);	 	//��
	OLED_Show_Chinese(48,0,7,1);	 	//��
	OLED_Show_Chinese(64,0,8,1);	 	//��
	OLED_Show_Chinese(80,0,29,1);	 	//ģ
	OLED_Show_Chinese(96,0,30,1);	 	//ʽ
	OLED_Show_Chinese(32,32,13,1); 	//��
	OLED_Show_Chinese(48,32,14,1);	//��
	OLED_ShowChar(64,32,':',16,1);	//:
}

void Display_Static_Pedomter(void)
{
	OLED_Show_Chinese(16,0,5,1);	 	//��
	OLED_Show_Chinese(32,0,6,1);	 	//��
	OLED_Show_Chinese(48,0,7,1);	 	//��
	OLED_Show_Chinese(64,0,8,1);	 	//��
	OLED_Show_Chinese(80,0,29,1);	 	//ģ
	OLED_Show_Chinese(96,0,30,1);	 	//ʽ
	OLED_Show_Chinese(24,16,26,1);	//��
	OLED_Show_Chinese(40,16,27,1);	//��
	OLED_ShowChar(56,16,':',16,1);	//:
}

void Update_Time(void)
{
	static u8 second;															//�洢��һ�θ���ϵͳ��ʾʱ��ʱ��ȡ��RTCCNT�Ĵ�����������
	
	if(second!=calendar.sec)
	{
		second=calendar.sec;												//����������
		
		OLED_ShowNum(0,32,calendar.w_year,4,16);		//��
		OLED_ShowNum(40,32,calendar.w_month,2,16);	//��					  
		OLED_ShowNum(64,32,calendar.w_date,2,16);		//��

		switch(calendar.week)
		{
			case 0:	OLED_Show_Chinese(112,32,17,1);		//�գ������գ�
							break;
			case 1:	OLED_Show_Chinese(112,32,18,1);		//�������ڶ���
							break;
			case 2:	OLED_Show_Chinese(112,32,19,1);		//������������
							break;
			case 3:	OLED_Show_Chinese(112,32,20,1);		//�ģ������ģ�
							break;
			case 4:	OLED_Show_Chinese(112,32,21,1);		//�壨�����壩
							break;
			case 5:	OLED_Show_Chinese(112,32,22,1);		//������������
							break;
			case 6:	OLED_Show_Chinese(112,32,23,1);		//һ������һ��
							break;  
		}
		OLED_ShowNum(0,48,calendar.hour,2,16);			//ʱ
		OLED_ShowNum(24,48,calendar.min,2,16);			//��
		OLED_ShowNum(48,48,calendar.sec,2,16);			//��
	}	
}

void Update_Temperture(void)
{
	u16 adc_val;																						//�洢ADC16ת����ĵ���ֵ
	float temperature;																			//�洢�ڲ��¶ȴ��������¶�ֵ
	adc_val=adcx_avg[1];																		//��ȡADCת��ֵ
	temperature=T_Get_Temperture(adc_val);									//�����¶�ֵ��δУ׼��
	temperature=T_Get_Calibrated_Temperature(temperature);	//У׼�¶�ֵ
	OLED_ShowNum(104,48,(u8)temperature,2,16); 							//��ʾ�¶���������
}

void Update_Heart_Rate(void)
{
	char SEND_buffer[7]="H000R\r\n";	//�������ݷ��ͻ�����
	
	if(QS==true)
	{
		OLED_ShowString(72,32,Data_Buffer);	//OLED��ʾ����ֵ
		
		SEND_buffer[1]=Data_Buffer[0];			//������ֵ���뷢�ͻ�����
		SEND_buffer[2]=Data_Buffer[1];
		SEND_buffer[3]=Data_Buffer[2];
		
		printf(SEND_buffer);							//��������ֵͨ��USART1)
	}
}

void Update_Pedometer(void)
{
	u8 reclen;																								//��Ž��ܵ������ݳ���
	u8 Step_Number[4];																				//�������ݷ��ͻ�����

	if(USART_RX_STA&0x8000)
	{
		reclen=USART_RX_STA&0x3fff;															//�õ��˴ν��յ������ݳ���
		if(USART_RX_BUF[0]=='S' && USART_RX_BUF[reclen-1]=='N')
		{
			Step_Number[0]=USART_RX_BUF[1];												//������ֵ���뷢�ͻ�����
			Step_Number[1]=USART_RX_BUF[2];
			Step_Number[2]=USART_RX_BUF[3];
			Step_Number[3]=USART_RX_BUF[4];
			
			OLED_ShowChar(64,16,Step_Number[0],16,1);							//OLED��ʾ�������Ĳ���ֵ
			OLED_ShowChar(72,16,Step_Number[1],16,1);
			OLED_ShowChar(80,16,Step_Number[2],16,1);
			OLED_ShowChar(88,16,Step_Number[3],16,1);
		}
		USART_RX_STA=0;
	}
}
