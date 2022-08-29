#include "common.h"
//#include "delay.h"
#include "oled_driver.h"
//#include "NVIC.h"
//#include "bme280.h"
//#include "mpu_task.h"
#include "altitude_display.h"

#define STR_WIFICMDMENU  "< My Menu >"
#define CMD1_NAME "Shut Down"
#define CMD2_NAME "Deep Sleep"
#define CMD3_NAME "MPU_Display"
#define CMD4_NAME "History_Data"
#define CMD5_NAME "Back"

#define OPTION_COUNT 5


static void mSelect()
{
	doAction(false);  //ִ��ָ��
	//menuData.isOpen = false;  //�رղ˵�
}


void ShutDown(void)
{
	display_startCRTAnim(CRTANIM_CLOSE);
	//GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}

extern bool DeepSleepFlag;
void cmd2(void)
{
	DeepSleepFlag = 1;
	OLED_OFF();
	menuData.isOpen = false;  //�رղ˵�
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI);//����ͣ��ģʽ
}

void cmd3(void)
{
	animation_start(display_load, ANIM_MOVE_OFF);
	menuData.isOpen = false;  //�رղ˵�
}
extern u8 log_time;

static void LogTimeUpdate()
{
//	battery_updateNow();
	log_time+=2;
	if(log_time>15)
		log_time=1;
}

static void itemLoader(byte num)
{
	char buff[20];
	num = 0;

	setMenuOption_P(num++, PSTR(CMD1_NAME), NULL, ShutDown);

	setMenuOption_P(num++, PSTR(CMD2_NAME), NULL, cmd2);
	
	//setMenuOption_P(num++, PSTR(CMD3_NAME), NULL, mpu_open);
	
	setMenuOption_P(num++, PSTR(CMD4_NAME), NULL, history_display);
	
	sprintf_P((char *)buff, PSTR("Log Time  %d min"), log_time);
	setMenuOption_P(num++, buff, NULL, LogTimeUpdate);

	setMenuOption_P(num++, PSTR(CMD5_NAME), NULL, cmd3);
}

bool my_menu_open(void)
{
	menuData.isOpen = true;  //�򿪲˵�
	
	display_setDrawFunc(menu_draw);  //�󶨻��ƺ���Ϊmenu_draw

	buttons_setFuncs(menu_up, menu_select, menu_down);  //wei 2022.8.3�󶨰������ܺ���

	setMenuInfo(OPTION_COUNT, MENU_TYPE_STR, PSTR(STR_WIFICMDMENU));   //��ȡ��ǰ�˵���Ϣ��ѡ��������˵����������ֻ���ͼ�꣩
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);  //�󶨲˵��ĺ���,��ǰ������ѡ��ȷ��
	beginAnimation2(NULL);   
	return true;
}



