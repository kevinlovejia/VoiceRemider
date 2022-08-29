/*
   Project: N|Watch
   Author: Zak Kemble, contact@zakkemble.co.uk
   Copyright: (C) 2013 by Zak Kemble
   License: GNU GPL v3 (see License.txt)
   Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
*/
#include "buttons.h"
#include "common.h"
#include "oled_driver.h"
#include "gpio.h"
#include "usart.h"
//#include "sys.h"
//#include "led.h"


#define BTN_IS_PRESSED	0
#define BTN_NOT_PRESSED	0

bool DeepSleepFlag = 0;


typedef struct {
  millis_t pressedTime;	// Time of press
  bool processed;			// Time of press has been stored (don't store again until next press)
  byte counter;			// Debounce counter
  bool funcDone;			// Function has been ran (don't run again until next press)
  button_f onPress;		// Function to run when pressed
  const ulong* tune;		// Tune to play when pressed
} s_button;

static s_button buttons[BTN_COUNT];

static void processButtons(void);
static void processButton(s_button*, BOOL);
static byte bitCount(byte);

//EMPTY_INTERRUPT(PCINT0_vect);

void buttons_init()
{
  buttons_startup();

	//�������Ĳ�������
  // Assign tunes
  buttons[BTN_1].tune = tuneBtn1;
  buttons[BTN_2].tune = tuneBtn2;
  buttons[BTN_3].tune = tuneBtn3;

  // Set up interrupts
//#ifdef __AVR_ATmega32U4__
//  SET_BITS(PCMSK0, PCINT4, PCINT6, PCINT7);
//#else
//  SET_BITS(PCMSK1, PCINT9, PCINT10, PCINT11);
//#endif
//  BTN_INT_ON();
}

void buttons_update()
{
  static millis8_t lastUpdate;

  // Update every 10ms
  millis8_t now = millis();
  if ((millis8_t)(now - lastUpdate) >= 1)
  {
	 // printf("1");
    lastUpdate = now;
    processButtons();
  }
}

//������ʼ������
// Sets button pins to INPUT with PULLUP
void buttons_startup()
{
	
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);	//��GPIO��ʱ�ӣ��ȴ򿪸��ò����޸ĸ��ù���
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//Ҫ�ȿ�ʱ�ӣ�����ӳ�䣻����ʾ�ر�jtag��ʹ��swd��  

//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	         //SW1, SW3, SW2
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_5|GPIO_Pin_10; //PB14 PB5  PB10	 
// 	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU; 		 //�ڲ���������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//�ٶ�50MHz
// 	GPIO_Init(GPIOB, &GPIO_InitStructure);	
}

// Sets button pins to OUTPUT LOW
// This stops them wasting current through the pull-up resistor when pressed
void buttons_shutdown()
{
	//�������sleep����������رհ�����������
	
//  pinPullup(BTN_1_P,	PU_DIS);
//  pinPullup(BTN_2_P,	PU_DIS);
//  pinPullup(BTN_3_P,	PU_DIS);

//  pinMode(BTN_1_P,	OUTPUT);
//  pinMode(BTN_2_P,	OUTPUT);
//  pinMode(BTN_3_P,	OUTPUT);
}
volatile uint8_t dir_flag = 2; /* �����־ 0: ˳ʱ?? 1: ��ʱ?? 2: δ��״???*/
uint16_t key_count = 0;
int32_t test_num = 0;
volatile uint8_t key_click_flag = 0, key_lock = 0;
volatile uint8_t beep_flag = 0;

uint8_t play[] = {0xAA, 0x02, 0x00, 0xAC};
uint8_t next[] = {0xAA, 0x06, 0x00, 0xB0}; 
uint8_t max_vol[] = {0xAA, 0x13, 0x01, 0x1E, 0xDC};
uint8_t play_no1[] = {0xAA, 0x07, 0x02, 0x00, 0x0E, 0xC1,  };
uint8_t play_no15[] ={0xAA, 0x07, 0x02, 0x00, 0x0F, 0xC2, };

extern int keyv;
int timesss = 10;

static void processButtons()
{
  // Get button pressed states
  BOOL isPressed[BTN_COUNT];	

	
	/* �������� */
	GPIO_PinState key_value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);//
	//key_count > 15 && &&DeepSleepFlag==1
	if(key_value == 0)
	{
		static millis8_t lastPress; // Time of last draw
		// Limit frame rate
		millis8_t nowPress = millis();
		if((millis8_t)(nowPress - lastPress) > timesss)
		{
			key_value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);
			if(key_value == 0)
			{
				isPressed[BTN_2] = 0;
			}
			else
				isPressed[BTN_2] = 1;
			lastPress = nowPress;
		}
		else
			isPressed[BTN_2] = 1;
	}
	else
		isPressed[BTN_2] = 1;
	

