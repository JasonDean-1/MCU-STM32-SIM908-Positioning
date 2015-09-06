/*********************************************************************************************************
** �ļ����� ��STM32�����¶ȴ�������������
** �������� ��FengZhaoHong
** ����ʱ��	��20140830
*********************************************************************************************************/
#include "tsensor.h" 

/*********************************************************************************************************
** �������� ��ͨ��ADCͨ����ȡ��ADCת������������ǰ���¶�ֵ
** ����˵�� �������¶ȴ�������STM32���ڲ�����STM32����ʱ��оƬ���¶Ȼ����ߣ����Եó����¶�ֵ����ʵ�ʵĻ����¶�
** ��ڲ��� ��adc_val:ADCת�������ֵ
** ���ڲ��� ����ǰ�Ļ����¶�ֵ
*********************************************************************************************************/
float T_Get_Temperture(u16 adc_val)
{
	float temp_vol;															//�洢ADCת����õ����¶ȶ�Ӧ�ĵ�ѹֵ
	float temperature;													//�洢����õ����¶�ֵ
	temp_vol=(float)adc_val*(3.3/4096);					//��ADCͨ����ȡ��12λ��������ֵת��Ϊ��Ӧ�ĵ�ѹֵ
	temperature=(1.43-temp_vol)/0.0043+25;			//�������ǰ�¶�ֵ
	return temperature;
}

/*********************************************************************************************************
** �������� ����ȡУ׼�¶�ֵ
** ����˵�� ��ʵ�ʻ����¶�ֵ
** ��ڲ��� ��ncal_temp��ΪУ׼���¶�ֵ
** ���ڲ��� ��cal_temp��У׼����¶�ֵ
*********************************************************************************************************/
float T_Get_Calibrated_Temperature(float ncal_temp)
{
	float cal_temp;
	cal_temp=ncal_temp-6.0;											//��ȥ����оƬ�������ȵ��µ������¶�ֵ
	return cal_temp;
}