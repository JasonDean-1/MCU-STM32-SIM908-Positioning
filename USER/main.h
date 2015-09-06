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

//声明外部变量
volatile u8 QS = false;    	//becomes true when Arduoino finds a beat
extern u8 Data_Buffer[4];		//used to store the pulse rate which is transformed to the char
extern u16 adcx[N][M];			//M路通道各N个ADC转换值
extern u16 adcx_avg[M];			//M路通道各N个ADC转换值的平均值

void System_Init(void)
{
	delay_init();	    	 																					//延时函数初始化	  
	NVIC_Configuration(); 	 																			//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 																						//串口初始化为9600
	LED_Init();			     																					//LED端口初始化
	KEY_Init();																										//按键初始化
	ADC_INIT();		  																							//ADC初始化
	TIM3_Init(19,7199);																						//定时器初始化，2ms定时器中断一次
	OLED_Init();																									//初始化OLED
	RTC_Init();	  																								//RTC初始化
	ADC1_DMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)adcx,M*N);	//DMA1的各通道配置
	ADC1_DMA_Enable(DMA1_Channel1);																//开启DMA传输
	usmart_dev.init(SystemCoreClock/1000000);											//初始化USMART
// 	while(HC05_Init());	 																					//初始化ATK-HC05模块
// 	while(HC05_Set_Cmd("AT+ROLE=0"));															//设置ATK-HC05模块为从机
}

void Display_Static_Time()
{	
	OLED_Show_Chinese(0,0,0,1);			//基
	OLED_Show_Chinese(16,0,1,1);		//于
	
	OLED_ShowString(32,0,"SIM908"); //SIM908
	
	OLED_Show_Chinese(80,0,2,1);		//的
	OLED_Show_Chinese(96,0,3,1);		//老
	OLED_Show_Chinese(112,0,4,1);		//人
	OLED_Show_Chinese(0,16,5,1);		//活
	OLED_Show_Chinese(16,16,6,1);		//动
	OLED_Show_Chinese(32,16,7,1);		//监
	OLED_Show_Chinese(48,16,8,1);		//测
	OLED_Show_Chinese(64,16,9,1);		//定
	OLED_Show_Chinese(80,16,10,1);	//位
	OLED_Show_Chinese(96,16,11,1);	//系
	OLED_Show_Chinese(112,16,12,1);	//统
	
	OLED_ShowChar(32,32,'-',16,1);	//-
	OLED_ShowChar(56,32,'-',16,1);	//-
	
	OLED_Show_Chinese(80,32,15,1);	//星
	OLED_Show_Chinese(96,32,16,1);	//期
	
	OLED_ShowChar(16,48,':',16,1);	//:
	OLED_ShowChar(40,48,':',16,1);	//:
	
	OLED_Show_Chinese(64,48,24,1);	//气
	OLED_Show_Chinese(80,48,25,1);	//温
	OLED_ShowChar(96,48,':',16,1);	//:	
	OLED_Special_Char(120,48,0,1);	//摄氏度符号
	
// 	OLED_Show_Chinese(24,48,8,1);		//心
// 	OLED_Show_Chinese(40,48,9,1);		//率
// 	OLED_ShowChar(56,48,':',16,1);	//:
// 	OLED_ShowString(88,48,"bpm");		//bpm
}

void Display_Static_Heart_Rate(void)
{
	OLED_Show_Chinese(16,0,13,1);	 	//心
	OLED_Show_Chinese(32,0,14,1);	 	//率
	OLED_Show_Chinese(48,0,7,1);	 	//监
	OLED_Show_Chinese(64,0,8,1);	 	//测
	OLED_Show_Chinese(80,0,29,1);	 	//模
	OLED_Show_Chinese(96,0,30,1);	 	//式
	OLED_Show_Chinese(32,32,13,1); 	//心
	OLED_Show_Chinese(48,32,14,1);	//率
	OLED_ShowChar(64,32,':',16,1);	//:
}

