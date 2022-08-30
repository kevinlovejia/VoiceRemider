/*
 * Project: Voice Reminder
 * Author: Wei Lee, kevin_jia@foxmail.com
 * Copyright: (C) 2022 by Wei
 * License: GNU GPL v3 (see License.txt)
 */
#include "common.h"
#include "main.h"

#define OPTION_COUNT					6

#define PARAM_X								6//
#define SETTING_NOW_NONE			0
#define SETTING_NOW_ODDEVEN		1
#define SETTING_NOW_START_Y		2
#define SETTING_NOW_START_M		3
#define SETTING_NOW_START_D		4
#define SETTING_NOW_END_Y			5
#define SETTING_NOW_END_M			6
#define SETTING_NOW_END_D			7

static prev_menu_s prevMenuData;
static void mSelect(void);
static void itemLoader(byte num);
static void mDown(void);
static void mUp(void);
static void showOddEvenStr(void);
static void showDaysleftStr(void);
static void selectOddEvenNum(void);
static void selectDaysleftNum(void);
void checkDataValid(void);
void paramUpdate(void);

void modeOddevenSelect()
{
	setMenuInfo(OPTION_COUNT, MENU_TYPE_STR, PSTR(STR_LIMITODDOREVEN));
	setMenuFuncs(mDown, mSelect, mUp, itemLoader);

	setPrevMenuOpen(&prevMenuData, modeOddevenSelect);	

	//1.从flash读取当前的单双号模式和退出天数
	static unsigned int sizeStruct = 0;
	sizeStruct = sizeof(driverInfo);
	readFlash(START_FLASH_ADDRESS, (uint16_t *)&driverInfo, sizeStruct);
	checkDataValid();
	menuData.selected = 1;	
	beginAnimation2(NULL);
}

static void mDown()
{
	nextOption();

	// Some lines are blank, skip them
	if(menuData.selected == 0 || menuData.selected == 2 || menuData.selected == 4)
		nextOption();
}

static void mUp()
{
	prevOption();

	// Some lines are blank, skip them
	if(menuData.selected == 0 || menuData.selected == 2 || menuData.selected == 4)
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
	byte fontArr[4] = {27, 28, 25, 26};
	byte xArr[10] = {6, 6+11,  6+11+11+8, 6+11+11+8+11, 6+11+11+8+11+11+8, 6+11+11+8+11+11+8+11};
	byte highB = 0, lowB = 0;
	LOOP(6, cnt)					//把开始时间的6个数字画出来
	{	
		highB = driverInfo.limitDS.startYMD[cnt/2] / 10;
		lowB = driverInfo.limitDS.startYMD[cnt/2] % 10;
		draw_bitmap(xArr[cnt], 	 8+16, numFont11x16[highB], 11, 16, NOINVERT, 0);	
		draw_bitmap(xArr[cnt+1], 8+16, numFont11x16[lowB],  11, 16, NOINVERT, 0);	
		cnt += 2;
	}
	LOOP(6, cnt)					//把结束时间的6个数字画出来
	{	
		highB = driverInfo.limitDS.endYMD[cnt/2] / 10;
		lowB = driverInfo.limitDS.endYMD[cnt/2] % 10;
		draw_bitmap(xArr[cnt], 	 8+16+16, numFont11x16[highB], 11, 16, NOINVERT, 0);	
		draw_bitmap(xArr[cnt+1], 8+16+16, numFont11x16[lowB],  11, 16, NOINVERT, 0);	
		cnt += 2;
	}
	LOOP(32, y)//把开始结束时间的4个点画出来
	{
		draw_bitmap(28, 8+16+y, numFont11x16[11], 8, 16, NOINVERT, 0);	
		draw_bitmap(58, 8+16+y, numFont11x16[11],  8, 16, NOINVERT, 0);
		y += 16;
	}

	LOOP(4, cnt)//把开始结束这4个字画出来
	{
		draw_bitmap(88+cnt%2*16, 8+16+cnt/2*16, textDisplay[fontArr[cnt]], 16, 16, NOINVERT, 0);
	}		

	setMenuOption(5, buff, NULL, selectDaysleftNum);
}

