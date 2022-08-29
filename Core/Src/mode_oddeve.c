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
#define SETTING_NOW_DAYLEFT		2

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

	//1.从flash读取当前的限号规则是多少
//	static unsigned int sizeStruct = 0;
//	static driverInfo_s inFlash;
//	byte ruleInFlash = 0;
//	sizeStruct = sizeof(driverInfo);
//	readFlash(START_FLASH_ADDRESS, (uint16_t *)&inFlash, sizeStruct);
//	//2.如果是123规则，则直接从presetMode中读取默认规则
//	driverInfo.limitCouple.rule = 1;							//使用预设模式1,以防数据存储失败
//	driverInfo.limitCouple.matchTimes = 3;									//每次进入界面，设置一次自动匹配次数
//	if(inFlash.limitCouple.rule > 0 && inFlash.limitCouple.rule < 4)
//	{

//	}
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
//		static unsigned int sizeStruct = 0;
//		sizeStruct = sizeof(driverInfo);
//		writeFlash(START_FLASH_ADDRESS, (uint16_t *)&driverInfo, sizeStruct);
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
	
	if(setting.val == 1)
		fontArr[0] = 16;//单
	else
		fontArr[0] = 17;//双
	LOOP(6, cnt)
	{			
		draw_bitmap(PARAM_X+cnt*16, 8, textDisplay[fontArr[cnt]], 16, 16, NOINVERT, 0);
	}	
	setMenuOption(1, buff, NULL, selectOddEvenNum);
}

static void showDaysleftStr(void)
{
	char buff[14] = {0};
	byte fontArr[4] = {15, 1, 25, 26};
	byte fontWidth = 0;
	const byte* bitmp;
	byte bits = 1, hun, ten, one;

	hun = setting.val / 100;
	ten = setting.val / 10 % 10;
	one = setting.val % 10;	
	if(setting.val / 100)
	{
		bits = 3;
		draw_bitmap(PARAM_X, 8+16, numFont11x16[hun], 11, 16, NOINVERT, 0);
		draw_bitmap(PARAM_X+11, 8+16, numFont11x16[ten], 11, 16, NOINVERT, 0);	
		draw_bitmap(PARAM_X+22, 8+16, numFont11x16[one], 11, 16, NOINVERT, 0);
	}		
	else if(setting.val / 10)
	{
		bits = 2;
		draw_bitmap(PARAM_X, 8+16, numFont11x16[ten], 11, 16, NOINVERT, 0);	
		draw_bitmap(PARAM_X+11, 8+16, numFont11x16[one], 11, 16, NOINVERT, 0);
	}		
	else 
	{
		bits = 1;
		draw_bitmap(PARAM_X, 8+16, numFont11x16[one], 11, 16, NOINVERT, 0);	
	}		

	LOOP(4, cnt)
	{	
		bitmp = textDisplay[fontArr[cnt]];
		fontWidth = 16;					
		draw_bitmap(PARAM_X+bits*11+cnt*16, 8+16, bitmp, fontWidth, 16, NOINVERT, 0);
	}	
	setMenuOption(3, buff, NULL, selectDaysleftNum);
}

static display_t oddEvenNumDraw()
{
	byte x = PARAM_X;//79
	byte y = 8;
	byte w = 16;
	byte hun = 0, ten = 0, one = 0;		//剩余天数是几位数

	switch(setting.now)
	{
		case SETTING_NOW_ODDEVEN:	
			y = 8;	
			draw_clearArea(x, y, w);
			draw_clearArea(x, y+8, w);
			draw_bitmap(x, y, textDisplay[setting.val+16] , 16, 16, INVERT, 0);//
			break;
		case SETTING_NOW_DAYLEFT:
			y = 8+16;
			w = 11;
			hun = setting.val / 100;
			ten = setting.val / 10 % 10;
			one = setting.val % 10;
			if(hun)	//百位有数字
			{
				draw_clearArea(x, y, 33);
				draw_clearArea(x, y+8, 33);
				draw_bitmap(x, y, numFont11x16[hun] , 11, 16, INVERT, 0);
				draw_bitmap(x+11, y, numFont11x16[ten] , 11, 16, INVERT, 0);
				draw_bitmap(x+22, y, numFont11x16[one] , 11, 16, INVERT, 0);
			}
			else if(ten)
			{
				draw_clearArea(x, y, 22);
				draw_clearArea(x, y+8, 22);
				draw_bitmap(x, y, numFont11x16[ten] , 11, 16, INVERT, 0);
				draw_bitmap(x+11, y, numFont11x16[one] , 11, 16, INVERT, 0);
			}
			else
			{
				draw_clearArea(x, y, 11);
				draw_clearArea(x, y+8, 11);
				draw_bitmap(x, y, numFont11x16[one] , 11, 16, INVERT, 0);	
			}			
			break;
		default:
			break;
	}

	return DISPLAY_DONE;
}

static void paramDataUp()
{
	setting.val++;
	if(setting.val > 180)
		setting.val = 0;
	paramUpdate();
	driverInfo.limitDS.normal = setting.val;			//把当前调整后的值存在用户结构体内
}

static void paramDataDown()
{
	setting.val--;
		
	byte max = 180;
	if(setting.val > max)																		// Overflow
		setting.val = max;
	paramUpdate();
	driverInfo.limitDS.normal = setting.val;			//把当前调整后的值存在用户结构体内
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
			//setting.val = driverInfo.limitDS.normal;
			break;
		case SETTING_NOW_ODDEVEN:
			//setting.val = driverInfo.limitDS.normal;	
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
			setting.now = SETTING_NOW_DAYLEFT;
			//setting.val = driverInfo.limitDS.validDays;
			break;
		case SETTING_NOW_DAYLEFT:
			//setting.val = driverInfo.limitDS.validDays;
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
	byte oddOrEven = 0;
	switch(setting.now)
	{
		case SETTING_NOW_ODDEVEN:
			oddOrEven = setting.val;
			oddOrEven ? (oddOrEven = 0) : (oddOrEven = 1);
			setting.val = oddOrEven;
			break;
		case SETTING_NOW_DAYLEFT:
			//setting.val = driverInfo.limitDS.validDays;
			break;
		default:
			break;
	}
}