//	
//	if(key_count > timesss)
//	{
//	
//	
//	
//	}
//	if(key_click_flag == 0)	/*���û�м����£��ߵ�ƽ������һЩ��־λ��ʱ����*/
//	{
//		key_lock = 0;	/*������־λ��0*/
//		//key_count = 0;	/*����ȥ������ʱ����������*/
//		isPressed[BTN_2] = 1;
//	}
//	else //if(key_lock == 0)	/*����а������£����ǵ�һ�ΰ���*/
//	{
//		
//		{
//			key_count = 0;
//			//key_lock = 1;/*������־λ��λ������һֱ����*/
//			isPressed[BTN_2] = 0;
//		}
//	}
	
	
	
//	if(key_click_flag == 1)
//	{
//		if(key_count > 100)
//		//if(key_value == GPIO_PIN_RESET )//wei change 2022.5.31 RESET ��ΪGPIO_PIN_RESET
//		{
//	//		DeepSleepFlag=0;
//	//		OLED_ON();
//	//		animation_start(display_load, ANIM_MOVE_OFF);
//			test_num = 0;
//			key_count = 0; key_click_flag = 0;
//			//HAL_UART_Transmit(&huart2, (uint8_t*)play_no15, sizeof(play_no15), 0xff);
//			//pcf8563_gettime();
//			isPressed[BTN_2] = 0;
//		}
//	}
//	else
//		isPressed[BTN_2] = 1;
	
	if(beep_flag == 1)
	{
		beep_flag = 0;
		if(dir_flag == 1)//��ʱ��		
		{			
			isPressed[BTN_1] = 1;
			isPressed[BTN_3] = 0;  //left
			//HAL_UART_Transmit(&huart2, (uint8_t*)play_no1, sizeof(play_no1), 0xff);
		}
		else if(dir_flag == 0)//˳ʱ��
		{
		//HAL_UART_Transmit(&huart1, (uint8_t*)next, sizeof(next), 0xff);		
			isPressed[BTN_1] = 0;  //rightһ�����¼���	
			isPressed[BTN_3] = 1;
		}
	}
	else
	{
		isPressed[BTN_3] = 1;  //left
		isPressed[BTN_1] = 1;  //rightһ�����¼���
		if(dir_flag != 2) {
			dir_flag = 2;
			//printf("num: %d\n", test_num);
		}
	}
	
  // Process each button
	
  LOOPR(BTN_COUNT, i)
  processButton(&buttons[i], !isPressed[i]);
}

static void processButton(s_button* button, BOOL isPressed)
{
  button->counter <<= 1;
  if (isPressed)
  {
    // Set debounce counter bit
    button->counter |= 1;

    // Are enough bits set to count as pressed?//�൱�ڰ�������
    //if (bitCount(button->counter) >= BTN_IS_PRESSED)
    {
      // Store time of press
      if (!button->processed)
      {
        button->pressedTime = millis();
        button->processed = true;
      }

      // Run function
      if (!button->funcDone && button->onPress != NULL && button->onPress())
      {
        button->funcDone = true;
        //tune_play(button->tune, VOL_UI, PRIO_UI);

          //led_flash(LED_GREEN, LED_FLASH_FAST, LED_BRIGHTNESS_MAX);
					//LED1=!LED1;	 
        
      }
    }
  }
  else // Not pressed
  {
    // Has button been not pressed for long enough?
    //if (bitCount(button->counter) <= BTN_NOT_PRESSED)
    {
      button->processed = false;
      button->funcDone = false;
    }
  }
}

// Count set bits in value
static byte bitCount(byte val)
{
  byte count = 0;
  for (; val; val >>= 1)
    count += val & 1;
  return count;
}

//����һ�����غ���ָ��ĺ���
// Set new function to run when button is pressed and return the old function
button_f buttons_setFunc(btn_t btn, button_f func)
{
  button_f old = buttons[btn].onPress;
  buttons[btn].onPress = func;
  return old;
}

// Set functions to run for each button
void buttons_setFuncs(button_f btn1, button_f btn2, button_f btn3)
{
  buttons[BTN_1].onPress = btn1;
  buttons[BTN_2].onPress = btn2;
  buttons[BTN_3].onPress = btn3;
}

/*
  // Get how long a button has been pressed for
  millis_t buttons_pressTime(btn_t btn) // set max press time to 1 min!!!
  {
	s_button* button = &buttons[btn];
	if(button->pressed == BTN_NOT_PRESSED)
		return 0;
	return (millis() - button->pressedTime);
  }
*/

// See if a button has been pressed in the past x milliseconds
bool buttons_isActive()
{
//  // If sleep has been disabled then just say that the buttons are always active
  if (!appConfig.sleepTimeout)
    return true;

//  // Get timeout val in ms
  uint timeout = (appConfig.sleepTimeout * 5) * 1000;
//  uint timeout =  1000;

  // See if a button has been pressed within that timeout
  LOOPR(BTN_COUNT, i)
  {
    if (millis() - buttons[i].pressedTime < timeout)
      return true;
  }

  return false;
}

// Set button status to pressed, processed etc but don't run their functions
void buttons_wake()
{
	LOOPR(BTN_COUNT, i)
  {
    buttons[i].funcDone		= true;
    buttons[i].processed	= true;
    buttons[i].counter		= BTN_IS_PRESSED;
    buttons[i].pressedTime	= millis();
  }
}
