/*********************************************************************************************************
** ���̹��� ������2+DMA+��ʱ��4��������
** �������� ��FengZhaoHong
** ����ʱ��	��20140828
*********************************************************************************************************/
#ifndef __USART2_H
#define __USART2_H	 

#include "sys.h"  	   
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"

#define USART2_MAX_RECV_LEN		200					//�����ջ����ֽ���
#define USART2_MAX_SEND_LEN		200					//����ͻ����ֽ���
#define USART2_RX_EN 					1						//0,������;1,����

extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern u16 USART2_RX_STA;   											//��������״̬

void USART2_Init(u32 bound);																					//��ʼ������2
void u2_printf(char* fmt, ...);																				//����2,printf ����
void TIM4_Set(u8 sta);																								//����TIM4�Ŀ���																							
void TIM4_Init(u16 arr,u16 psc);																			//ͨ�ö�ʱ��4��ʼ��
void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar);	//DMA1�ĸ�ͨ������
void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u8 len);							//����һ��DMA����

#endif
