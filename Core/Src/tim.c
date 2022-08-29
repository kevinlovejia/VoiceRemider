/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */
#ifndef ARDUINO

#include "common.h"
extern 	float BatteryVol;//改成u16会导致无法正常显示
millis_t milliseconds;
/* USER CODE END 0 */

TIM_HandleTypeDef htim3;

/* TIM3 init function */
void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1439;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 49;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */
	if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)		//启动定时器3
  {
    /* Starting Error */
    Error_Handler();
  }
  /* USER CODE END TIM3_Init 2 */

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

    /* TIM3 interrupt Init */
    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();

    /* TIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
//  BSP_LED_Toggle(LED2);
	static u8 count=0;	
		//wifi_wait_data_hander();
	milliseconds++;		
	//update = true;
	
	#ifdef RTC_SRC
		update = true;
	#else
	
	
	
	if(milliseconds%200==0)
	{
		update = true;
		//++timeDate.time.secs;   //每0.2秒钟更新一次时间;
	}
	
	if(milliseconds%(bme_time*1000)==0)
	{
		bme_flag=1;
	}
//	if(milliseconds%10000==0)
//	{
//		BatteryVol=Get_Adc_Average(0,50)*(3.3/4096)*2;//电压计算
//		if(bme_enable)
//			count++;
//	}
	if(count>6*log_time)          //每10分钟更新一次数据
	{
		count=0;
		historydat[HistoryCount].hour=timeDate.time.hour;
		historydat[HistoryCount].min=timeDate.time.mins;
		//historydat[HistoryCount].temp=DS3231_Temp;
		//historydat[HistoryCount].shidu=bme280_data.H;
		//historydat[HistoryCount].height=altitude;
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



//定时器中断函数处理。 //TIM3通用定时器
//void TIM3_IRQHandler(void)
//{
//	static u8 count=0;	
//	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)//判断中断标志是否发生
//	{
//		//wifi_wait_data_hander();
//			milliseconds++;		
//			//update = true;
//		
//		#ifdef RTC_SRC
//		  update = true;
//		#else
//		
//		
//		
//		if(milliseconds%200==0)
//		{
//			update = true;
//			//++timeDate.time.secs;   //每0.2秒钟更新一次时间;
//		}
//		
//		if(milliseconds%(bme_time*1000)==0)
//		{
//			bme_flag=1;
//		}
//		if(milliseconds%10000==0)
//		{
//			BatteryVol=Get_Adc_Average(0,50)*(3.3/4096)*2;//电压计算
//			if(bme_enable)
//				count++;
//		}
//		if(count>6*log_time)          //每10分钟更新一次数据
//		{
//			count=0;
//			historydat[HistoryCount].hour=timeDate.time.hour;
//			historydat[HistoryCount].min=timeDate.time.mins;
//			historydat[HistoryCount].temp=DS3231_Temp;
//			historydat[HistoryCount].shidu=bme280_data.H;
//			historydat[HistoryCount].height=altitude;
//			if(++HistoryCount>11)
//				HistoryCount=0;
//		}
//		
////		if(milliseconds%5==0)
////		{
////			if(imu_run)
////			{
////				mpu_dmp_get_data(&pitch,&roll,&yaw);	
////			}	
////		}
//		#endif
//		
//   }
// TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);     //手动清除中断标志位
//}

#endif
/* USER CODE END 1 */
