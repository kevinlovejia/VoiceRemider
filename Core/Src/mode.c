/*
 * Project: Voice Reminder
 * Author: Wei Lee, kevin_jia@foxmail.com
 * Copyright: (C) 2022 by Wei
 * License: GNU GPL v3 (see License.txt)
 */
#include "common.h"
#include "main.h"

#define OPTION_COUNT	3


static prev_menu_s prevMenuData;
static void mSelect(void);
static void itemLoader(byte num);
static display_t thisdraw(void);
static void setMenuOptions(void);
static void selectCouple(void);
static void selectOddOrEven(void);
static void selectCoupleLoop(void);
//static driverInfo_s driverInfo;

void mode_select()
{
// Create copy of current time & date
//	memcpy(&timeDateSet, &timeDate, sizeof(timeDate_s));
//	timeMode = appConfig.timeMode;

//	saved = false;

	setMenuInfo(OPTION_COUNT, MENU_TYPE_ICON, PSTR(STR_MODEMENU));
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);

//	setMenuOptions();
	
	menuData.func.draw = thisdraw; //绑定菜单画图函数，不知道退出时，这个函数并没有继续执行的原因

	setPrevMenuOpen(&prevMenuData, mode_select);

//	menuData.selected = 1;
	
	beginAnimation2(NULL);
}

static void mSelect()
{
	bool isExiting = exitSelected();
	if(isExiting)
		appconfig_save();
 
	setPrevMenuExit(&prevMenuData);
	doAction(isExiting);	
}

static void itemLoader(byte num)
{
	UNUSED(num);
	setMenuOptions();
	addBackOption();
}

static display_t thisdraw()
{
	char name[3];
	
	switch(menuData.selected)
	{
		case 0:sprintf(name,"%d",appConfig.volUI);break;
		case 1:sprintf(name,"%d",appConfig.volAlarm);break;
		case 2:sprintf(name,"%d",appConfig.volHour);break;
	}
	
	draw_string((char*)name, false, 122, 0);
	return DISPLAY_DONE;
}

static void setMenuOptions()
{
	setMenuOption_P(0, PSTR(STR_LIMITODDOREVEN), 	menu_threemode[1], 	selectCouple);			//俩号
	setMenuOption_P(1, PSTR(STR_LIMITCOUPLE), 		menu_threemode[0], 	selectOddOrEven);		//单双
	setMenuOption_P(2, PSTR(STR_LIMITCOUPLELOOP), menu_threemode[2], 	selectCoupleLoop);	//轮替
}	

static void selectCouple()
{		
	modeCoupleSelect();
}

static void selectOddOrEven()
{
	modeOddevenSelect();
}

static void selectCoupleLoop()
{
//	volHour = setVolume(volHour);

	//appConfig.volHour = setVolume(appConfig.volHour);

	driverInfo_P->mode = 3;
}













