/*********************************************************************************************************
** 工程功能 ：2*8脚0.96寸OLED模块SPI和8080通信驱动程序
** 工程作者 ：FengZhaoHong
** 创建时间	：20140828
*********************************************************************************************************/
#include "oled.h"
#include "oledfont.h"

//OLED的显存
//存放格式如下
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   
u8 OLED_GRAM[128][8];	 

/*********************************************************************************************************
** 函数功能 ：OLED模块初始化
** 函数说明 ：初始化OLED模块控制器SSD1306,通过查看厂家提供的资料
** 入口参数 ：无
** 出口参数 ：无 
*********************************************************************************************************/			    
void OLED_Init(void)
{ 	
 	GPIO_InitTypeDef GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG,ENABLE);	 	//使能PC,D,G端口时钟

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_6;	 																							//PD3,PD6推挽输出  
 	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP; 		 																								//推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;																										//速度50MHz
 	GPIO_Init(GPIOD,&GPIO_InitStructure);	  																													//初始化GPIOD3,6
 	GPIO_SetBits(GPIOD,GPIO_Pin_3|GPIO_Pin_6);																												//PD3,PD6 输出高

 #if OLED_MODE==1
 
 	GPIO_InitStructure.GPIO_Pin=0xFF; 																	//PC0~7 OUT推挽输出
 	GPIO_Init(GPIOC,&GPIO_InitStructure);
 	GPIO_SetBits(GPIOC,0xFF); 																					//PC0~7输出高

 	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;		//PG13,14,15 OUT推挽输出
 	GPIO_Init(GPIOG,&GPIO_InitStructure);
 	GPIO_SetBits(GPIOG,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);						//PG13,14,15 OUT  输出高

 #else
 	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;				 					//PC0,1 OUT推挽输出
 	GPIO_Init(GPIOC,&GPIO_InitStructure);
 	GPIO_SetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1);						 							//PC0,1 OUT  输出高

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;				 										//PG15 OUT推挽输出	  RST
 	GPIO_Init(GPIOG,&GPIO_InitStructure);
 	GPIO_SetBits(GPIOG,GPIO_Pin_15);							 											//PG15 OUT  输出高

 #endif
  							  
	OLED_RST_Clr();
	delay_ms(100);
	OLED_RST_Set(); 
					  
	OLED_WR_Byte(0xAE,OLED_CMD); 			//关闭显示
	OLED_WR_Byte(0xD5,OLED_CMD); 			//设置时钟分频因子,震荡频率
	OLED_WR_Byte(0x80,OLED_CMD);   			//[3:0],分频因子;[7:4],震荡频率
	OLED_WR_Byte(0xA8,OLED_CMD); 			//设置驱动路数
	OLED_WR_Byte(0X3F,OLED_CMD); 			//默认0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); 			//设置显示偏移
	OLED_WR_Byte(0X00,OLED_CMD);			//默认为0

	OLED_WR_Byte(0x40,OLED_CMD); 			//设置显示开始行 [5:0],行数.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); 			//电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD); 			//bit2，开启/关闭
	OLED_WR_Byte(0x20,OLED_CMD); 			//设置内存地址模式
	OLED_WR_Byte(0x02,OLED_CMD); 			//[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_WR_Byte(0xA1,OLED_CMD); 			//段重定义设置,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); 			//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	OLED_WR_Byte(0xDA,OLED_CMD); 			//设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD); 			//[5:4]配置
		 
	OLED_WR_Byte(0x81,OLED_CMD); 			//对比度设置
	OLED_WR_Byte(0xEF,OLED_CMD); 			//1~255;默认0X7F (亮度设置,越大越亮)
	OLED_WR_Byte(0xD9,OLED_CMD); 			//设置预充电周期
	OLED_WR_Byte(0xF1,OLED_CMD); 			//[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); 			//设置VCOMH 电压倍率
	OLED_WR_Byte(0x30,OLED_CMD); 			//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); 			//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD); 			//设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); 			//开启显示	 
	
	OLED_Clear();
} 

/*********************************************************************************************************
** 函数功能 ：更新显存到OLED
** 函数说明 ：把在STM32 SRAM上创建的显存一次写到OLED显存上
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************/		 
void OLED_Refresh_Gram(void)
{
	u8 i,j;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte(0xB0+i,OLED_CMD);    							//设置页地址（0~7）
		OLED_WR_Byte(0x00,OLED_CMD);      							//设置显示位置—列低地址
		OLED_WR_Byte(0x10,OLED_CMD);      							//设置显示位置—列高地址   
		for(j=0;j<128;j++)
			OLED_WR_Byte(OLED_GRAM[j][i],OLED_DATA); 
	}   
}

#if OLED_MODE==1
/*********************************************************************************************************
** 函数功能 ：向SSD1306写入一个字节
** 函数说明 ：使用并行8080模式
** 入口参数 ：dat:要写入的数据/命令
**						cmd:数据/命令标志 0,表示命令;1,表示数据
** 出口参数 ：无
*********************************************************************************************************/
void OLED_WR_Byte(u8 dat,u8 cmd)
{
	DATA_OUT(dat);	    
	if(cmd)
	  OLED_RS_Set();
	else 
	  OLED_RS_Clr();		   
	OLED_CS_Clr();
	OLED_WR_Clr();	 
	OLED_WR_Set();
	OLED_CS_Set();	  
	OLED_RS_Set();	 
} 	    
#else
/*********************************************************************************************************
** 函数功能 ：向SSD1306写入一个字节
** 函数说明 ：使用SPI模式
** 入口参数 ：dat:要写入的数据/命令
**						cmd:数据/命令标志 0,表示命令;1,表示数据
** 出口参数 ：无
*********************************************************************************************************/
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	if(cmd)
	  OLED_RS_Set();
	else
	  OLED_RS_Clr();		  
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_RS_Set();   	  
} 
#endif

