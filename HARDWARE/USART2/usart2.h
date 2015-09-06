/*********************************************************************************************************
** 工程功能 ：串口2+DMA+定时器4驱动代码
** 工程作者 ：FengZhaoHong
** 创建时间	：20140828
*********************************************************************************************************/
#ifndef __USART2_H
#define __USART2_H	 

#include "sys.h"  	   
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"

#define USART2_MAX_RECV_LEN		200					//最大接收缓存字节数
#define USART2_MAX_SEND_LEN		200					//最大发送缓存字节数
#define USART2_RX_EN 					1						//0,不接收;1,接收

extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节
extern u16 USART2_RX_STA;   											//接收数据状态

void USART2_Init(u32 bound);																					//初始化串口2
void u2_printf(char* fmt, ...);																				//串口2,printf 函数
void TIM4_Set(u8 sta);																								//设置TIM4的开关																							
void TIM4_Init(u16 arr,u16 psc);																			//通用定时器4初始化
void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar);	//DMA1的各通道配置
void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u8 len);							//开启一次DMA传输

#endif
