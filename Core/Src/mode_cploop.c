/*
 * Project: Voice Reminder
 * Author: Wei Lee, kevin_jia@foxmail.com
 * Copyright: (C) 2022 by Wei
 * License: GNU GPL v3 (see License.txt)
 */
#include "common.h"
#include "main.h"

#define OPTION_COUNT			32

static prev_menu_s prevMenuData;
static void mSelect(void);
static void itemLoader(byte num);
static void mDown(void);
static void mUp(void);
static void showStageStr(byte num);
static void checkDate(void);
static void select1StartDate(void);
static void select1EndDate(void);
static void select1Rule(void);
static void select2StartDate(void);
static void select2EndDate(void);
static void select2Rule(void);
static void select3StartDate(void);
static void select3EndDate(void);
static void select3Rule(void);
static void select4StartDate(void);
static void select4EndDate(void);
static void select4Rule(void);
static void dateUpdate(void);
static void ruleMatch(void);

limCoupLocal_s loopDetails;	//自动匹配次数

menu_f dateNumDrawFunc[] = {select1StartDate, select1EndDate, NULL, select1Rule,\
														select2StartDate, select2EndDate, NULL, select2Rule,\
														select3StartDate, select3EndDate, NULL, select3Rule,\
														select4StartDate, select4EndDate, NULL, select4Rule};

byte preDateSet[][6] = {
	{	22,03,04,22,06,03	},
	{	22,06,04,22, 9,02	},
	{	22, 9,03,23,00,01	},
	{	23,00,02,23,03,02	},
};//预设时间数组

void modeCpLoopSelect()
{
	setMenuInfo(OPTION_COUNT, MENU_TYPE_STR, PSTR(STR_LIMITODDOREVEN));
	setMenuFuncs(mDown, mSelect, mUp, itemLoader);

	setPrevMenuOpen(&prevMenuData, modeCpLoopSelect);
	//1.从flash读取当前的单双号模式和退出时间
	static unsigned int sizeStruct = 0;
	sizeStruct = sizeof(driverInfo_s);
	readFlash(START_FLASH_ADDRESS, (uint16_t *)driverInfo_P, sizeStruct);
	driverInfo_P->mode = 3;
	checkDate();
	menuData.selected = 1;	
	loopDetails.matchTimes = 5;			//匹配5次后退出自动匹配
	loopDetails.usedRule1 = false;
	loopDetails.usedRule2 = false;
	loopDetails.usedRule3 = false;
	loopDetails.usedRule4 = false;
	loopDetails.usedRule5 = false;
	beginAnimation2(NULL);
}

static void mDown()
{
	nextOption();
	// Some lines are blank, skip them
	if(menuData.selected % 2 == 0 && menuData.selected < 32)
		nextOption();
}

static void mUp()
{
	prevOption();
	if(menuData.selected % 2 == 0 && menuData.selected < 32)
		prevOption();
}

static void mSelect()
{
	bool isExiting = exitSelected();
	if(isExiting)
	{
		//save rules to flash	
		// unsigned int ret = 99;
		// readFlash(START_FLASH_ADDRESS, (uint16_t *)flashCont, sizeof(driverInfo_s));
		// if(flashCont->limitJJJ_stage[0].normal != driverInfo_P->limitJJJ_stage[0].normal)
		// 	wFlash(START_FLASH_ADDRESS, (uint16_t *)driverInfo_P, sizeof(driverInfo_s));
		// else
		// {
		// 	ret = memcmp(flashCont->limitJJJ_stage[0].endYMD, driverInfo_P->limitJJJ_stage[0].endYMD, \
		// 	sizeof(flashCont->limitJJJ_stage[0].endYMD));
		// 	if(ret != 0)
		// 		wFlash(START_FLASH_ADDRESS, (uint16_t *)driverInfo_P, sizeof(driverInfo_s));
		// }
		// if(flashCont->mode != driverInfo_P->mode)
		// 	wFlash(START_FLASH_ADDRESS, (uint16_t *)driverInfo_P, sizeof(driverInfo_s));
	}
	setPrevMenuExit(&prevMenuData);
	doAction(exitSelected());	
}

static void itemLoader(byte num)
{
	//UNUSED(num);
	showStageStr(num);
	addBackOption();
}

static void showStageStr(byte num)
{
	char buff[14] = {0};
	byte xArr[11] = { 6, 22, 30, 41, 52, 60, 71, 82, 90, 101, 112};
	byte wArr[11] = {16,  8, 11, 11,  8, 11, 11,  8, 11,  11,  16};
	byte numxArr[10] = { 9, 20, 33, 44, 57, 68, 81, 92, 105, 116};
	//byte paraArr[6] = {16, 18, 16, 15, 19, 7};
	byte lowB, highB, interVal, flagMenu = 0, posY = 0, line = 0;
	const byte *bmArr[11] = {textDisplay[29], numFont11x16[10], numFont11x16[0], \
		numFont11x16[0], numFont11x16[11], numFont11x16[0], numFont11x16[0], \
		numFont11x16[11], numFont11x16[0], numFont11x16[0], textDisplay[28]};	
	if(num%2 == 0 && num <= 32)
		return;
	else
		flagMenu = num / 2;	//1357=>0123
	
	posY = flagMenu*16 + 8;
	posY -= menuData.scroll*8;
	if(posY == 0)														//???y?????0??????α???
		draw_clearArea(14, 0, 94);						//???< MODE >
	
	num = num / 8;		//13579
	line = flagMenu % 4;
	switch(line)
	{
		case 0:
			//***************①:22.04.04始
			//1.split year, update year of bmArr
			bmArr[0] = textDisplay[29+num];
			interVal = driverInfo_P->limitJJJ_stage[num].startYMD[0];
			highB = interVal / 10;
			lowB = interVal % 10;
			bmArr[2] = numFont11x16[highB];
			bmArr[3] = numFont11x16[lowB];
			//2.split month, update month of bmArr
			interVal = driverInfo_P->limitJJJ_stage[num].startYMD[1];
			interVal += 1;//显示的时候才把月份加1，其余都用0-11表示
			highB = interVal / 10;
			lowB = interVal % 10;
			bmArr[5] = numFont11x16[highB];
			bmArr[6] = numFont11x16[lowB];	
			//3.split day, update day of bmArr
			interVal = driverInfo_P->limitJJJ_stage[num].startYMD[2];
			highB = interVal / 10;
			lowB = interVal % 10;
			bmArr[8] = numFont11x16[highB];
			bmArr[9] = numFont11x16[lowB];	
			LOOP(11, cnt)
				draw_bitmap(xArr[cnt], 	posY, bmArr[cnt], wArr[cnt], 16, NOINVERT, 0);
			setMenuOption(flagMenu*2+1, buff, NULL, dateNumDrawFunc[flagMenu]);
			break;
		case 1:
			//***************  22.07.03止
			//1.split year, update year of bmArr
			interVal = driverInfo_P->limitJJJ_stage[num].endYMD[0];
			highB = interVal / 10;
			lowB = interVal % 10;
			bmArr[2] = numFont11x16[highB];
			bmArr[3] = numFont11x16[lowB];
			//2.split month, update month of bmArr
			interVal = driverInfo_P->limitJJJ_stage[num].endYMD[1];
			interVal += 1;//显示的时候才把月份加1，其余都用0-11表示
			highB = interVal / 10;
			lowB = interVal % 10;
			bmArr[5] = numFont11x16[highB];
			bmArr[6] = numFont11x16[lowB];	
			//3.split day, update day of bmArr
			interVal = driverInfo_P->limitJJJ_stage[num].endYMD[2];
			highB = interVal / 10;
			lowB = interVal % 10;
			bmArr[8] = numFont11x16[highB];
			bmArr[9] = numFont11x16[lowB];	
			bmArr[10] = textDisplay[19];	//止
			for(byte cnt = 2; cnt < 11; cnt++)
				draw_bitmap(xArr[cnt], 	posY, bmArr[cnt], wArr[cnt], 16, NOINVERT, 0);
			setMenuOption(flagMenu*2+1, buff, NULL, dateNumDrawFunc[flagMenu]);
			break;
		case 2:
			//***************一二三四五9-13
			for(byte cnt = 0; cnt < 5; cnt++)
				draw_bitmap(12*(cnt*2+1), posY, textDisplay[9+cnt], 16, 16, NOINVERT, 0);
			break;
		case 3:
			//***************27 38 49 50 16			
			LOOP(10, cnt)
			{
				interVal = driverInfo_P->limitJJJ_stage[num].limitNum[cnt];
				draw_bitmap(numxArr[cnt], posY, numFont11x16[interVal], 11, 16, NOINVERT, 0);
			}	
			setMenuOption(flagMenu*2+1, buff, NULL, dateNumDrawFunc[flagMenu]);	
			break;
		default:
			break;
	}
}

