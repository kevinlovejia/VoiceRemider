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
driverInfo_s driverInfo, *driverInfo_P;
//static driverInfo_s flashInfo;
static void mSelect(void);
static void itemLoader(byte num);
//static void showTailNumStr(void);
static void selectTailNum(void);
static void beginSelect(void);
static void endSelect(void);
static void setMenuOptions(void);

void tailnum_select()
{
	setMenuInfo(OPTION_COUNT, MENU_TYPE_ICON, PSTR(STR_TAILNOMENU));
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);
	setPrevMenuOpen(&prevMenuData, tailnum_select);	
	
	readFlash(START_FLASH_ADDRESS, (uint16_t *)driverInfo_P, sizeof(driverInfo_s));
	if(driverInfo_P->tailNo > 9)
		setting.val = 0;
	else
		setting.val = driverInfo_P->tailNo & 0xFF;

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
		//save rules to flash
		unsigned int sizeStruct = 0;
		sizeStruct = sizeof(driverInfo);
		driverInfo_s *flashCont;
		flashCont = (driverInfo_s *)malloc(sizeof(driverInfo_s));
		if(flashCont == NULL)
			while(1);//Todo: 		
		readFlash(START_FLASH_ADDRESS, (uint16_t *)flashCont, sizeStruct);
		if(flashCont->tailNo != driverInfo_P->tailNo)
			wFlash(START_FLASH_ADDRESS, (uint16_t *)driverInfo_P, sizeStruct);
		free(flashCont);
		flashCont = NULL;
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
	byte fontPos = driverInfo_P->tailNo & 0xFF;																	//不转换直接使用会硬件故障，
	if(fontPos > 9)	//出错或者还未选择尾号
		fontPos = 0;
	setMenuOption_P(0, PSTR(STR_UI), arial_font[fontPos], selectTailNum);
}

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
	byte max = 9;
	if(setting.val > max) // Overflow
		setting.val = max;
}

static void beginSelect()
{
	setMenuFuncs(tailNumDataUp, mSelect, tailNumDataDown, itemLoader);
	menuData.func.draw = tailNumDraw;
}

static void endSelect()
{
	setting.now = SETTING_NOW_NONE;
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
			driverInfo_P->tailNo = setting.val;
			break;
		case SETTING_NOW_TAILNO:
			driverInfo_P->tailNo = setting.val;
			if(driverInfo_P->tailNo > 9)
				driverInfo_P->tailNo = 9;
			setting.val = driverInfo_P->tailNo;			
			endSelect();
			break;
		default: 
			driverInfo_P->tailNo = setting.val;
			endSelect();
			break;
	}	
}





