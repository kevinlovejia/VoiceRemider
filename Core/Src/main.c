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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled_i2c.h"
#include "pcf8563.h"
#include <stdio.h>
#include "lvgl.h"
#include "porting/lv_port_disp.h"
#include "porting/lv_port_indev.h"
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
int32_t test_num = 0;
uint8_t dir_flag = 2; /* ·½Ïò±êÖ¾ 0: Ë³Ê±?? 1: ÄæÊ±?? 2: Î´¶¯×´???*/
uint8_t key_click_flag = 0;
uint8_t key_count = 0;
extern int keyv;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t play[] = {0xAA, 0x02, 0x00, 0xAC};
uint8_t next[] = {0xAA, 0x06, 0x00, 0xB0}; 
uint8_t max_vol[] = {0xAA, 0x13, 0x01, 0x1E, 0xDC};
uint8_t play_no1[] = {0xAA, 0x07, 0x02, 0x00, 0x0E, 0xC1,  };
uint8_t play_no15[] ={0xAA, 0x07, 0x02, 0x00, 0x0F, 0xC2, };
uint8_t beep_flag = 0;
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
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	//pcf8563_init();
	pcf8563_gettime();
	now_sec = g8563_Store[0];

	
	int i = 0;
	OLED_Fill(0xFF);//È«ÆÁµãÁÁ
	HAL_Delay(2000);
	OLED_Fill(0x00);//È«ÆÁÃð
	HAL_Delay(2000);	
	
	HAL_UART_Transmit(&huart2, (uint8_t*)max_vol, sizeof(max_vol), 0xff);
	HAL_Delay(200);
	HAL_UART_Transmit(&huart2, (uint8_t*)max_vol, sizeof(max_vol), 0xff);
	//HAL_Delay(2000);
	
//	for(i=0;i<5;i++)
//	{
//		OLED_ShowCN(22+i*16,0,i);//²âÊÔÏÔÊ¾ÖÐÎÄ
//	}
	//OLED_ShowStr(0,3,(unsigned char*)"Say yes is no",1);//²âÊÔ6*8×Ö·û
	//OLED_ShowStr(0,4,(unsigned char*)"Say no is yes",2);//²âÊÔ8*16×Ö·û

	lv_init();
	
	lv_port_disp_init();
	//lv_port_indev_init();
	
	lv_obj_t * label1 = lv_label_create(lv_scr_act());
	lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
	lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
	lv_label_set_text(label1, "#0000ff Re-color# #ff00ff words# #ff0000 of a# label, align the lines to the center "
										"and wrap long text automatically.");
	lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
	lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(dir_flag != 2) {
			dir_flag = 2;
			//printf("num: %d\n", test_num);
	  }
	  
	  /* °´¼üÏû¶¶ */
	  GPIO_PinState key_value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);//
	  if(key_count > 15 && key_value == GPIO_PIN_RESET)//wei change 2022.5.31 RESET ¸ÄÎªGPIO_PIN_RESET
	  {
		  test_num = 0;
		  key_count = 0; key_click_flag = 0;
			HAL_UART_Transmit(&huart2, (uint8_t*)play_no15, sizeof(play_no15), 0xff);
			pcf8563_gettime();
	  }
		if(beep_flag == 1)
		{
			beep_flag = 0;
			//if(dir_flag == 1)
				HAL_UART_Transmit(&huart2, (uint8_t*)play_no1, sizeof(play_no1), 0xff);
			//else if(dir_flag == 0)
				//HAL_UART_Transmit(&huart1, (uint8_t*)next, sizeof(next), 0xff);
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		pcf8563_gettime();
		if(now_sec != g8563_Store[0])
		{
			sprintf(time_str,"20%d/%02d/%02d",g8563_Store[5],g8563_Store[4],g8563_Store[3]);			
			//OLED_ShowStr(0,3  ,(unsigned char*)time_str,2);//²âÊÔ8*16×Ö·û			
			sprintf(time_str,"%02d:%02d:%02d", g8563_Store[2],g8563_Store[1],g8563_Store[0]);			
			//OLED_ShowStr(0,5  ,(unsigned char*)time_str,2);//²âÊÔ8*16×Ö·û
			now_sec = g8563_Store[0];
		}
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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
  static uint8_t count = 0;
  static uint8_t b_flag;
  GPIO_PinState a_value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);//
  GPIO_PinState b_value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);
  
  if(GPIO_Pin == GPIO_PIN_6){
    if(a_value == RESET && count == 0){
			b_flag = 0;
			if(b_value) b_flag = 1;
				count = 1;
		}
	
		if(a_value == SET && count == 1){
			if(b_value == RESET && b_flag ==1){
				test_num--;		//ÄæÊ±Õë
				dir_flag = 1;
				beep_flag = 1;
			}
			if(b_value && b_flag == 0){
				test_num++;		//Ë³Ê±Õë
				dir_flag = 0;
				beep_flag = 1;
			}
			count = 0;
		}    
  }
  
  /* °´¼üÖÐ¶Ï */
  if(GPIO_Pin == GPIO_PIN_7){
		key_click_flag = 1;
  }
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