static display_t manyNumDraw()		//四个阶段选项选中显示
{
	byte flag = 0;
	byte y = 24;  
	byte w = 22;  
	byte xArr[3] = { 30, 60, 90};
	byte numxArr[10] = { 9, 20, 33, 44, 57, 68, 81, 92, 105, 116};
	byte highB = 0, lowB = 0, num = 0;

	// if(posY == 0)		
	// 	draw_clearArea(14, 0, 94);	
	num = (setting.now - 1) / 16;		//获得limitJJJ_stage的下标
	switch(setting.now)
	{
		case SETTING_I_ST_YEAR:	
			y = 8 - menuData.scroll*8;	
			flag = 0;
			highB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_I_ST_MONT:
			y = 8 - menuData.scroll*8;	
			flag = 1;
			highB = (driverInfo_P->limitJJJ_stage[num].startYMD[flag]+1) / 10;//需要显示的时候月份加1
			lowB = (driverInfo_P->limitJJJ_stage[num].startYMD[flag]+1) % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_I_ST_DAYS:
			y = 8 - menuData.scroll*8;	
			flag = 2;			
			highB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_I_ED_YEAR:
			y = 8 - menuData.scroll*8 + 16;	
			flag = 0;
			highB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_I_ED_MONT:
			y = 8 - menuData.scroll*8 + 16;	
			flag = 1;
			highB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_I_ED_DAYS:
			y = 8 - menuData.scroll*8 + 16;	
			flag = 2;
			highB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_I_MON_1:
			y = 8 - menuData.scroll*8 + 16*3;	
			flag = 0;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);			
			break;
		case SETTING_I_MON_2:
			y = 8 - menuData.scroll*8 + 16*3;	
			flag = 1;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);			
			break;	
		case SETTING_I_TUE_1:
			y = 8 - menuData.scroll*8 + 16*3;	
			flag = 2;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);			
			break;
		case SETTING_I_TUE_2:
			y = 8 - menuData.scroll*8 + 16*3;	
			flag = 3;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);	
			break;
		case SETTING_I_WED_1:
			y = 8 - menuData.scroll*8 + 16*3;	
			flag = 4;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);	
			break;
		case SETTING_I_WED_2:
			y = 8 - menuData.scroll*8 + 16*3;	
			flag = 5;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);	
			break;	
		case SETTING_I_THU_1:
			y = 8 - menuData.scroll*8 + 16*3;	
			flag = 6;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);	
			break;		
		case SETTING_I_THU_2:
			y = 8 - menuData.scroll*8 + 16*3;	
			flag = 7;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);	
			break;	
		case SETTING_I_FRI_1:
			y = 8 - menuData.scroll*8 + 16*3;	
			flag = 8;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);	
			break;	
		case SETTING_I_FRI_2:
			y = 8 - menuData.scroll*8 + 16*3;	
			flag = 9;
			w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);	
			break;	
		case SETTING_II_ST_YEAR:
			y = 8 - menuData.scroll*8 + 16*4;	
			flag = 0;
			highB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;		
		case SETTING_II_ST_MONT:
			y = 8 - menuData.scroll*8 + 16*4;	
			flag = 1;
			highB = (driverInfo_P->limitJJJ_stage[num].startYMD[flag]+1) / 10;
			lowB = (driverInfo_P->limitJJJ_stage[num].startYMD[flag]+1) % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_II_ST_DAYS:
			y = 8 - menuData.scroll*8 + 16*4;	
			flag = 2;
			highB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_II_ED_YEAR:
			y = 8 - menuData.scroll*8 + 16*5;	
			flag = 0;
			highB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_II_ED_MONT:
			y = 8 - menuData.scroll*8 + 16*5;	
			flag = 1;
			highB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_II_ED_DAYS:
			y = 8 - menuData.scroll*8 + 16*5;	
			flag = 2;
			highB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_II_MON_1:
			y = 8 - menuData.scroll*8 + 16*7;	
			flag = 0;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_II_MON_2:
			y = 8 - menuData.scroll*8 + 16*7;	
			flag = 1;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_II_TUE_1:
			y = 8 - menuData.scroll*8 + 16*7;	
			flag = 2;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_II_TUE_2:
			y = 8 - menuData.scroll*8 + 16*7;	
			flag = 3;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_II_WED_1:
			y = 8 - menuData.scroll*8 + 16*7;	
			flag = 4;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_II_WED_2:
			y = 8 - menuData.scroll*8 + 16*7;	
			flag = 5;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_II_THU_1:
			y = 8 - menuData.scroll*8 + 16*7;	
			flag = 6;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_II_THU_2:
			y = 8 - menuData.scroll*8 + 16*7;	
			flag = 7;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_II_FRI_1:
			y = 8 - menuData.scroll*8 + 16*7;	
			flag = 8;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_II_FRI_2:
			y = 8 - menuData.scroll*8 + 16*7;	
			flag = 9;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_III_ST_YEAR:
			y = 8 - menuData.scroll*8 + 16*8;	
			flag = 0;
			highB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_III_ST_MONT:
			y = 8 - menuData.scroll*8 + 16*8;	
			flag = 1;
			highB = (driverInfo_P->limitJJJ_stage[num].startYMD[flag]+1) / 10;
			lowB = (driverInfo_P->limitJJJ_stage[num].startYMD[flag]+1) % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_III_ST_DAYS:
			y = 8 - menuData.scroll*8 + 16*8;	
			flag = 2;
			highB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_III_ED_YEAR:
			y = 8 - menuData.scroll*8 + 16*9;	
			flag = 0;
			highB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_III_ED_MONT:
			y = 8 - menuData.scroll*8 + 16*9;	
			flag = 1;
			highB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_III_ED_DAYS:
			y = 8 - menuData.scroll*8 + 16*9;	
			flag = 2;
			highB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_III_MON_1:
			y = 8 - menuData.scroll*8 + 16*11;	
			flag = 0;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_III_MON_2:
			y = 8 - menuData.scroll*8 + 16*11;	
			flag = 1;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_III_TUE_1:
			y = 8 - menuData.scroll*8 + 16*11;	
			flag = 2;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_III_TUE_2:
			y = 8 - menuData.scroll*8 + 16*11;	
			flag = 3;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_III_WED_1:
			y = 8 - menuData.scroll*8 + 16*11;	
			flag = 4;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_III_WED_2:
			y = 8 - menuData.scroll*8 + 16*11;	
			flag = 5;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_III_THU_1:
			y = 8 - menuData.scroll*8 + 16*11;	
			flag = 6;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_III_THU_2:
			y = 8 - menuData.scroll*8 + 16*11;	
			flag = 7;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_III_FRI_1:
			y = 8 - menuData.scroll*8 + 16*11;	
			flag = 8;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_III_FRI_2:
			y = 8 - menuData.scroll*8 + 16*11;	
			flag = 9;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_IV_ST_YEAR:
			y = 8 - menuData.scroll*8 + 16*12;	
			flag = 0;
			highB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_IV_ST_MONT:
			y = 8 - menuData.scroll*8 + 16*12;	
			flag = 1;
			highB = (driverInfo_P->limitJJJ_stage[num].startYMD[flag]+1) / 10;
			lowB = (driverInfo_P->limitJJJ_stage[num].startYMD[flag]+1) % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_IV_ST_DAYS:
			y = 8 - menuData.scroll*8 + 16*12;	
			flag = 2;
			highB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].startYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_IV_ED_YEAR:
			y = 8 - menuData.scroll*8 + 16*13;	
			flag = 0;
			highB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_IV_ED_MONT:
			y = 8 - menuData.scroll*8 + 16*13;	
			flag = 1;
			highB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_IV_ED_DAYS:
			y = 8 - menuData.scroll*8 + 16*13;	
			flag = 2;
			highB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] / 10;
			lowB = driverInfo_P->limitJJJ_stage[num].endYMD[flag] % 10;
			draw_clearArea(xArr[flag], y, w);
			draw_clearArea(xArr[flag], y+8, w);
			draw_bitmap(xArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);//
			draw_bitmap(xArr[flag]+11, y, numFont11x16[lowB] , 11, 16, INVERT, 0);//
			break;
		case SETTING_IV_MON_1:
			y = 8 - menuData.scroll*8 + 16*15;	
			flag = 0;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_IV_MON_2:
			y = 8 - menuData.scroll*8 + 16*15;	
			flag = 1;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_IV_TUE_1:
			y = 8 - menuData.scroll*8 + 16*15;	
			flag = 2;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_IV_TUE_2:
			y = 8 - menuData.scroll*8 + 16*15;	
			flag = 3;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_IV_WED_1:
			y = 8 - menuData.scroll*8 + 16*15;	
			flag = 4;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_IV_WED_2:
			y = 8 - menuData.scroll*8 + 16*15;	
			flag = 5;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_IV_THU_1:
			y = 8 - menuData.scroll*8 + 16*15;	
			flag = 6;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_IV_THU_2:
			y = 8 - menuData.scroll*8 + 16*15;	
			flag = 7;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_IV_FRI_1:
			y = 8 - menuData.scroll*8 + 16*15;	
			flag = 8;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		case SETTING_IV_FRI_2:
			y = 8 - menuData.scroll*8 + 16*15;	
			flag = 9;w = 11;
			highB = driverInfo_P->limitJJJ_stage[num].limitNum[flag];			
			draw_clearArea(numxArr[flag], y, w);
			draw_clearArea(numxArr[flag], y+8, w);
			draw_bitmap(numxArr[flag], y, numFont11x16[highB] , 11, 16, INVERT, 0);
			break;
		default:
			break;
	}
	return DISPLAY_DONE;
}