static display_t oddEvenNumDraw()		//选中&退出界面
{
	byte x = PARAM_X;
	byte y = 24;   //0  1    2         3         4              5
	byte w = 16;   //2   2    .        0          8             .
	byte xArr[10] = {6, 6+11, 6+11+11, 6+11+11+8, 6+11+11+8+11, 6+11+11+8+11+11,\
				  //3				   0                     开88                     始104 
					6+11+11+8+11+11+8, 6+11+11+8+11+11+8+11, 6+11+11+8+11+11+8+11+11, 6+11+11+8+11+11+8+11+11+16};
	byte highB = 0, lowB = 0;
	switch(setting.now)
	{
		case SETTING_NOW_ODDEVEN:	
			y = 8;	
			draw_clearArea(x, y, w);
			draw_clearArea(x, y+8, w);
			draw_bitmap(x, y, textDisplay[driverInfo.limitDS.normal+16] , 16, 16, INVERT, 0);//
			break;
		case SETTING_NOW_START_Y:
			y = 8+16;
			highB = driverInfo.limitDS.startYMD[0] / 10;
			lowB = driverInfo.limitDS.startYMD[0] % 10;
			draw_clearArea(xArr[0], y, 22);
			draw_clearArea(xArr[0], y+8, 22);
			draw_bitmap(xArr[0], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			draw_bitmap(xArr[1], y, numFont11x16[lowB] , 11, 16, INVERT, 0);
			break;
		case SETTING_NOW_START_M:
			y = 8+16;				
			highB = driverInfo.limitDS.startYMD[1] / 10;
			lowB = driverInfo.limitDS.startYMD[1] % 10;
			draw_clearArea(xArr[3], y, 22);
			draw_clearArea(xArr[3], y+8, 22);
			draw_bitmap(xArr[3], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			draw_bitmap(xArr[4], y, numFont11x16[lowB] , 11, 16, INVERT, 0);
			break;
		case SETTING_NOW_START_D:
			y = 8+16;	
			highB = driverInfo.limitDS.startYMD[2] / 10;
			lowB = driverInfo.limitDS.startYMD[2] % 10;
			draw_clearArea(xArr[6], y, 22);
			draw_clearArea(xArr[6], y+8, 22);
			draw_bitmap(xArr[6], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			draw_bitmap(xArr[7], y, numFont11x16[lowB] , 11, 16, INVERT, 0);
			break;
		case SETTING_NOW_END_Y:
			y = 8+16+16;	
			highB = driverInfo.limitDS.endYMD[0] / 10;
			lowB = driverInfo.limitDS.endYMD[0] % 10;
			draw_clearArea(xArr[0], y, 22);
			draw_clearArea(xArr[0], y+8, 22);
			draw_bitmap(xArr[0], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			draw_bitmap(xArr[1], y, numFont11x16[lowB] , 11, 16, INVERT, 0);
			break;
		case SETTING_NOW_END_M:
			y = 8+16+16;	
			highB = driverInfo.limitDS.endYMD[1] / 10;
			lowB = driverInfo.limitDS.endYMD[1] % 10;
			draw_clearArea(xArr[3], y, 22);
			draw_clearArea(xArr[3], y+8, 22);
			draw_bitmap(xArr[3], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			draw_bitmap(xArr[4], y, numFont11x16[lowB] , 11, 16, INVERT, 0);
			break;
		case SETTING_NOW_END_D:
			y = 8+16+16;	
			highB = driverInfo.limitDS.endYMD[2] / 10;
			lowB = driverInfo.limitDS.endYMD[2] % 10;
			draw_clearArea(xArr[6], y, 22);
			draw_clearArea(xArr[6], y+8, 22);
			draw_bitmap(xArr[6], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			draw_bitmap(xArr[7], y, numFont11x16[lowB] , 11, 16, INVERT, 0);
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
			setting.now = SETTING_NOW_START_Y;
			setting.val = driverInfo.limitDS.startYMD[0];
			break;
		case SETTING_NOW_START_Y:
			setting.now = SETTING_NOW_START_M;
			setting.val = driverInfo.limitDS.startYMD[0];
			break;
		case SETTING_NOW_START_M:	
			setting.now = SETTING_NOW_START_D;		
			setting.val = driverInfo.limitDS.startYMD[1];
			break;
		case SETTING_NOW_START_D:
			setting.now = SETTING_NOW_END_Y;
			setting.val = driverInfo.limitDS.startYMD[2];
			break;
		case SETTING_NOW_END_Y:
			setting.now = SETTING_NOW_END_M;
			setting.val = driverInfo.limitDS.endYMD[0];
			break;
		case SETTING_NOW_END_M:
			setting.now = SETTING_NOW_END_D;
			setting.val = driverInfo.limitDS.endYMD[1];
			break;
		case SETTING_NOW_END_D:
			setting.val = driverInfo.limitDS.endYMD[2];
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
		case SETTING_NOW_START_Y:
			driverInfo.limitDS.startYMD[0] = setting.val;
			break;
		case SETTING_NOW_START_M:
			driverInfo.limitDS.startYMD[1] = setting.val;
			break;
		case SETTING_NOW_START_D:
			driverInfo.limitDS.startYMD[2] = setting.val;
			break;		
		case SETTING_NOW_END_Y:
			driverInfo.limitDS.endYMD[0] = setting.val;
			break;		
		case SETTING_NOW_END_M:
			driverInfo.limitDS.endYMD[1] = setting.val;
			break;		
		case SETTING_NOW_END_D:
			driverInfo.limitDS.endYMD[2] = setting.val;
			break;
		default:
			break;
	}
}

void checkDataValid(void)
{
	byte cnt;
	for(cnt = 0; cnt < 3; cnt++)
	{
		if(driverInfo.limitDS.startYMD[cnt] == 0)
			break;
	}
	if(cnt != 3) //年月日有0出现
	{
		//更新driverInfo.limitDS.startYMD[cnt]
		driverInfo.limitDS.startYMD[0] = timeDate.date.year;
		driverInfo.limitDS.startYMD[1] = timeDate.date.month;
		driverInfo.limitDS.startYMD[2] = timeDate.date.day;
	}
	else
	{
		for(cnt = 0; cnt < 3; cnt++)
		{
			if(driverInfo.limitDS.endYMD[cnt] == 0)
				break;
		}
		if(cnt != 3) //年月日有0出现
		{
			//更新driverInfo.limitDS.endYMD[cnt]
			driverInfo.limitDS.endYMD[0] = timeDate.date.year;
			driverInfo.limitDS.endYMD[1] = timeDate.date.month;
			driverInfo.limitDS.endYMD[2] = timeDate.date.day;
		}
	}
}















