/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_
#include "typedefs.h"
#include "main.h"



/*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/
//#define KEY_UP      GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)   //back
#define KEY0        HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)//GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)  //right
#define KEY1        HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)//GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7) //ok
#define KEY2        HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5)//GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//left
/*���淽ʽ��ͨ��λ��������ʽ��ȡIO*/
/*
#define KEY0 		PEin(4)   	//PE4
#define KEY1 		PEin(3)		//PE3 
#define KEY2 		PEin(2)		//P32
#define WK_UP 	PAin(0)		//PA0
*/


typedef enum
{
	BTN_1 = 0,
	BTN_2 = 1,
	BTN_3 = 2,
	BTN_COUNT = 3 // must be last
} btn_t;

void buttons_init(void);
void buttons_update(void);
void buttons_startup(void);
void buttons_shutdown(void);
button_f buttons_setFunc(btn_t, button_f);
void buttons_setFuncs(button_f, button_f, button_f);
//millis_t buttons_pressTime(btn_t);
bool buttons_isActive(void);
void buttons_wake(void);
extern uint16_t key_count;




#endif /* BUTTONS_H_ */