static void paramDataUp()
{
	//static u16 days = 0;
	setting.val++;
	dateUpdate();
	ruleMatch();
	//days = formatDaysCounter(driverInfo_P->limitJJJ_stage[0].startYMD, driverInfo_P->limitJJJ_stage[0].endYMD);
	//driverInfo_P->limitJJJ_stage[0].normal = setting.val;			//?????????????????????????
}

static void paramDataDown()
{
	//static u16 days = 0;
	setting.val--;
	dateUpdate();
	ruleMatch();
	//days = formatDaysCounter(driverInfo_P->limitJJJ_stage[0].startYMD, driverInfo_P->limitJJJ_stage[0].endYMD);
	//driverInfo_P->limitJJJ_stage[0].normal = setting.val;			//?????????????????????????
}

static void beginSelect()
{
	setMenuFuncs(paramDataUp, mSelect, paramDataDown, itemLoader);
	menuData.func.draw = manyNumDraw;
}

static void endSelect()
{
	setting.now = SETTING_NOW_NONE;
	setMenuFuncs(mDown, mSelect, mUp, itemLoader);
	menuData.func.draw = NULL;
}
/***********STAGE 1************/
static void select1StartDate()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_I_ST_YEAR;
			setting.val = driverInfo_P->limitJJJ_stage[0].startYMD[0];
			break;
		case SETTING_I_ST_YEAR:
			setting.now = SETTING_I_ST_MONT;
			setting.val = driverInfo_P->limitJJJ_stage[0].startYMD[1];			
			break;
		case SETTING_I_ST_MONT:
			setting.now = SETTING_I_ST_DAYS;
			setting.val = driverInfo_P->limitJJJ_stage[0].startYMD[2];		
			break;
		case SETTING_I_ST_DAYS:
			setting.val = driverInfo_P->limitJJJ_stage[0].startYMD[2];		
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}
}
static void select1EndDate()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_I_ED_YEAR;
			setting.val = driverInfo_P->limitJJJ_stage[0].endYMD[0];
			break;
		case SETTING_I_ED_YEAR:
			setting.now = SETTING_I_ED_MONT;
			setting.val = driverInfo_P->limitJJJ_stage[0].endYMD[1];			
			break;
		case SETTING_I_ED_MONT:
			setting.now = SETTING_I_ED_DAYS;
			setting.val = driverInfo_P->limitJJJ_stage[0].endYMD[2];		
			break;
		case SETTING_I_ED_DAYS:
			setting.val = driverInfo_P->limitJJJ_stage[0].endYMD[2];		
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}
}
static void select1Rule()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_I_MON_1;
			setting.val = driverInfo_P->limitJJJ_stage[0].limitNum[0];
			break;
		case SETTING_I_MON_1:
			setting.now = SETTING_I_MON_2;
			setting.val = driverInfo_P->limitJJJ_stage[0].limitNum[1];			
			break;
		case SETTING_I_MON_2:
			setting.now =  SETTING_I_TUE_1;
			setting.val = driverInfo_P->limitJJJ_stage[0].limitNum[2];		
			break;
		case  SETTING_I_TUE_1:
			setting.now =  SETTING_I_TUE_2;
			setting.val = driverInfo_P->limitJJJ_stage[0].limitNum[3];	
			break;
		case  SETTING_I_TUE_2:
			setting.now =  SETTING_I_WED_1;
			setting.val = driverInfo_P->limitJJJ_stage[0].limitNum[4];	
			break;
		case  SETTING_I_WED_1:
			setting.now =  SETTING_I_WED_2;
			setting.val = driverInfo_P->limitJJJ_stage[0].limitNum[5];	
			break;
		case  SETTING_I_WED_2:
			setting.now =  SETTING_I_THU_1;
			setting.val = driverInfo_P->limitJJJ_stage[0].limitNum[6];	
			break;
		case  SETTING_I_THU_1:
			setting.now =  SETTING_I_THU_2;
			setting.val = driverInfo_P->limitJJJ_stage[0].limitNum[7];	
			break;
		case  SETTING_I_THU_2:
			setting.now =  SETTING_I_FRI_1;
			setting.val = driverInfo_P->limitJJJ_stage[0].limitNum[8];	
			break;
		case  SETTING_I_FRI_1:
			setting.now =  SETTING_I_FRI_2;
			setting.val = driverInfo_P->limitJJJ_stage[0].limitNum[9];	
			break;
		case  SETTING_I_FRI_2:
			setting.val = driverInfo_P->limitJJJ_stage[0].limitNum[9];	
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}
}
/***********STAGE 2************/
static void select2StartDate()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_II_ST_YEAR;
			setting.val = driverInfo_P->limitJJJ_stage[1].startYMD[0];
			break;
		case SETTING_II_ST_YEAR:
			setting.now = SETTING_II_ST_MONT;
			setting.val = driverInfo_P->limitJJJ_stage[1].startYMD[1];			
			break;
		case SETTING_II_ST_MONT:
			setting.now = SETTING_II_ST_DAYS;
			setting.val = driverInfo_P->limitJJJ_stage[1].startYMD[2];		
			break;
		case SETTING_II_ST_DAYS:
			setting.val = driverInfo_P->limitJJJ_stage[1].startYMD[2];		
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}
}
static void select2EndDate()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_II_ED_YEAR;
			setting.val = driverInfo_P->limitJJJ_stage[1].endYMD[0];
			break;
		case SETTING_II_ED_YEAR:
			setting.now = SETTING_II_ED_MONT;
			setting.val = driverInfo_P->limitJJJ_stage[1].endYMD[1];			
			break;
		case SETTING_II_ED_MONT:
			setting.now = SETTING_II_ED_DAYS;
			setting.val = driverInfo_P->limitJJJ_stage[1].endYMD[2];		
			break;
		case SETTING_II_ED_DAYS:
			setting.val = driverInfo_P->limitJJJ_stage[1].endYMD[2];		
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}
}
static void select2Rule()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_II_MON_1;
			setting.val = driverInfo_P->limitJJJ_stage[1].limitNum[0];
			break;
		case SETTING_II_MON_1:
			setting.now = SETTING_II_MON_2;
			setting.val = driverInfo_P->limitJJJ_stage[1].limitNum[1];			
			break;
		case SETTING_II_MON_2:
			setting.now =  SETTING_II_TUE_1;
			setting.val = driverInfo_P->limitJJJ_stage[1].limitNum[2];		
			break;
		case  SETTING_II_TUE_1:
			setting.now =  SETTING_II_TUE_2;
			setting.val = driverInfo_P->limitJJJ_stage[1].limitNum[3];	
			break;
		case  SETTING_II_TUE_2:
			setting.now =  SETTING_II_WED_1;
			setting.val = driverInfo_P->limitJJJ_stage[1].limitNum[4];	
			break;
		case  SETTING_II_WED_1:
			setting.now =  SETTING_II_WED_2;
			setting.val = driverInfo_P->limitJJJ_stage[1].limitNum[5];	
			break;
		case  SETTING_II_WED_2:
			setting.now =  SETTING_II_THU_1;
			setting.val = driverInfo_P->limitJJJ_stage[1].limitNum[6];	
			break;
		case  SETTING_II_THU_1:
			setting.now =  SETTING_II_THU_2;
			setting.val = driverInfo_P->limitJJJ_stage[1].limitNum[7];	
			break;
		case  SETTING_II_THU_2:
			setting.now =  SETTING_II_FRI_1;
			setting.val = driverInfo_P->limitJJJ_stage[1].limitNum[8];	
			break;
		case  SETTING_II_FRI_1:
			setting.now =  SETTING_II_FRI_2;
			setting.val = driverInfo_P->limitJJJ_stage[1].limitNum[9];	
			break;
		case  SETTING_II_FRI_2:
			setting.val = driverInfo_P->limitJJJ_stage[1].limitNum[9];	
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}
}
/***********STAGE 3************/
static void select3StartDate()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_III_ST_YEAR;
			setting.val = driverInfo_P->limitJJJ_stage[2].startYMD[0];
			break;
		case SETTING_III_ST_YEAR:
			setting.now = SETTING_III_ST_MONT;
			setting.val = driverInfo_P->limitJJJ_stage[2].startYMD[1];			
			break;
		case SETTING_III_ST_MONT:
			setting.now = SETTING_III_ST_DAYS;
			setting.val = driverInfo_P->limitJJJ_stage[2].startYMD[2];		
			break;
		case SETTING_III_ST_DAYS:
			setting.val = driverInfo_P->limitJJJ_stage[2].startYMD[2];		
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}
}
static void select3EndDate()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_III_ED_YEAR;
			setting.val = driverInfo_P->limitJJJ_stage[2].endYMD[0];
			break;
		case SETTING_III_ED_YEAR:
			setting.now = SETTING_III_ED_MONT;
			setting.val = driverInfo_P->limitJJJ_stage[2].endYMD[1];			
			break;
		case SETTING_III_ED_MONT:
			setting.now = SETTING_III_ED_DAYS;
			setting.val = driverInfo_P->limitJJJ_stage[2].endYMD[2];		
			break;
		case SETTING_III_ED_DAYS:
			setting.val = driverInfo_P->limitJJJ_stage[2].endYMD[2];		
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}
}
static void select3Rule()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_III_MON_1;
			setting.val = driverInfo_P->limitJJJ_stage[2].limitNum[0];
			break;
		case SETTING_III_MON_1:
			setting.now = SETTING_III_MON_2;
			setting.val = driverInfo_P->limitJJJ_stage[2].limitNum[1];			
			break;
		case SETTING_III_MON_2:
			setting.now =  SETTING_III_TUE_1;
			setting.val = driverInfo_P->limitJJJ_stage[2].limitNum[2];		
			break;
		case  SETTING_III_TUE_1:
			setting.now =  SETTING_III_TUE_2;
			setting.val = driverInfo_P->limitJJJ_stage[2].limitNum[3];	
			break;
		case  SETTING_III_TUE_2:
			setting.now =  SETTING_III_WED_1;
			setting.val = driverInfo_P->limitJJJ_stage[2].limitNum[4];	
			break;
		case  SETTING_III_WED_1:
			setting.now =  SETTING_III_WED_2;
			setting.val = driverInfo_P->limitJJJ_stage[2].limitNum[5];	
			break;
		case  SETTING_III_WED_2:
			setting.now =  SETTING_III_THU_1;
			setting.val = driverInfo_P->limitJJJ_stage[2].limitNum[6];	
			break;
		case  SETTING_III_THU_1:
			setting.now =  SETTING_III_THU_2;
			setting.val = driverInfo_P->limitJJJ_stage[2].limitNum[7];	
			break;
		case  SETTING_III_THU_2:
			setting.now =  SETTING_III_FRI_1;
			setting.val = driverInfo_P->limitJJJ_stage[2].limitNum[8];	
			break;
		case  SETTING_III_FRI_1:
			setting.now =  SETTING_III_FRI_2;
			setting.val = driverInfo_P->limitJJJ_stage[2].limitNum[9];	
			break;
		case  SETTING_III_FRI_2:
			setting.val = driverInfo_P->limitJJJ_stage[2].limitNum[9];	
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}
}
/***********STAGE 4************/
static void select4StartDate()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_IV_ST_YEAR;
			setting.val = driverInfo_P->limitJJJ_stage[3].startYMD[0];
			break;
		case SETTING_IV_ST_YEAR:
			setting.now = SETTING_IV_ST_MONT;
			setting.val = driverInfo_P->limitJJJ_stage[3].startYMD[1];			
			break;
		case SETTING_IV_ST_MONT:
			setting.now = SETTING_IV_ST_DAYS;
			setting.val = driverInfo_P->limitJJJ_stage[3].startYMD[2];		
			break;
		case SETTING_IV_ST_DAYS:
			setting.val = driverInfo_P->limitJJJ_stage[3].startYMD[2];		
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}
}
static void select4EndDate()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_IV_ED_YEAR;
			setting.val = driverInfo_P->limitJJJ_stage[3].endYMD[0];
			break;
		case SETTING_IV_ED_YEAR:
			setting.now = SETTING_IV_ED_MONT;
			setting.val = driverInfo_P->limitJJJ_stage[3].endYMD[1];			
			break;
		case SETTING_IV_ED_MONT:
			setting.now = SETTING_IV_ED_DAYS;
			setting.val = driverInfo_P->limitJJJ_stage[3].endYMD[2];		
			break;
		case SETTING_IV_ED_DAYS:
			setting.val = driverInfo_P->limitJJJ_stage[3].endYMD[2];		
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}
}
static void select4Rule()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_IV_MON_1;
			setting.val = driverInfo_P->limitJJJ_stage[3].limitNum[0];
			break;
		case SETTING_IV_MON_1:
			setting.now = SETTING_IV_MON_2;
			setting.val = driverInfo_P->limitJJJ_stage[3].limitNum[1];			
			break;
		case SETTING_IV_MON_2:
			setting.now =  SETTING_IV_TUE_1;
			setting.val = driverInfo_P->limitJJJ_stage[3].limitNum[2];		
			break;
		case  SETTING_IV_TUE_1:
			setting.now =  SETTING_IV_TUE_2;
			setting.val = driverInfo_P->limitJJJ_stage[3].limitNum[3];	
			break;
		case  SETTING_IV_TUE_2:
			setting.now =  SETTING_IV_WED_1;
			setting.val = driverInfo_P->limitJJJ_stage[3].limitNum[4];	
			break;
		case  SETTING_IV_WED_1:
			setting.now =  SETTING_IV_WED_2;
			setting.val = driverInfo_P->limitJJJ_stage[3].limitNum[5];	
			break;
		case  SETTING_IV_WED_2:
			setting.now =  SETTING_IV_THU_1;
			setting.val = driverInfo_P->limitJJJ_stage[3].limitNum[6];	
			break;
		case  SETTING_IV_THU_1:
			setting.now =  SETTING_IV_THU_2;
			setting.val = driverInfo_P->limitJJJ_stage[3].limitNum[7];	
			break;
		case  SETTING_IV_THU_2:
			setting.now =  SETTING_IV_FRI_1;
			setting.val = driverInfo_P->limitJJJ_stage[3].limitNum[8];	
			break;
		case  SETTING_IV_FRI_1:
			setting.now =  SETTING_IV_FRI_2;
			setting.val = driverInfo_P->limitJJJ_stage[3].limitNum[9];	
			break;
		case  SETTING_IV_FRI_2:
			setting.val = driverInfo_P->limitJJJ_stage[3].limitNum[9];	
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}
}

