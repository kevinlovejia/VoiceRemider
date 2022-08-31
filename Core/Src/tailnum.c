/*
 * Project: Voice Reminder
 * Author: Wei Lee, kevin_jia@foxmail.com
 * Copyright: (C) 2022 by Wei
 * License: GNU GPL v3 (see License.txt)
 */
#include "common.h"
#include "main.h"

#define OPTION_COUNT				1		//????????????????????????????
#define SETTING_NOW_NONE		0
#define SETTING_NOW_TAILNO	1
#define SETTING_NOW_FINISH	2

static prev_menu_s prevMenuData;
driverInfo_s driverInfo;
static driverInfo_s flashInfo;
static void mSelect(void);
static void itemLoader(byte num);
//static void showTailNumStr(void);
static void selectTailNum(void);
static void beginSelect(void);
static void endSelect(void);
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
	
	readFlash(START_FLASH_ADDRESS, (uint16_t *)&driverInfo, sizeof(driverInfo));
	if(driverInfo.tailNo > 9)
		setting.val = 0;
	else
		setting.val = driverInfo.tailNo & 0xFF;

	menuData.selected = 0;	
	
	beginAnimation2(NULL);
	

}

static void mSelect()
{
	bool isExiting = exitSelected();

	setPrevMenuExit(&prevMenuData);
	doAction(isExiting);
	if(isExiting)
	{
//		//save rules to flash
		unsigned int sizeStruct = 0;
		sizeStruct = sizeof(driverInfo);
		writeFlash(START_FLASH_ADDRESS, (uint16_t *)&driverInfo, sizeStruct);
	}
}

static void itemLoader(byte num)
{
	UNUSED(num);
	setMenuOptions();
	addBackOption();
}

static void setMenuOptions()
{
//	setMenuOption_P(0, PSTR(STR_UI), menu_volume[volUI], setVolumeUI);
//	setMenuOption_P(1, PSTR(STR_ALARMS), menu_volume[volAlarm], setVolumeAlarm);
//	setMenuOption_P(2, PSTR(STR_HOURBEEPS), menu_volume[volHour], setVolumeHour);
	
	byte fontPos = driverInfo.tailNo;																	//不转换直接使用会硬件故障，
	setMenuOption_P(0, PSTR(STR_UI), arial_font[fontPos], selectTailNum);
}

//static void saveUserData()
//{

//}

static display_t tailNumDraw()
{
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
			if(driverInfo.tailNo > 9)
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




//static void showTailNumStr()
//{
//	char buff[10];
//	makeTailNumStr(buff);
//	setMenuOption(1, buff, NULL, selectTailNum);
//}





