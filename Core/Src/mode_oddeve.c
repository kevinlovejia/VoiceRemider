/*
 * Project: Voice Reminder
 * Author: Wei Lee, kevin_jia@foxmail.com
 * Copyright: (C) 2022 by Wei
 * License: GNU GPL v3 (see License.txt)
 */
#include "common.h"
#include "main.h"

#define OPTION_COUNT	4

#define PARAM_X								6//
#define SETTING_NOW_NONE			0
#define SETTING_NOW_ODDEVEN		1
#define SETTING_NOW_DAYS_HUN	2
#define SETTING_NOW_DAYS_TEN	3
#define SETTING_NOW_DAYS_ONE	4

static prev_menu_s prevMenuData;
static void mSelect(void);
static void itemLoader(byte num);
static void mDown(void);
static void mUp(void);
static void showOddEvenStr(void);
static void showDaysleftStr(void);
static void selectOddEvenNum(void);
static void selectDaysleftNum(void);
void paramUpdate(void);

void modeOddevenSelect()
{
//	setMenuInfo(OPTION_COUNT, MENU_TYPE_ICON, PSTR(STR_MODEMENU));
//	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);
	setMenuInfo(OPTION_COUNT, MENU_TYPE_STR, PSTR(STR_LIMITODDOREVEN));
	setMenuFuncs(mDown, mSelect, mUp, itemLoader);

	setPrevMenuOpen(&prevMenuData, modeOddevenSelect);	

	//1.从flash读取当前的单双号模式和退出天数
	static unsigned int sizeStruct = 0;
	//static driverInfo_s inFlash;
	//byte ruleInFlash = 0;
	sizeStruct = sizeof(driverInfo);
	readFlash(START_FLASH_ADDRESS, (uint16_t *)&driverInfo, sizeStruct);

	menuData.selected = 1;	
	beginAnimation2(NULL);
}

static void mDown()
{
	nextOption();

	// Some lines are blank, skip them
	if(menuData.selected == 0 || menuData.selected == 2)
		nextOption();
}

static void mUp()
{
	prevOption();

	// Some lines are blank, skip them
	if(menuData.selected == 0 || menuData.selected == 2)
		prevOption();
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
	setPrevMenuExit(&prevMenuData);
	doAction(exitSelected());
	
}

static void itemLoader(byte num)
{
	UNUSED(num);
	showOddEvenStr();
	showDaysleftStr();
	addBackOption();
}

static void showOddEvenStr(void)
{
	char buff[14] = {0};
	byte fontArr[6] = {16, 18, 16, 15, 19, 7};
	
	if(driverInfo.limitDS.normal == 0)
		fontArr[0] = 16;//单
	else
		fontArr[0] = 17;//双
	LOOP(6, cnt)
	{			
		draw_bitmap(PARAM_X+cnt*16, 8, textDisplay[fontArr[cnt]], 16, 16, NOINVERT, 0);
	}	
	setMenuOption(1, buff, NULL, selectOddEvenNum);
}

static void showDaysleftStr(void)		//绘制N日后退出界面
{
	char buff[14] = {0};
	byte fontArr[4] = {15, 1, 25, 26};
	byte fontWidth = 0;
	const byte* bitmp;
	byte bits = 1;// hun, ten, one;
	u16 newDays = 0;
	newDays = driverInfo.limitDS.validDaysH *100 + driverInfo.limitDS.validDaysT *10 + \
		driverInfo.limitDS.validDaysO;

	if(newDays / 100 || setting.now >= 2)
	{
		bits = 3;
		draw_bitmap(PARAM_X, 8+16, numFont11x16[driverInfo.limitDS.validDaysH], 11, 16, NOINVERT, 0);
		draw_bitmap(PARAM_X+12, 8+16, numFont11x16[driverInfo.limitDS.validDaysT], 11, 16, NOINVERT, 0);	
		draw_bitmap(PARAM_X+24, 8+16, numFont11x16[driverInfo.limitDS.validDaysO], 11, 16, NOINVERT, 0);
	}		
	else if(newDays / 10)
	{
		bits = 2;
		draw_bitmap(PARAM_X, 8+16, numFont11x16[driverInfo.limitDS.validDaysT], 11, 16, NOINVERT, 0);	
		draw_bitmap(PARAM_X+12, 8+16, numFont11x16[driverInfo.limitDS.validDaysO], 11, 16, NOINVERT, 0);
	}		
	else 
	{
		bits = 1;
		draw_bitmap(PARAM_X, 8+16, numFont11x16[driverInfo.limitDS.validDaysO], 11, 16, NOINVERT, 0);	
	}		

	LOOP(4, cnt)
	{	
		bitmp = textDisplay[fontArr[cnt]];
		fontWidth = 16;					
		draw_bitmap(PARAM_X+bits*12+cnt*16, 8+16, bitmp, fontWidth, 16, NOINVERT, 0);
	}	
	setMenuOption(3, buff, NULL, selectDaysleftNum);
}

