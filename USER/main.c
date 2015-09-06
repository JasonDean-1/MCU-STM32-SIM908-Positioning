#include "main.h"

int main(void)
{	 
	u8 t,key,shut_flag=0;			//key:按键值；shut_flag；关闭屏幕标志 0:关闭显示，非0，显示

	System_Init(); 						//系统初始化

	while(1)
	{
		key=KEY_Scan(0);							//如果有键按下（支持连按）
		while(key==KEY_RIGHT)
		{
			key=0;
			delay_ms(50);
			t++;												//用于按键按下计时
			key=KEY_Scan(1);
			if(t>40)										//如果按键按下超过2S
			{
				shut_flag=0;
				break;
			}
			shut_flag++;
			if(shut_flag==4)
				shut_flag=1;
			OLED_Clear();								//关闭屏幕，清屏
		}
		t=0;													//清0计时器

		if(shut_flag==1)
		{
			Display_Static_Time();			//显示时间固定部分
			Update_Time();							//更新时间
			Update_Temperture();				//更新气温
		}
		else if(shut_flag==2)
		{
			Display_Static_Heart_Rate();	//显示心率固定部分
			Update_Heart_Rate();					//更新心率
		}
		else if(shut_flag==3)
		{
			Display_Static_Pedomter();
			Update_Pedometer();
		}
		else	if(shut_flag==0)
			OLED_Clear();								//关闭屏幕，清屏

		OLED_Refresh_Gram();					//更新显示
	}
}