static void dateUpdate(void)
{
	byte daysOfMonth = 0;
	switch(setting.now)
	{
		case SETTING_I_ST_YEAR:
			if(setting.val > 99)
				setting.val = 22;
			else if(setting.val < 22)
				setting.val = 99;
			driverInfo_P->limitJJJ_stage[0].startYMD[0] = setting.val;
			//update stage I end year and other 3 stage start & end year
			LOOP(2, cnt)
			{
				if(driverInfo_P->limitJJJ_stage[cnt].endYMD[0] < setting.val)
					driverInfo_P->limitJJJ_stage[cnt].endYMD[0] = setting.val;
				if(driverInfo_P->limitJJJ_stage[cnt+2].endYMD[0] == setting.val)
					driverInfo_P->limitJJJ_stage[cnt+2].endYMD[0] = setting.val+1;
				if(driverInfo_P->limitJJJ_stage[1+cnt].startYMD[0] < setting.val)
					driverInfo_P->limitJJJ_stage[1+cnt].startYMD[0] = setting.val;	
			}
			if(driverInfo_P->limitJJJ_stage[3].startYMD[0] == setting.val)
				driverInfo_P->limitJJJ_stage[3].startYMD[0] = setting.val+1;			
			break;
		case SETTING_I_ST_MONT:
			if(setting.val == 0xFF)
				setting.val = 11;
			else if(setting.val > 11)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[0].startYMD[1] = setting.val;
			//every change calculate days of this 			
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[0].startYMD[1], \
				driverInfo_P->limitJJJ_stage[0].startYMD[0]);
			if(driverInfo_P->limitJJJ_stage[0].startYMD[2] > daysOfMonth)
				driverInfo_P->limitJJJ_stage[0].startYMD[2] = daysOfMonth;
			break;
		case SETTING_I_ST_DAYS:			
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[0].startYMD[1], \
				driverInfo_P->limitJJJ_stage[0].startYMD[0]);
			if(setting.val > daysOfMonth)		//
				setting.val = 1;
			else if(setting.val < 1)
				setting.val = daysOfMonth;		//
			driverInfo_P->limitJJJ_stage[0].startYMD[2] = setting.val;
			break;		
		case SETTING_I_ED_YEAR:		//need larger than start year
			if(setting.val > 99)
				setting.val = driverInfo_P->limitJJJ_stage[0].startYMD[0];
			else if(setting.val < 22)
				setting.val = 99;
			if(setting.val < driverInfo_P->limitJJJ_stage[0].startYMD[0])
			{
				setting.val = driverInfo_P->limitJJJ_stage[0].startYMD[0];
				driverInfo_P->limitJJJ_stage[0].endYMD[0] = setting.val;
				//ToDo: PLAY BOOM BOOM EFFECT, CANT BE SMALLER
			}
			else
				driverInfo_P->limitJJJ_stage[0].endYMD[0] = setting.val;
			break;		
		case SETTING_I_ED_MONT:		//need larger than start month
			if(setting.val == 0xFF)
				setting.val = 11;
			else if(setting.val > 11)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[0].endYMD[1] = setting.val;
			//every change calculate days of this 			
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[0].endYMD[1], \
				driverInfo_P->limitJJJ_stage[0].endYMD[0]);
			if(driverInfo_P->limitJJJ_stage[0].endYMD[2] > daysOfMonth)
				driverInfo_P->limitJJJ_stage[0].endYMD[2] = daysOfMonth;
			break;		
		case SETTING_I_ED_DAYS:
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[0].endYMD[1], \
				driverInfo_P->limitJJJ_stage[0].endYMD[0]);
			if(setting.val > daysOfMonth)		//
				setting.val = 1;
			else if(setting.val < 1)
				setting.val = daysOfMonth;		//
			driverInfo_P->limitJJJ_stage[0].endYMD[2] = setting.val;
			break;
		case SETTING_I_MON_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[0].limitNum[0] = setting.val;	
			break;
		case SETTING_I_MON_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[0].limitNum[1] = setting.val;	
			break;	
		case SETTING_I_TUE_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[0].limitNum[2] = setting.val;	
			break;	
		case SETTING_I_TUE_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[0].limitNum[3] = setting.val;	
			break;	
		case SETTING_I_WED_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[0].limitNum[4] = setting.val;	
			break;
		case SETTING_I_WED_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[0].limitNum[5] = setting.val;	
			break;
		case SETTING_I_THU_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[0].limitNum[6] = setting.val;	
			break;	
		case SETTING_I_THU_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[0].limitNum[7] = setting.val;	
			break;	
		case SETTING_I_FRI_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[0].limitNum[8] = setting.val;	
			break;	
		case SETTING_I_FRI_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[0].limitNum[9] = setting.val;	
			break;	
		case SETTING_II_ST_YEAR:
			if(setting.val > 99)
				setting.val = 22;
			else if(setting.val < 22)
				setting.val = 99;
			driverInfo_P->limitJJJ_stage[1].startYMD[0] = setting.val;				
			break;
		case SETTING_II_ST_MONT:
			if(setting.val == 0xFF)
				setting.val = 11;
			else if(setting.val > 11)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[1].startYMD[1] = setting.val;
			//every change calculate days of this 			
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[1].startYMD[1], \
				driverInfo_P->limitJJJ_stage[1].startYMD[0]);
			if(driverInfo_P->limitJJJ_stage[1].startYMD[2] > daysOfMonth)
				driverInfo_P->limitJJJ_stage[1].startYMD[2] = daysOfMonth;
			break;
		case SETTING_II_ST_DAYS:			
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[1].startYMD[1], \
				driverInfo_P->limitJJJ_stage[1].startYMD[0]);
			if(setting.val > daysOfMonth)		//
				setting.val = 1;
			else if(setting.val < 1)
				setting.val = daysOfMonth;		//
			driverInfo_P->limitJJJ_stage[1].startYMD[2] = setting.val;
			break;		
		case SETTING_II_ED_YEAR:		//need larger than start year
			if(setting.val > 99)
				setting.val = driverInfo_P->limitJJJ_stage[1].startYMD[0];
			else if(setting.val < 22)
				setting.val = 99;
			if(setting.val < driverInfo_P->limitJJJ_stage[1].startYMD[0])
			{
				setting.val = driverInfo_P->limitJJJ_stage[1].startYMD[0];
				driverInfo_P->limitJJJ_stage[1].endYMD[0] = setting.val;
				//ToDo: PLAY BOOM BOOM EFFECT, CANT BE SMALLER
			}
			else
				driverInfo_P->limitJJJ_stage[1].endYMD[0] = setting.val;
			break;		
		case SETTING_II_ED_MONT:		//need larger than start month
			if(setting.val == 0xFF)
				setting.val = 11;
			else if(setting.val > 11)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[1].endYMD[1] = setting.val;
			//every change calculate days of this 			
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[1].endYMD[1], \
				driverInfo_P->limitJJJ_stage[1].endYMD[0]);
			if(driverInfo_P->limitJJJ_stage[1].endYMD[2] > daysOfMonth)
				driverInfo_P->limitJJJ_stage[1].endYMD[2] = daysOfMonth;
			break;		
		case SETTING_II_ED_DAYS:
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[1].endYMD[1], \
				driverInfo_P->limitJJJ_stage[1].endYMD[0]);
			if(setting.val > daysOfMonth)		//
				setting.val = 1;
			else if(setting.val < 1)
				setting.val = daysOfMonth;		//
			driverInfo_P->limitJJJ_stage[1].endYMD[2] = setting.val;
			break;
		case SETTING_II_MON_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[1].limitNum[0] = setting.val;	
			break;
		case SETTING_II_MON_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[1].limitNum[1] = setting.val;	
			break;	
		case SETTING_II_TUE_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[1].limitNum[2] = setting.val;	
			break;	
		case SETTING_II_TUE_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[1].limitNum[3] = setting.val;	
			break;	
		case SETTING_II_WED_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[1].limitNum[4] = setting.val;	
			break;
		case SETTING_II_WED_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[1].limitNum[5] = setting.val;	
			break;
		case SETTING_II_THU_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[1].limitNum[6] = setting.val;	
			break;	
		case SETTING_II_THU_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[1].limitNum[7] = setting.val;	
			break;	
		case SETTING_II_FRI_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[1].limitNum[8] = setting.val;	
			break;	
		case SETTING_II_FRI_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[1].limitNum[9] = setting.val;	
			break;		
		case SETTING_III_ST_YEAR:
			if(setting.val > 99)
				setting.val = 22;
			else if(setting.val < 22)
				setting.val = 99;
			driverInfo_P->limitJJJ_stage[2].startYMD[0] = setting.val;				
			break;
		case SETTING_III_ST_MONT:
			if(setting.val == 0xFF)
				setting.val = 11;
			else if(setting.val > 11)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[2].startYMD[1] = setting.val;
			//every change calculate days of this 			
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[2].startYMD[1], \
				driverInfo_P->limitJJJ_stage[2].startYMD[0]);
			if(driverInfo_P->limitJJJ_stage[2].startYMD[2] > daysOfMonth)
				driverInfo_P->limitJJJ_stage[2].startYMD[2] = daysOfMonth;
			break;
		case SETTING_III_ST_DAYS:			
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[2].startYMD[1], \
				driverInfo_P->limitJJJ_stage[2].startYMD[0]);
			if(setting.val > daysOfMonth)		//
				setting.val = 1;
			else if(setting.val < 1)
				setting.val = daysOfMonth;		//
			driverInfo_P->limitJJJ_stage[2].startYMD[2] = setting.val;
			break;		
		case SETTING_III_ED_YEAR:		//need larger than start year
			if(setting.val > 99)
				setting.val = driverInfo_P->limitJJJ_stage[2].startYMD[0];
			else if(setting.val < 22)
				setting.val = 99;
			if(setting.val < driverInfo_P->limitJJJ_stage[2].startYMD[0])
			{
				setting.val = driverInfo_P->limitJJJ_stage[2].startYMD[0];
				driverInfo_P->limitJJJ_stage[2].endYMD[0] = setting.val;
				//ToDo: PLAY BOOM BOOM EFFECT, CANT BE SMALLER
			}
			else
				driverInfo_P->limitJJJ_stage[2].endYMD[0] = setting.val;
			break;		
		case SETTING_III_ED_MONT:		//need larger than start month
			if(setting.val == 0xFF)
				setting.val = 11;
			else if(setting.val > 11)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[2].endYMD[1] = setting.val;
			//every change calculate days of this 			
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[2].endYMD[1], \
				driverInfo_P->limitJJJ_stage[2].endYMD[0]);
			if(driverInfo_P->limitJJJ_stage[2].endYMD[2] > daysOfMonth)
				driverInfo_P->limitJJJ_stage[2].endYMD[2] = daysOfMonth;
			break;		
		case SETTING_III_ED_DAYS:
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[2].endYMD[1], \
				driverInfo_P->limitJJJ_stage[2].endYMD[0]);
			if(setting.val > daysOfMonth)		//
				setting.val = 1;
			else if(setting.val < 1)
				setting.val = daysOfMonth;		//
			driverInfo_P->limitJJJ_stage[2].endYMD[2] = setting.val;
			break;
		case SETTING_III_MON_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[2].limitNum[0] = setting.val;	
			break;
		case SETTING_III_MON_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[2].limitNum[1] = setting.val;	
			break;	
		case SETTING_III_TUE_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[2].limitNum[2] = setting.val;	
			break;	
		case SETTING_III_TUE_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[2].limitNum[3] = setting.val;	
			break;	
		case SETTING_III_WED_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[2].limitNum[4] = setting.val;	
			break;
		case SETTING_III_WED_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[2].limitNum[5] = setting.val;	
			break;
		case SETTING_III_THU_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[2].limitNum[6] = setting.val;	
			break;	
		case SETTING_III_THU_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[2].limitNum[7] = setting.val;	
			break;	
		case SETTING_III_FRI_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[2].limitNum[8] = setting.val;	
			break;	
		case SETTING_III_FRI_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[2].limitNum[9] = setting.val;	
			break;	
		case SETTING_IV_ST_YEAR:
			if(setting.val > 99)
				setting.val = 22;
			else if(setting.val < 22)
				setting.val = 99;
			driverInfo_P->limitJJJ_stage[3].startYMD[0] = setting.val;				
			break;
		case SETTING_IV_ST_MONT:
			if(setting.val == 0xFF)
				setting.val = 11;
			else if(setting.val > 11)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[3].startYMD[1] = setting.val;
			//every change calculate days of this 			
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[3].startYMD[1], \
				driverInfo_P->limitJJJ_stage[3].startYMD[0]);
			if(driverInfo_P->limitJJJ_stage[3].startYMD[2] > daysOfMonth)
				driverInfo_P->limitJJJ_stage[3].startYMD[2] = daysOfMonth;
			break;
		case SETTING_IV_ST_DAYS:			
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[3].startYMD[1], \
				driverInfo_P->limitJJJ_stage[3].startYMD[0]);
			if(setting.val > daysOfMonth)		//
				setting.val = 1;
			else if(setting.val < 1)
				setting.val = daysOfMonth;		//
			driverInfo_P->limitJJJ_stage[3].startYMD[2] = setting.val;
			break;		
		case SETTING_IV_ED_YEAR:		//need larger than start year
			if(setting.val > 99)
				setting.val = driverInfo_P->limitJJJ_stage[3].startYMD[0];
			else if(setting.val < 22)
				setting.val = 99;
			if(setting.val < driverInfo_P->limitJJJ_stage[3].startYMD[0])
			{
				setting.val = driverInfo_P->limitJJJ_stage[3].startYMD[0];
				driverInfo_P->limitJJJ_stage[3].endYMD[0] = setting.val;
				//ToDo: PLAY BOOM BOOM EFFECT, CANT BE SMALLER
			}
			else
				driverInfo_P->limitJJJ_stage[3].endYMD[0] = setting.val;
			break;		
		case SETTING_IV_ED_MONT:		//need larger than start month
			if(setting.val == 0xFF)
				setting.val = 11;
			else if(setting.val > 11)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[3].endYMD[1] = setting.val;
			//every change calculate days of this 			
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[3].endYMD[1], \
				driverInfo_P->limitJJJ_stage[3].endYMD[0]);
			if(driverInfo_P->limitJJJ_stage[3].endYMD[2] > daysOfMonth)
				driverInfo_P->limitJJJ_stage[3].endYMD[2] = daysOfMonth;
			break;		
		case SETTING_IV_ED_DAYS:
			daysOfMonth = time_monthDayCount((month_t)driverInfo_P->limitJJJ_stage[3].endYMD[1], \
				driverInfo_P->limitJJJ_stage[3].endYMD[0]);
			if(setting.val > daysOfMonth)		//
				setting.val = 1;
			else if(setting.val < 1)
				setting.val = daysOfMonth;		//
			driverInfo_P->limitJJJ_stage[3].endYMD[2] = setting.val;
			break;
		case SETTING_IV_MON_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[3].limitNum[0] = setting.val;	
			break;
		case SETTING_IV_MON_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[3].limitNum[1] = setting.val;	
			break;	
		case SETTING_IV_TUE_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[3].limitNum[2] = setting.val;	
			break;	
		case SETTING_IV_TUE_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[3].limitNum[3] = setting.val;	
			break;	
		case SETTING_IV_WED_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[3].limitNum[4] = setting.val;	
			break;
		case SETTING_IV_WED_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[3].limitNum[5] = setting.val;	
			break;
		case SETTING_IV_THU_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[3].limitNum[6] = setting.val;	
			break;	
		case SETTING_IV_THU_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[3].limitNum[7] = setting.val;	
			break;	
		case SETTING_IV_FRI_1:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[3].limitNum[8] = setting.val;	
			break;	
		case SETTING_IV_FRI_2:
			if(setting.val == 0xFF)
				setting.val = 9;
			if(setting.val > 9)
				setting.val = 0;
			driverInfo_P->limitJJJ_stage[3].limitNum[9] = setting.val;	
			break;																			
		default:
			break;
	}
}

