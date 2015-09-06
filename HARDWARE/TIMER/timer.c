/*********************************************************************************************************
** 文件功能 ：STM32定时器驱动程序
** 工程作者 ：FengZhaoHong
** 创建时间	：20140828
*********************************************************************************************************/
#include "timer.h"

//声明外部变量
extern volatile u8 QS;        // becomes true when Arduoino finds a beat
extern u16 adcx[M][N];				//M路通道各N个ADC转换值
extern u16 adcx_avg[M];				//M路通道各N个ADC转换值的平均值
extern float adcx_vol[M];			//M路通道各N个ADC转换值的平均值后的电压值

u8 Data_Buffer[4]={0};     		//存储心率数值转换为字符串后的数组

// these variables are volatile because they are used during the interrupt service routine!
volatile u16 		BPM;                   				// used to hold the pulse rate
volatile u16 		Signal;                				// holds the incoming raw data
volatile u16 		IBI 					= 600;          // holds the time between beats, must be seeded! 
volatile u8 		Pulse 				= false;     		// true when pulse wave is high, false when it's low
volatile u16 		rate[10];                    	// array to hold last ten IBI values
volatile u32 		sampleCounter = 0;     				// used to determine pulse timing
volatile u32 		lastBeatTime 	= 0;      			// used to find IBI
volatile u16 		Peak 					= 2048;         // used to find peak in pulse wave, seeded
volatile u16 		Trough 				= 2048;         // used to find trough in pulse wave, seeded
volatile u16 		thresh 				= 2048;         // used to find instant moment of heart beat, seeded
volatile u16 		amp 					= 400;          // used to hold amplitude of pulse waveform, seeded
volatile u8 		firstBeat 		= true;        	// used to seed rate array so we startup with reasonable BPM
volatile u8 		secondBeat 		= false;      	// used to seed rate array so we startup with reasonable BPM

/*********************************************************************************************************
** 函数功能 ：初始化通用定时器3
** 函数说明 ：这里时钟选择为APB1的2倍，而APB1为36M，开启更新中断
** 入口参数 ：arr：自动重装值
**						psc：时钟预分频数
** 出口参数 ：无
*********************************************************************************************************/
void TIM3_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);				 	//时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; 											//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 										//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 			//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 							//根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 										//使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  							//TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  		//先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  					//从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 							//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  															//初始化NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  																			//使能TIMx					 
}

/*********************************************************************************************************
** 函数功能 ：定时器3中断服务程序
** 函数说明 ：每2ms中断一次，读取AD值，计算心率值
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************/
void TIM3_IRQHandler(void)													
{
	u16 t;
	u8 i;
	//keep a running total of the last 10 IBI values
	u16 runningTotal=0; 	              	 									  //clear the runningTotal variable    

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  			//检查TIM3更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  						//清除TIMx更新中断标志 
		
		TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);               //disable interrupts while we do this
		
		adc_buffer();																						//读取DMA存储器存储的AD转换值
		Signal=adcx_avg[0];             												//read the Pulse Sensor
		sampleCounter+=2;                         							//keep track of the time in mS with this variable
		t=sampleCounter-lastBeatTime;       										//monitor the time since the last beat to avoid noise

		//find the peak and trough of the pulse wave
		if(Signal<thresh && t>(IBI/5)*3)												//avoid dichrotic noise by waiting 3/5 of last IBI			
			if(Signal<Trough)																						                      																				
				Trough=Signal;                         							//keep track of lowest point in pulse wave 

		if(Signal>thresh && Signal>Peak)												//thresh condition helps avoid noise      										
			Peak=Signal;                             							//keep track of highest point in pulse wave                                     													

		//NOW IT'S TIME TO LOOK FOR THE HEART BEAT
		//signal surges up in value every time there is a pulse
		if (t>250)																							//avoid high frequency noise
		{                                   									
			if((Signal>thresh)&&(Pulse==false)&&(t>(IBI/5)*3))
			{        
				Pulse=true;                              		 				//set the Pulse flag when we think there is a pulse
				IBI=sampleCounter-lastBeatTime;         						//measure time between beats in mS
				lastBeatTime=sampleCounter;               					//keep track of time for next pulse

				if(secondBeat)																			//if this is the second beat, if secondBeat == TRUE
				{                        												
					secondBeat=false;                  								//clear secondBeat flag
					for(i=0;i<=9;i++)																	//seed the running total to get a realisitic BPM at startup
					{             														
						rate[i]=IBI;                      
					}
				}

				if(firstBeat)																				//if it's the first time we found a beat, if firstBeat == TRUE
				{                         												
					firstBeat=false;                   								//clear firstBeat flag
					secondBeat=true;                   								//set the second beat flag
					TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);      		//enable interrupts again
					return;                             	 						//IBI value is unreliable so discard it
				}   
				
				for(i=0;i<=8;i++)																		//shift data in the rate array
				{
					rate[i]=rate[i+1];             										//and drop the oldest IBI value 
					runningTotal+=rate[i];              							//add up the 9 oldest IBI values
				}

				rate[9]=IBI;                      				    			//add the latest IBI to the rate array
				runningTotal+=rate[9];                							//add the latest IBI to runningTotal
				runningTotal/=10;                     							//average the last 10 IBI values 
				BPM=60000/runningTotal;               							//how many beats can fit into a minute? that's BPM!
				if(BPM>200)
					BPM=200;																					//限制BPM最高显示值
				if(BPM<30)
					BPM=30;																						//限制BPM最低显示值
				Data_Buffer[2]=BPM%10+48;														//取个位数
				Data_Buffer[1]=BPM%100/10+48; 											//取十位数
				Data_Buffer[0]=BPM/100+48;	   											//百位数
				if(Data_Buffer[0]==48)
					Data_Buffer[0]=' ';
				Data_Buffer[3]='\0';
				QS = true;                              						//set Quantified Self flag 
			}                       
		}

		if(Signal<thresh && Pulse==true)	//when the values are going down, the beat is over
		{   
			Pulse  = false;                 //reset the Pulse flag so we can do it again
			amp    = Peak - Trough;         //get amplitude of the pulse wave
			thresh = amp/2 + Trough;        //set thresh at 50% of the amplitude
			Peak   = thresh;                //reset these for next time
			Trough = thresh;
		}

		if(t>2500)											//if 2.5 seconds go by without a beat
		{                           
			thresh       = 2048;          //set thresh default
			Peak         = 2048;          //set P default
			Trough       = 2048;          //set T default
			lastBeatTime = sampleCounter; //bring the lastBeatTime up to date        
			firstBeat    = true;          //set these to avoid noise
			secondBeat   = false;         //when we get the heartbeat back
		}
		
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);	//enable interrupts when youre done!
	}
}












