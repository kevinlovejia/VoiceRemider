/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "oled_i2c.h"
#include "pcf8563.h"
#include <stdio.h>
#include <string.h>
#include "common.h"
//#include "time.h"
//#include "util.h"
#include "oled_driver.h"
//#include "gui_log_console.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void c_loop(void);
void test_loop(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


extern unsigned char g8563_Store[6];
unsigned char now_sec;
char time_str[25];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	//int i = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	driverInfo_P = (driverInfo_s *)malloc(sizeof(driverInfo_s));
  if(driverInfo_P == NULL)
    while(1);//Todo: 
	flashCont = (driverInfo_s *)malloc(sizeof(driverInfo_s));
	if(flashCont == NULL)
		while(1);//Todo:
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	OLED_Init();
	//pcf8563_init();
	pcf8563_gettime();
	
	
	console_log(500,"init_OLED: OK");
	console_log(10,"--- welcome !---");
	console_log(10,"  _   _    _    ");
	console_log(10," | | | |  |_|   ");
	console_log(10," | |_| |   _    ");
	console_log(10," |  _  |  | |   ");
	console_log(10," | | | |  | |   ");
	console_log(10," |_| |_|  |_|   ");
	console_log(10,"                ");
	milliseconds=0;
	
	memset(&oledBuffer, 0x00, FRAME_BUFFER_SIZE);
	appconfig_init();
	alarm_init();              //�޷��������ӣ�ÿ�������Ժ���Ҫ�Զ���
	Set_PCF8563_date();
	pwrmgr_init();
	console_log(500,"START !");
	display_set(watchface_normal);
	display_load();//��������	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		c_loop(); //ѭ��test_loop();
		//test_loop();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		pcf8563_gettime();
//		if(now_sec != g8563_Store[0])
//		{
//			sprintf(time_str,"20%d/%02d/%02d",g8563_Store[5],g8563_Store[4],g8563_Store[3]);			
//			//OLED_ShowStr(0,3  ,(unsigned char*)time_str,2);//����8*16�ַ�			
//			sprintf(time_str,"%02d:%02d:%02d", g8563_Store[2],g8563_Store[1],g8563_Store[0]);			
//			//OLED_ShowStr(0,5  ,(unsigned char*)time_str,2);//����8*16�ַ�
//			now_sec = g8563_Store[0];
//		}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void c_loop()
{
	time_update();
	if(pwrmgr_userActive())
	{
		//battery_update();
		buttons_update();
	}
  
//////	mpu_updata(); 

//////	buzzer_update();
//////	led_update();
	
	
//#if COMPILE_STOPWATCH
//	stopwatch_update();
//#endif
//////  global_update();
	
	if(pwrmgr_userActive())
	{
////		alarm_update();
		display_update();
	}
//////	if(bme_enable)
//////		bme_update();
//////	pwrmgr_update();
//��ʾ��ɺ����������
	memset(&oledBuffer, 0x00, FRAME_BUFFER_SIZE);
}

void test_loop()
{

  
	

	display_test();
	
	memset(&oledBuffer, 0x00, FRAME_BUFFER_SIZE);
}


extern volatile uint8_t dir_flag, beep_flag, key_click_flag;
extern int32_t test_num;
int32_t test_ts = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
  static uint8_t count = 0;
  static uint8_t b_flag;
  GPIO_PinState a_value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);//
  GPIO_PinState b_value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);
	//GPIO_PinState c_value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);
  
  if(GPIO_Pin == GPIO_PIN_6){
    if(a_value == RESET && count == 0){
			b_flag = 0;
			if(b_value) b_flag = 1;
				count = 1;
		}
	
		if(a_value == SET && count == 1){
			if(b_value == RESET && b_flag ==1){
				test_num--;		//��ʱ��
				dir_flag = 1;
				beep_flag = 1;
			}
			if(b_value && b_flag == 0){
				test_num++;		//˳ʱ��
				dir_flag = 0;
				beep_flag = 1;
			}
			count = 0;
		}    
  }
  
//  /* �����ж� */
//	static millis8_t lastPress; // Time of last draw


//	// Limit frame rate
//	millis8_t nowPress = millis();
//	if((millis8_t)(now - lastPress) < timesss)
//	{
//		//pwrmgr_setState(PWR_ACTIVE_DISPLAY, PWR_STATE_IDLE);
//		return;
//	}
//	lastPress = now;
//	
//	
//  if(GPIO_Pin == GPIO_PIN_7){		
//			key_click_flag = 1;
//  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
