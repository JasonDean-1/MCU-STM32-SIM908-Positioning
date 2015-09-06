/*********************************************************************************************************
** �ļ����� ��STM32��ʱ����������
** �������� ��FengZhaoHong
** ����ʱ��	��20140828
*********************************************************************************************************/
#include "timer.h"

//�����ⲿ����
extern volatile u8 QS;        // becomes true when Arduoino finds a beat
extern u16 adcx[M][N];				//M·ͨ����N��ADCת��ֵ
extern u16 adcx_avg[M];				//M·ͨ����N��ADCת��ֵ��ƽ��ֵ
extern float adcx_vol[M];			//M·ͨ����N��ADCת��ֵ��ƽ��ֵ��ĵ�ѹֵ

u8 Data_Buffer[4]={0};     		//�洢������ֵת��Ϊ�ַ����������

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
** �������� ����ʼ��ͨ�ö�ʱ��3
** ����˵�� ������ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M�����������ж�
** ��ڲ��� ��arr���Զ���װֵ
**						psc��ʱ��Ԥ��Ƶ��
** ���ڲ��� ����
*********************************************************************************************************/
void TIM3_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);				 	//ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; 											//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 										//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 			//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 							//����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 										//ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  							//TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  		//��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  					//�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 							//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  															//��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  																			//ʹ��TIMx					 
}

/*********************************************************************************************************
** �������� ����ʱ��3�жϷ������
** ����˵�� ��ÿ2ms�ж�һ�Σ���ȡADֵ����������ֵ
** ��ڲ��� ����
** ���ڲ��� ����
*********************************************************************************************************/
void TIM3_IRQHandler(void)													
{
	u16 t;
	u8 i;
	//keep a running total of the last 10 IBI values
	u16 runningTotal=0; 	              	 									  //clear the runningTotal variable    

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  			//���TIM3�����жϷ������
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  						//���TIMx�����жϱ�־ 
		
		TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);               //disable interrupts while we do this
		
		adc_buffer();																						//��ȡDMA�洢���洢��ADת��ֵ
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
					BPM=200;																					//����BPM�����ʾֵ
				if(BPM<30)
					BPM=30;																						//����BPM�����ʾֵ
				Data_Buffer[2]=BPM%10+48;														//ȡ��λ��
				Data_Buffer[1]=BPM%100/10+48; 											//ȡʮλ��
				Data_Buffer[0]=BPM/100+48;	   											//��λ��
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












