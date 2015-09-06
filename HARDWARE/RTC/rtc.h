/*********************************************************************************************************
** ���̹��� ��RTCʵʱʱ����������
** �������� ��FengZhaoHong
** ����ʱ��	��20140829
*********************************************************************************************************/
#ifndef __RTC_H
#define __RTC_H	    

#include "sys.h"
#include "delay.h"

//ʱ��ṹ��
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//������������
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;		 
}_calendar_obj;			

extern _calendar_obj calendar;	//�����ṹ��

u8 RTC_Init(void);       																			//ʵʱʱ�����ã���ʼ��RTCʱ��,ͬʱ���ʱ���Ƿ�������
static void RTC_NVIC_Config(void);														//��ʼ��RTC�ж������Ĵ���
u8 Is_Leap_Year(u16 year);																		//�ж��Ƿ������꺯��
extern u8 const mon_table[12];																//�·��������ݱ�
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);	//����ʱ��
u8 RTC_Get(void);         																		//�õ���ǰ��ʱ��
u8 RTC_Get_Week(u16 year,u8 month,u8 day);										//������������ڼ�
		 
#endif
