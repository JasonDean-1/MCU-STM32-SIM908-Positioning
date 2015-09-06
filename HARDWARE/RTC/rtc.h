/*********************************************************************************************************
** 工程功能 ：RTC实时时钟驱动代码
** 工程作者 ：FengZhaoHong
** 创建时间	：20140829
*********************************************************************************************************/
#ifndef __RTC_H
#define __RTC_H	    

#include "sys.h"
#include "delay.h"

//时间结构体
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//公历日月年周
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;		 
}_calendar_obj;			

extern _calendar_obj calendar;	//日历结构体

u8 RTC_Init(void);       																			//实时时钟配置，初始化RTC时钟,同时检测时钟是否工作正常
static void RTC_NVIC_Config(void);														//初始化RTC中断向量寄存器
u8 Is_Leap_Year(u16 year);																		//判断是否是闰年函数
extern u8 const mon_table[12];																//月份日期数据表
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);	//设置时钟
u8 RTC_Get(void);         																		//得到当前的时间
u8 RTC_Get_Week(u16 year,u8 month,u8 day);										//获得现在是星期几
		 
#endif