void Display_Static_Pedomter(void)
{
	OLED_Show_Chinese(16,0,5,1);	 	//活
	OLED_Show_Chinese(32,0,6,1);	 	//动
	OLED_Show_Chinese(48,0,7,1);	 	//监
	OLED_Show_Chinese(64,0,8,1);	 	//测
	OLED_Show_Chinese(80,0,29,1);	 	//模
	OLED_Show_Chinese(96,0,30,1);	 	//式
	OLED_Show_Chinese(24,16,26,1);	//步
	OLED_Show_Chinese(40,16,27,1);	//数
	OLED_ShowChar(56,16,':',16,1);	//:
}

void Update_Time(void)
{
	static u8 second;															//存储上一次更新系统显示时间时读取的RTCCNT寄存器的秒钟数
	
	if(second!=calendar.sec)
	{
		second=calendar.sec;												//更新秒钟数
		
		OLED_ShowNum(0,32,calendar.w_year,4,16);		//年
		OLED_ShowNum(40,32,calendar.w_month,2,16);	//月					  
		OLED_ShowNum(64,32,calendar.w_date,2,16);		//日

		switch(calendar.week)
		{
			case 0:	OLED_Show_Chinese(112,32,17,1);		//日（星期日）
							break;
			case 1:	OLED_Show_Chinese(112,32,18,1);		//二（星期二）
							break;
			case 2:	OLED_Show_Chinese(112,32,19,1);		//三（星期三）
							break;
			case 3:	OLED_Show_Chinese(112,32,20,1);		//四（星期四）
							break;
			case 4:	OLED_Show_Chinese(112,32,21,1);		//五（星期五）
							break;
			case 5:	OLED_Show_Chinese(112,32,22,1);		//六（星期六）
							break;
			case 6:	OLED_Show_Chinese(112,32,23,1);		//一（星期一）
							break;  
		}
		OLED_ShowNum(0,48,calendar.hour,2,16);			//时
		OLED_ShowNum(24,48,calendar.min,2,16);			//分
		OLED_ShowNum(48,48,calendar.sec,2,16);			//秒
	}	
}

void Update_Temperture(void)
{
	u16 adc_val;																						//存储ADC16转换后的的数值
	float temperature;																			//存储内部温度传感器的温度值
	adc_val=adcx_avg[1];																		//获取ADC转换值
	temperature=T_Get_Temperture(adc_val);									//计算温度值（未校准）
	temperature=T_Get_Calibrated_Temperature(temperature);	//校准温度值
	OLED_ShowNum(104,48,(u8)temperature,2,16); 							//显示温度整数部分
}

void Update_Heart_Rate(void)
{
	char SEND_buffer[7]="H000R\r\n";	//心率数据发送缓冲区
	
	if(QS==true)
	{
		OLED_ShowString(72,32,Data_Buffer);	//OLED显示心率值
		
		SEND_buffer[1]=Data_Buffer[0];			//把心率值存入发送缓冲区
		SEND_buffer[2]=Data_Buffer[1];
		SEND_buffer[3]=Data_Buffer[2];
		
		printf(SEND_buffer);							//发送心率值通过USART1)
	}
}

void Update_Pedometer(void)
{
	u8 reclen;																								//存放接受到的数据长度
	u8 Step_Number[4];																				//步数数据发送缓冲区

	if(USART_RX_STA&0x8000)
	{
		reclen=USART_RX_STA&0x3fff;															//得到此次接收到的数据长度
		if(USART_RX_BUF[0]=='S' && USART_RX_BUF[reclen-1]=='N')
		{
			Step_Number[0]=USART_RX_BUF[1];												//把心率值存入发送缓冲区
			Step_Number[1]=USART_RX_BUF[2];
			Step_Number[2]=USART_RX_BUF[3];
			Step_Number[3]=USART_RX_BUF[4];
			
			OLED_ShowChar(64,16,Step_Number[0],16,1);							//OLED显示缓冲区的步数值
			OLED_ShowChar(72,16,Step_Number[1],16,1);
			OLED_ShowChar(80,16,Step_Number[2],16,1);
			OLED_ShowChar(88,16,Step_Number[3],16,1);
		}
		USART_RX_STA=0;
	}
}
