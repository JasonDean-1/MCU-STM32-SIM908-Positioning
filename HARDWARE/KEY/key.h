/*********************************************************************************************************
** 文件功能 ：STM32 KEY驱动代码	  
** 工程作者 ：FengZhaoHong
** 创建时间	：20140830
*********************************************************************************************************/
#ifndef __KEY_H
#define __KEY_H	 

#include "sys.h"
#include "delay.h"

#define KEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)	//读取按键0
#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	//读取按键1
#define KEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)	//读取按键2 
#define KEY3  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//读取按键3(WK_UP) 

#define KEY_UP 		4
#define KEY_LEFT	3
#define KEY_DOWN	2
#define KEY_RIGHT	1

void KEY_Init(void);	//IO初始化
u8 KEY_Scan(u8);  		//按键扫描函数

#endif