static display_t oddEvenNumDraw()		//选中 N日后退出
{
	byte x = PARAM_X;//79
	byte y = 24;
	byte w = 16;
//	byte hun = 0, ten = 0, one = 0;		//剩余天数是几位数

//	hun = setting.val / 100;
//	ten = setting.val / 10 % 10;
//	one = setting.val % 10;

	switch(setting.now)
	{
		case SETTING_NOW_ODDEVEN:	
			y = 8;	
			draw_clearArea(x, y, w);
			draw_clearArea(x, y+8, w);
			draw_bitmap(x, y, textDisplay[driverInfo.limitDS.normal+16] , 16, 16, INVERT, 0);//
			break;
		case SETTING_NOW_DAYS_HUN:
			w = 11;
			draw_clearArea(x, y, 35);
			draw_clearArea(x, y+8, 35);
			draw_bitmap(x, y, numFont11x16[driverInfo.limitDS.validDaysH] , 11, 16, INVERT, 0);
			draw_bitmap(x+12, y, numFont11x16[driverInfo.limitDS.validDaysT] , 11, 16, NOINVERT, 0);
			draw_bitmap(x+24, y, numFont11x16[driverInfo.limitDS.validDaysO] , 11, 16, NOINVERT, 0);
			break;
		case SETTING_NOW_DAYS_TEN:	
			draw_clearArea(x, y, 35);
			draw_clearArea(x, y+8, 35);
			draw_bitmap(x, y, numFont11x16[driverInfo.limitDS.validDaysH] , 11, 16, NOINVERT, 0);
			draw_bitmap(x+12, y, numFont11x16[driverInfo.limitDS.validDaysT] , 11, 16, INVERT, 0);
			draw_bitmap(x+24, y, numFont11x16[driverInfo.limitDS.validDaysO] , 11, 16, NOINVERT, 0);
			break;
		case SETTING_NOW_DAYS_ONE:
			draw_clearArea(x, y, 35);
			draw_clearArea(x, y+8, 35);
			draw_bitmap(x, y, numFont11x16[driverInfo.limitDS.validDaysH] , 11, 16, NOINVERT, 0);
			draw_bitmap(x+12, y, numFont11x16[driverInfo.limitDS.validDaysT] , 11, 16, NOINVERT, 0);
			draw_bitmap(x+24, y, numFont11x16[driverInfo.limitDS.validDaysO] , 11, 16, INVERT, 0);	
			break;
		default:
			break;
	}

	return DISPLAY_DONE;
}

static void paramDataUp()
{
	setting.val++;
	if(setting.val > 9)
		setting.val = 0;
	paramUpdate();
	//driverInfo.limitDS.normal = setting.val;			//把当前调整后的值存在用户结构体内
}

static void paramDataDown()
{
	setting.val--;
		
	byte max = 9;
	if(setting.val > max)																		// Overflow
		setting.val = max;
	paramUpdate();
	//driverInfo.limitDS.normal = setting.val;			//把当前调整后的值存在用户结构体内
}

static void beginSelect()
{
	setMenuFuncs(paramDataUp, mSelect, paramDataDown, itemLoader);
	menuData.func.draw = oddEvenNumDraw;
}

static void endSelect()
{
	setting.now = SETTING_NOW_NONE;
	setMenuFuncs(mDown, mSelect, mUp, itemLoader);
	menuData.func.draw = NULL;
}

static void selectOddEvenNum()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_NOW_ODDEVEN;
			setting.val = driverInfo.limitDS.normal;
			break;
		case SETTING_NOW_ODDEVEN:
			setting.val = driverInfo.limitDS.normal;				
			endSelect();		
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}	
}
static void selectDaysleftNum()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_NOW_DAYS_HUN;
			setting.val = driverInfo.limitDS.validDaysH;
			break;
		case SETTING_NOW_DAYS_HUN:
			setting.now = SETTING_NOW_DAYS_TEN;
			setting.val = driverInfo.limitDS.validDaysH;
			break;
		case SETTING_NOW_DAYS_TEN:	
			setting.now = SETTING_NOW_DAYS_ONE;		
			setting.val = driverInfo.limitDS.validDaysT;
			break;
		case SETTING_NOW_DAYS_ONE:
			setting.now = SETTING_NOW_NONE;
			setting.val = driverInfo.limitDS.validDaysO;
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}	
}

void paramUpdate(void)
{
	switch(setting.now)
	{
		case SETTING_NOW_ODDEVEN:
			driverInfo.limitDS.normal ? (driverInfo.limitDS.normal = 0) : (driverInfo.limitDS.normal = 1);
			break;
		case SETTING_NOW_DAYS_HUN:
			driverInfo.limitDS.validDaysH = setting.val;
			break;
		case SETTING_NOW_DAYS_TEN:
			driverInfo.limitDS.validDaysT = setting.val;
			break;
		case SETTING_NOW_DAYS_ONE:
			driverInfo.limitDS.validDaysO = setting.val;
			break;
		default:
			break;
	}
}