static void checkDate(void)
{
	byte cnt, loop, ruleArr[5] = {2,1,0,4,3};	//五种限号规则循环
	static byte times = 0;

	for(loop = 0; loop < 4; loop++)
		for(cnt = 0; cnt < 3; cnt++)
		{
			if(driverInfo_P->limitJJJ_stage[loop].startYMD[cnt] == 0xFF || \
				driverInfo_P->limitJJJ_stage[loop].endYMD[cnt] == 0xFF)
				break;
			times++;
		}
	if(times < 12) 			//need default value
	{
		for(loop = 0; loop < 4; loop++)
		{			
			memcpy(driverInfo_P->limitJJJ_stage[loop].startYMD,\
			preDateSet[loop], sizeof(preDateSet[0]));//copy date preset
			memcpy(driverInfo_P->limitJJJ_stage[loop].limitNum,\
			presetMode[ruleArr[loop]], sizeof(presetMode[0]));//copy rule 
		}
	}
}
static void ruleMatch(void)
{
	if(loopDetails.matchTimes)
	{
		if(setting.val == 2 || setting.val == 7)				//使用匹配规则2
		{
			if(setting.now == SETTING_I_MON_1 || setting.now == SETTING_I_MON_2)//只有设置周一的数字会自动匹配规则
			{
				if(loopDetails.usedRule2 == false)	//匹配规则2没有使用过
				{
					loopDetails.usedRule2 = true;
					memcpy(driverInfo_P->limitJJJ_stage[0].limitNum, presetMode[1], 10);
					memcpy(driverInfo_P->limitJJJ_stage[1].limitNum, presetMode[0], 10);
					memcpy(driverInfo_P->limitJJJ_stage[2].limitNum, presetMode[4], 10);
					memcpy(driverInfo_P->limitJJJ_stage[3].limitNum, presetMode[3], 10);		
					loopDetails.matchTimes -= 1;
					if(setting.now == SETTING_I_MON_1 && setting.val == 7)	//如果在周一的第一个数位置选择了规则2，把周一的两个号码交换位置
						switchTwoNum(&driverInfo_P->limitJJJ_stage[0].limitNum[0],\
							&driverInfo_P->limitJJJ_stage[0].limitNum[1]);
				}	
			}		
		}
		else if(setting.val == 3 || setting.val == 8)														//使用匹配规则3
		{
			if(setting.now == SETTING_I_MON_1 || setting.now == SETTING_I_MON_2)//只有设置周一的数字会自动匹配规则
			{
				if(loopDetails.usedRule3 == false)			//匹配规则3没有使用过
				{
					loopDetails.usedRule3 = true;				
					memcpy(driverInfo_P->limitJJJ_stage[0].limitNum, presetMode[2], 10);
					memcpy(driverInfo_P->limitJJJ_stage[1].limitNum, presetMode[1], 10);
					memcpy(driverInfo_P->limitJJJ_stage[2].limitNum, presetMode[0], 10);
					memcpy(driverInfo_P->limitJJJ_stage[3].limitNum, presetMode[4], 10);	
					loopDetails.matchTimes -= 1;	
					if(setting.now == SETTING_I_MON_1)						//如果在周一的第一个数位置选择了规则3，把周一的两个号码交换位置
						switchTwoNum(&driverInfo_P->limitJJJ_stage[0].limitNum[0],\
							&driverInfo_P->limitJJJ_stage[0].limitNum[1]);
				}			
			}
		}
		else if(setting.val == 4 || setting.val == 9)														//使用匹配规则3
		{
			if(setting.now == SETTING_I_MON_1 || setting.now == SETTING_I_MON_2)//只有设置周一的数字会自动匹配规则
			{
				if(loopDetails.usedRule4 == false)			//匹配规则4没有使用过
				{
					loopDetails.usedRule4 = true;				
					memcpy(driverInfo_P->limitJJJ_stage[0].limitNum, presetMode[3], 10);
					memcpy(driverInfo_P->limitJJJ_stage[1].limitNum, presetMode[2], 10);
					memcpy(driverInfo_P->limitJJJ_stage[2].limitNum, presetMode[1], 10);
					memcpy(driverInfo_P->limitJJJ_stage[3].limitNum, presetMode[0], 10);	
					loopDetails.matchTimes -= 1;	
					if(setting.now == SETTING_I_MON_1)						//如果在周一的第一个数位置选择了规则3，把周一的两个号码交换位置
						switchTwoNum(&driverInfo_P->limitJJJ_stage[0].limitNum[0],\
							&driverInfo_P->limitJJJ_stage[0].limitNum[1]);
				}			
			}
		}
		else if(setting.val == 5 || setting.val == 0)														//使用匹配规则3
		{
			if(setting.now == SETTING_I_MON_1 || setting.now == SETTING_I_MON_2)//只有设置周一的数字会自动匹配规则
			{
				if(loopDetails.usedRule5 == false)			//匹配规则5没有使用过
				{
					loopDetails.usedRule5 = true;				
					memcpy(driverInfo_P->limitJJJ_stage[0].limitNum, presetMode[4], 10);
					memcpy(driverInfo_P->limitJJJ_stage[1].limitNum, presetMode[3], 10);
					memcpy(driverInfo_P->limitJJJ_stage[2].limitNum, presetMode[2], 10);
					memcpy(driverInfo_P->limitJJJ_stage[3].limitNum, presetMode[1], 10);		
					loopDetails.matchTimes -= 1;	
					if(setting.now == SETTING_I_MON_1)						//如果在周一的第一个数位置选择了规则3，把周一的两个号码交换位置
						switchTwoNum(&driverInfo_P->limitJJJ_stage[0].limitNum[0],\
							&driverInfo_P->limitJJJ_stage[0].limitNum[1]);
				}			
			}
		}
		else if(setting.val == 1 || setting.val == 6)
		{
			if(setting.now == SETTING_I_MON_1 || setting.now == SETTING_I_MON_2)//只有设置周一的数字会自动匹配规则
			{
				if(loopDetails.usedRule1 == false)			//匹配规则1没有使用过
				{
					loopDetails.usedRule1 = true;				
					memcpy(driverInfo_P->limitJJJ_stage[0].limitNum, presetMode[0], 10);
					memcpy(driverInfo_P->limitJJJ_stage[1].limitNum, presetMode[4], 10);
					memcpy(driverInfo_P->limitJJJ_stage[2].limitNum, presetMode[3], 10);
					memcpy(driverInfo_P->limitJJJ_stage[3].limitNum, presetMode[2], 10);		
					loopDetails.matchTimes -= 1;	
					if(setting.now == SETTING_I_MON_1 && setting.val == 6)						//如果在周一的第一个数位置选择了规则1，把周一的两个号码交换位置
						switchTwoNum(&driverInfo_P->limitCouple.limitRules[0], &driverInfo_P->limitCouple.limitRules[1]);
				}			
			}
		}
	}	
}




