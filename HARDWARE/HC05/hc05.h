/*********************************************************************************************************
** 工程功能 ：ATK-HC05蓝牙模块驱动代码
** 工程作者 ：FengZhaoHong
** 创建时间	：20140828
*********************************************************************************************************/
#ifndef __HC05_H
#define __HC05_H	 

#include "sys.h"  
#include "delay.h" 			 		 
#include "usart2.h" 

#define HC05_KEY  	PCout(4) 	//蓝牙控制KEY信号
#define HC05_LED  	PAin(4)		//蓝牙连接状态信号

u8 HC05_Init(void);						//初始化ATK-HC05模块
u8 HC05_Get_Role(void);				//获取ATK-HC05模块的角色
u8 HC05_Set_Cmd(u8* atstr);	 	//ATK-HC05设置命令
void HC05_CFG_CMD(u8 *str);		//通过该函数,可以利用USMART,调试接在串口2上的ATK-HC05模块

#endif  