/*********************************************************************************************************
** 函数功能 ：开启OLED显示
** 函数说明 ：无
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************/ 
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

/*********************************************************************************************************
** 函数功能 ：关闭OLED显示     
** 函数说明 ：无
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************/  
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		

/*********************************************************************************************************
** 函数功能 ：清屏函数
** 函数说明 ：清完屏,整个屏幕是黑色的!和没点亮一样!!!	
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************/ 
void OLED_Clear(void)  
{  
	u8 i,j;  
	for(i=0;i<8;i++)
		for(j=0;j<128;j++)
			OLED_GRAM[j][i]=0X00;  
	OLED_Refresh_Gram();					//更新显示
}

/*********************************************************************************************************
** 函数功能 ：画点 
** 函数说明 ：在OLED屏上点亮或清空一个点
** 入口参数 ：x:0~127
**						y:0~63
**						mode:1：填充 ；0：清空	
** 出口参数 ：无
*********************************************************************************************************/ 				   
void OLED_DrawPoint(u8 x,u8 y,u8 mode)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)
		return;											//超出范围了
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(mode)
		OLED_GRAM[x][pos]|=temp;
	else 
		OLED_GRAM[x][pos]&=~temp;	    
}

/*********************************************************************************************************
** 函数功能 ：在OLED屏上点亮或清空一片区域
** 函数说明 ：区域为矩形
** 入口参数 ：x1,y1,x2,y2 填充区域的对角坐标（确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63）
**						mode:0,清空;1,填充	  
** 出口参数 ：无
*********************************************************************************************************/ 	 
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)
			OLED_DrawPoint(x,y,mode);
	}													    
	OLED_Refresh_Gram();					//更新显示
}

/*********************************************************************************************************
** 函数功能 ：在指定位置显示一个字符,包括部分字符
** 函数说明 ：字符为ASCII字符
** 入口参数 ：x:0~127
**						y:0~63
**						mode:0,反白显示;1,正常显示		
**						size:选择字体 16/12 
** 出口参数 ：无
*********************************************************************************************************/ 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';																		//得到偏移后的值				   
	for(t=0;t<size;t++)
	{   
		if(size==12)
			temp=OLED_ASCII_1206[chr][t];  							//调用1206字体
		else 
			temp=OLED_ASCII_1608[chr][t];		 						//调用1608字体 	                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
				OLED_DrawPoint(x,y,mode);
			else 
				OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}          
}

/*********************************************************************************************************
** 函数功能 ：在指定位置显示一个特殊字符,包括部分字符
** 函数说明 ：通过调用void OLED_WR_Byte(uchar dat,uchar cmd)函数在指定的位置显示一个特殊字符
** 入口参数 ：x:0~127
**						y:0~63
**						num	:特殊字符点阵数组中字符的位置
**						mode:0,反白显示;1,正常显示
** 出口参数 ：无
*********************************************************************************************************/ 
void OLED_Special_Char(u8 x,u8 y,u8 num,u8 mode)
{      			    
	u8 temp,i,j;
	u8 y0=y;
	for(i=0;i<16;i++)
	{
		temp=OLED_SPE_CHA_1608[num][i];
		for(j=0;j<8;j++)
		{
			if(temp&0x80)
				OLED_DrawPoint(x,y,mode);
			else 
				OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==16)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}         
}

/*********************************************************************************************************
** 函数功能 ：幂函数
** 函数说明 ：m^n
** 入口参数 ：m,n
** 出口参数 ：无
*********************************************************************************************************/ 
u32 Power_Function(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)
		result*=m;    
	return result;
}				  

/*********************************************************************************************************
** 函数功能 ：显示2个数字
** 函数说明 ：无
** 入口参数 ：x,y :起点坐标	 
**						num:数值(0~4294967295)	
**						len :数字的位数		
**						size:字体大小
** 出口参数 ：无
*********************************************************************************************************/
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/Power_Function(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,'0',size,1);
				continue;
			}
			else 
				enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 

/*********************************************************************************************************
** 函数功能 ：显示字符串
** 函数说明 ：用16字体
** 入口参数 ：x,y :起点坐标	 
**						*p:字符串起始地址
** 出口参数 ：无
*********************************************************************************************************/
void OLED_ShowString(u8 x,u8 y,const u8 *p)
{         
	while(*p!='\0')
	{
		if(x>Max_Column-1-8)
		{
			x=0;
			y+=16;
		}
		if(y>Max_Row-1-8)
		{
			y=x=0;
			OLED_Clear();
		}
		OLED_ShowChar(x,y,*p,16,1);
		x+=8;
		p++;
	}
}

/*********************************************************************************************************
** 函数功能 ：在指定位置显示显示一个汉字
** 函数说明 ：通过调用void OLED_DrawPoint(u8 x,u8 y,u8 mode)函数在指定的位置显示一个汉字
** 入口参数 ：x		:0~127
**						y		:0~63
**						num	:汉字点阵数组中的汉字位置
**						mode:0,反白显示;1,正常显示
** 出口参数 ：无
*********************************************************************************************************/ 
void OLED_Show_Chinese(u8 x,u8 y,u8 num,u8 mode)
{         
	u8 temp,i,j;
	u8 y0=y;
	for(i=0;i<32;i++)
	{   
		temp=OLED_CHN_3216[num][i];
		for(j=0;j<8;j++)
		{
			if(temp&0x80)
				OLED_DrawPoint(x,y,mode);
			else 
				OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==16)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}
}
