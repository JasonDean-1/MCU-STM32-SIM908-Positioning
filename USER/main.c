#include "main.h"

int main(void)
{	 
	u8 t,key,shut_flag=0;			//key:����ֵ��shut_flag���ر���Ļ��־ 0:�ر���ʾ����0����ʾ

	System_Init(); 						//ϵͳ��ʼ��

	while(1)
	{
		key=KEY_Scan(0);							//����м����£�֧��������
		while(key==KEY_RIGHT)
		{
			key=0;
			delay_ms(50);
			t++;												//���ڰ������¼�ʱ
			key=KEY_Scan(1);
			if(t>40)										//����������³���2S
			{
				shut_flag=0;
				break;
			}
			shut_flag++;
			if(shut_flag==4)
				shut_flag=1;
			OLED_Clear();								//�ر���Ļ������
		}
		t=0;													//��0��ʱ��

		if(shut_flag==1)
		{
			Display_Static_Time();			//��ʾʱ��̶�����
			Update_Time();							//����ʱ��
			Update_Temperture();				//��������
		}
		else if(shut_flag==2)
		{
			Display_Static_Heart_Rate();	//��ʾ���ʹ̶�����
			Update_Heart_Rate();					//��������
		}
		else if(shut_flag==3)
		{
			Display_Static_Pedomter();
			Update_Pedometer();
		}
		else	if(shut_flag==0)
			OLED_Clear();								//�ر���Ļ������

		OLED_Refresh_Gram();					//������ʾ
	}
}

