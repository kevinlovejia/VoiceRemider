/*
 * Project: Voice Reminder
 * Author: Wei Lee, kevin_jia@foxmail.com
 * Copyright: (C) 2022 by Wei
 * License: GNU GPL v3 (see License.txt)
 */
#include "common.h"
#include "main.h"

#define OPTION_COUNT				1		//���������ݿ�ѡ�񣬾���Ҫ����Ϊ����
#define SETTING_NOW_NONE		0
#define SETTING_NOW_TAILNO	1
#define SETTING_NOW_FINISH	2

static prev_menu_s prevMenuData;
driverInfo_s driverInfo;
static bool saved;

static void mSelect(void);
static void itemLoader(byte num);
//static display_t thisdraw(void);
//static void setMenuOptions(void);
//static void mDown(void);
//static void mUp(void);
static void showTailNumStr(void);
static void selectTailNum(void);
static void beginSelect(void);
static void endSelect(void);
static void saveUserData(void);
static void setMenuOptions(void);

void tailnum_select()
{
	/************************************/
	//memcpy(&timeDateSet, &timeDate, sizeof(timeDate_s));
	//timeMode = appConfig.timeMode;

	//saved = false;

	setMenuInfo(OPTION_COUNT, MENU_TYPE_ICON, PSTR(STR_TAILNOMENU));
	//setMenuFuncs(mUp, mSelect, mDown, itemLoader);
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);
	setPrevMenuOpen(&prevMenuData, tailnum_select);

	menuData.selected = 0;
	
	beginAnimation2(NULL);
	
	static unsigned int sizeStruct = 0;
	static driverInfo_s inFlash;
	byte ruleInFlash = 0;
	sizeStruct = sizeof(driverInfo);
	readFlash(START_FLASH_ADDRESS, (uint16_t *)&inFlash, sizeStruct);
	setting.val = inFlash.tailNo;	
}

static void mSelect()
{
	bool isExiting = exitSelected();
	if(isExiting)
	{
		//save rules to flash
		static unsigned int sizeStruct = 0;
		sizeStruct = sizeof(driverInfo);
		writeFlash(START_FLASH_ADDRESS, (uint16_t *)&driverInfo, sizeStruct);
	}
		//appconfig_save();

	setPrevMenuExit(&prevMenuData);
	doAction(isExiting);
}

//static void mDown()
//{
//	nextOption();

//	// Some lines are blank, skip them
//	if(menuData.selected == 0)
//		nextOption();
//}

//static void mUp()
//{
//	prevOption();

//	// Some lines are blank, skip them
//	if(menuData.selected == 0)
//		prevOption();
//}

static void itemLoader(byte num)
{
	UNUSED(num);
	setMenuOptions();
	addBackOption();
//	UNUSED(num);
//	showTailNumStr();
	//setMenuOption_P(3, saved ? PSTR(STR_SAVED) : PSTR(STR_SAVE), NULL, saveUserData);
	//addBackOption();
}

static void setMenuOptions()
{
//	setMenuOption_P(0, PSTR(STR_UI), menu_volume[volUI], setVolumeUI);
//	setMenuOption_P(1, PSTR(STR_ALARMS), menu_volume[volAlarm], setVolumeAlarm);
//	setMenuOption_P(2, PSTR(STR_HOURBEEPS), menu_volume[volHour], setVolumeHour);
	
	byte fontPos = driverInfo.tailNo;																	//��ת��ֱ��ʹ�û�Ӳ�����ϣ�
	setMenuOption_P(0, PSTR(STR_UI), arial_font[fontPos], selectTailNum);
}

static void makeTailNumStr(char* buff)
{
	//byte timeModeVal = (timeDateSet.time.ampm != CHAR_24) ? 12 : 24;
//	sprintf_P(buff, PSTR("Tail No: %hhu"), driverInfo.tailNo);
	byte fontPos = driverInfo.tailNo;	
	draw_bitmap(4, 28 + 4 - 16, menu_tailnum[1] , 32, 32, NOINVERT, 0);			//β
	draw_bitmap(4+34, 28 + 4 - 16, menu_tailnum[2] , 32, 32, NOINVERT, 0);	//��
	draw_bitmap(4+34+34, 24, midFont[fontPos] , 19, 24, INVERT, 0);// 19x24
}

//static void selectTailNum(void)
//{
//	driverInfo.tailNo = setting.val;
//}

static void saveUserData()
{

}

static void showTailNumStr()
{
	char buff[10];
	makeTailNumStr(buff);
	setMenuOption(1, buff, NULL, selectTailNum);
}



static display_t tailNumDraw()
{
//	byte x;
//	byte y;
//	byte w = 5;
//	
//	x = 70;//10*7 (>Tail No: )=10*(5+1+1)Ӣ���ַ����5�����Ҽ�1���ؼ��
//	y = 16;	

//	draw_clearArea(x, y, w);

////	char buff[max(BUFFSIZE_STR_MONTHS, 5)];
//	char buff[5];
//	sprintf_P(buff, PSTR("%hhu"), setting.val);

//	draw_string(buff, true, x, y);

//	return DISPLAY_DONE;
//	byte x = 72;
//	byte y = 24;
//	byte w = 19;

//	draw_clearArea(x, y, w);
//	draw_clearArea(x, y+8, w);
//	draw_clearArea(x, y+16, w);
//	draw_bitmap(x, y, midFont[setting.val] , 19, 24, INVERT, 0);// 19x24

	byte x = 48;
	byte y = 16;
	byte w = 32;

	draw_clearArea(x, y, w);
	draw_clearArea(x, y+8, w);
	draw_clearArea(x, y+16, w);
	draw_clearArea(x, y+24, w);
	draw_bitmap(x, y, arial_font[setting.val] , 32, 32, INVERT, 0);// 32x32

	return DISPLAY_DONE;
}

static void tailNumDataUp()
{
	setting.val++;
	if(setting.val > 9)
		setting.val = 0;

//	if(((setting.now == SETTING_NOW_HOUR && timeMode == TIMEMODE_12HR) || setting.now == SETTING_NOW_DATE) && setting.val == 0)
//		setting.val = 1;

//	saved = false;
}

static void tailNumDataDown()
{
	setting.val--;
	
	if(setting.val == 0xFF)
	{
		setting.val = 9;
	}
	
	byte max = 9;
	if(setting.val > max) // Overflow
		setting.val = max;

	//saved = false;
}

static void beginSelect()
{
	setMenuFuncs(tailNumDataUp, mSelect, tailNumDataDown, itemLoader);
	menuData.func.draw = tailNumDraw;
}

static void endSelect()
{
	setting.now = SETTING_NOW_NONE;
	//setMenuFuncs(mDown, mSelect, mUp, itemLoader);
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);
	menuData.func.draw = NULL;
}

static void selectTailNum()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_NOW_TAILNO;
			driverInfo.tailNo = setting.val;
			break;
		case SETTING_NOW_TAILNO:
			driverInfo.tailNo = setting.val;
			if(driverInfo.tailNo == 0xFF)
				driverInfo.tailNo = 9;
//			setting.now = SETTING_NOW_NONE;
			setting.val = driverInfo.tailNo;
			
			endSelect();
			break;
		default: // Also SETTING_NOW_FINISH
			driverInfo.tailNo = setting.val;

			endSelect();
			break;
	}
	
}









