/*
 * Project: Lightweight millisecond tracking library
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/millisecond-tracking-library-for-avr/
 */

#ifndef ARDUINO


#include "common.h"
//#include "sys.h"
//#include "led.h"
//#include "ADC.h"
//#include "bme280.h"
//#include "misc.h"

extern 	float BatteryVol;//�ĳ�u16�ᵼ���޷�������ʾ
 millis_t milliseconds;


//static inline bool millis_interrupt_off(void)
//{
//	TIM_Cmd(TIM2,DISABLE);//����ʱ��
//	return true;
//}

//static inline bool millis_interrupt_on(void)
//{
//	TIM_Cmd(TIM2,ENABLE);//����ʱ��
//	return false;
//}

////// NEED TO DO A RESTORE THING! if paused then millis() is called it will unpause
//#define MILLIS_ATOMIC() for(bool cs = millis_interrupt_off(); cs; cs = millis_interrupt_on())


// Initialise library
void millis_init()
{
	// Timer settings
	  //1������Ƕ���жϿ�����NVIC
     //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);//���ȼ�����
     NVIC_InitTypeDef nvic;
     nvic.NVIC_IRQChannel=TIM3_IRQn;
     nvic.NVIC_IRQChannelCmd=ENABLE;
     nvic.NVIC_IRQChannelPreemptionPriority=0;
     nvic.NVIC_IRQChannelSubPriority=0;
     NVIC_Init(&nvic);

	 //2����ʱ����ʼ������
     TIM_TimeBaseInitTypeDef tim;//�ṹ��
     //���ȼ���������
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//����ʱ��
     TIM_DeInit(TIM3);//����������
     tim.TIM_ClockDivision=TIM_CKD_DIV1;//������Ƶ
     tim.TIM_CounterMode=TIM_CounterMode_Up;//���ϼ���
     tim.TIM_Period=50;//�Զ���װ�ؼĴ�����ֵ
     tim.TIM_Prescaler=1439;//ʱ��Ԥ��Ƶ
     //tim.TIM_RepetitionCounter=
     TIM_TimeBaseInit(TIM3,&tim);//��ʼ���ṹ��
     TIM_ClearFlag(TIM3,TIM_FLAG_Update);//�������жϱ�־
     TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
     TIM_Cmd(TIM3,ENABLE);//����ʱ��
}

// Get current milliseconds
millis_t millis_get()
{
	millis_t ms;
		ms = milliseconds;
	return ms;
}

extern char imu_run;
u8 HistoryCount=0;
extern HistoryData historydat[12];
extern float DS3231_Temp;
extern int altitude;
bool bme_flag=0;
u8 bme_time = 5;
u8 log_time = 1;
extern bool bme_enable;
 //��ʱ���жϺ������� //TIM2ͨ�ö�ʱ��
 void TIM3_IRQHandler(void)
{
	static u8 count=0;	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)//�ж��жϱ�־�Ƿ���
	{
		//wifi_wait_data_hander();
			milliseconds++;		
			//update = true;
		
		#ifdef RTC_SRC
		  update = true;
		#else
		
		
		
		if(milliseconds%200==0)
		{
			update = true;
			//++timeDate.time.secs;   //ÿ0.2���Ӹ���һ��ʱ��;
		}
		
		if(milliseconds%(bme_time*1000)==0)
		{
			bme_flag=1;
		}
		if(milliseconds%10000==0)
		{
			BatteryVol=Get_Adc_Average(0,50)*(3.3/4096)*2;//��ѹ����
			if(bme_enable)
				count++;
		}
		if(count>6*log_time)          //ÿ10���Ӹ���һ������
		{
			count=0;
			historydat[HistoryCount].hour=timeDate.time.hour;
			historydat[HistoryCount].min=timeDate.time.mins;
			historydat[HistoryCount].temp=DS3231_Temp;
			historydat[HistoryCount].shidu=bme280_data.H;
			historydat[HistoryCount].height=altitude;
			if(++HistoryCount>11)
				HistoryCount=0;
		}
		
//		if(milliseconds%5==0)
//		{
//			if(imu_run)
//			{
//				mpu_dmp_get_data(&pitch,&roll,&yaw);	
//			}	
//		}
		#endif
		
   }
 TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);     //�ֶ�����жϱ�־λ
}

#endif
