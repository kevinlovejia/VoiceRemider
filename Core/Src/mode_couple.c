/*
 * Project: Voice Reminder
 * Author: Wei Lee, kevin_jia@foxmail.com
 * Copyright: (C) 2022 by Wei
 * License: GNU GPL v3 (see License.txt)
 */
#include "common.h"
#include "main.h"

#define OPTION_COUNT	10
#define SETTING_NOW_NONE		0
#define SETTING_NOW_MON_1		1
#define SETTING_NOW_MON_2		2
#define SETTING_NOW_TUE_1		3
#define SETTING_NOW_TUE_2		4
#define SETTING_NOW_WED_1		5
#define SETTING_NOW_WED_2		6
#define SETTING_NOW_THU_1		7
#define SETTING_NOW_THU_2		8
#define SETTING_NOW_FRI_1		9
#define SETTING_NOW_FRI_2		10

#define WEEKNUM_X1					48//6+16+16+8+2
#define WEEKNUM_X2					79//6+16+16+8+11+16+2+4

//static driverInfo_s driverInfo;

static prev_menu_s prevMenuData;
limCoupLocal_s coupleDetails;
static void mSelect(void);
static void itemLoader(byte num);
static display_t thisdraw(void);
static void mDown(void);
static void mUp(void);
static void showWeekStr(byte num);
static void selectMondayNum(void);
static void selectTuesdayNum(void);
static void selectWednesdayNum(void);
static void selectThursdayNum(void);
static void selectFridayNum(void);
static void ruleAutoMatch(void);
void switchTwoNum(byte *one, byte *two);

menu_f weekDrawFunc[] = {selectMondayNum, selectTuesdayNum, \
 selectWednesdayNum, selectThursdayNum, selectFridayNum};

byte presetMode[][10] ={
	//???	???	????	????	????
	{
		1,6,	2,7,	3,8,	4,9,	5,0,	//[0]?????1
	},
	{
		2,7,	3,8,	4,9,	5,0,	1,6,	//[1]?????2
	},
	{
		3,8,	4,9,	5,0,	1,6,	2,7,
	},
	{
		4,9,	5,0,	1,6,	2,7,	3,8,
	},
	{
		5,0,	1,6,	2,7,	3,8,	4,9,
	},
	{
		1,9,	2,8,	3,7,	4,6,	5,0,	//[5]?????3
	},
 };

void modeCoupleSelect()
{
//	setMenuInfo(OPTION_COUNT, MENU_TYPE_ICON, PSTR(STR_MODEMENU));
//	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);
	setMenuInfo(OPTION_COUNT, MENU_TYPE_STR, PSTR(STR_LIMITCOUPLE));
	setMenuFuncs(mDown, mSelect, mUp, itemLoader);
//	setMenuOptions();
	//menuData.func.draw = thisdraw; //????????????????????????????????????м?????е????

	setPrevMenuOpen(&prevMenuData, modeCoupleSelect);	

	//1.从flash读取当前的限号规则是多少
//	static unsigned int sizeStruct = 0;
//	// static driverInfo_s inFlash;
//	sizeStruct = sizeof(driverInfo);
	readFlash(START_FLASH_ADDRESS, (uint16_t *)driverInfo_P, sizeof(driverInfo_s));
	//2.?????123??????????presetMode???????????
	//driverInfo_P->limitCouple.rule = 1;							//????????1,????????????
	coupleDetails.matchTimes = 3;									//???????????????????????????
	coupleDetails.usedRule1 = false;
	coupleDetails.usedRule2 = false;
	coupleDetails.usedRule3 = false;

	if(driverInfo_P->limitCouple.rule == 0 || driverInfo_P->limitCouple.rule > 4) //遇到出错或者没有选择任何规则
	{
		memcpy(driverInfo_P->limitCouple.limitRules, presetMode[0], sizeof(driverInfo_P->limitCouple.limitRules));
		coupleDetails.usedRule1 = 1;
	}
	menuData.selected = 1;
	
	beginAnimation2(NULL);
}

static void mDown()
{
	nextOption();

	// Some lines are blank, skip them
	if(menuData.selected == 0 || menuData.selected == 2 || menuData.selected == 4 || menuData.selected == 6\
		 || menuData.selected == 8)
		nextOption();
}

static void mUp()
{
	prevOption();

	// Some lines are blank, skip them
	if(menuData.selected == 0 || menuData.selected == 2 || menuData.selected == 4 || menuData.selected == 6\
		 || menuData.selected == 8)
		prevOption();
}

static void mSelect()
{
	bool isExiting = exitSelected();
	if(isExiting)
	{
		//save rules to flash	
		unsigned int sizeStruct = 0, ret = 99;
		sizeStruct = sizeof(driverInfo_s);			
		driverInfo_s *flashCont;
		flashCont = (driverInfo_s *)malloc(sizeof(driverInfo_s));
		if(flashCont == NULL)
			while(1);//Todo: 
		readFlash(START_FLASH_ADDRESS, (uint16_t *)flashCont, sizeStruct);
		if(flashCont->limitCouple.rule != driverInfo_P->limitCouple.rule)
			wFlash(START_FLASH_ADDRESS, (uint16_t *)driverInfo_P, sizeStruct);
		else if(flashCont->limitCouple.rule == 4)
		{
			ret = memcmp(flashCont->limitCouple.limitRules, driverInfo_P->limitCouple.limitRules, \
			sizeof(flashCont->limitCouple.limitRules));
			if(ret != 0)
				wFlash(START_FLASH_ADDRESS, (uint16_t *)driverInfo_P, sizeStruct);
		}
		free(flashCont);
		flashCont = NULL;
	}

	setPrevMenuExit(&prevMenuData);
	doAction(exitSelected());	
}

static void itemLoader(byte num)
{
	UNUSED(num);
	showWeekStr(num);
	addBackOption();
}

static display_t weekNumDraw()
{
	byte x = WEEKNUM_X1;//79
	byte y = 8;
	byte w = 11;
	byte scrollY = menuData.scroll * 8;

	switch(setting.now)
	{
		case SETTING_NOW_MON_1:	
			x = WEEKNUM_X1;			
			y = 8 - scrollY;	
			break;
		case SETTING_NOW_MON_2:
			x = WEEKNUM_X2;
			y = 8 - scrollY;
			break;
		case SETTING_NOW_TUE_1:
			x = WEEKNUM_X1;
			y = 24 - scrollY;
			break;		
		case SETTING_NOW_TUE_2:
			x = WEEKNUM_X2;
			y = 24 - scrollY;
			break;
		case SETTING_NOW_WED_1:
			x = WEEKNUM_X1;
			y = 40 - scrollY;
			break;		
		case SETTING_NOW_WED_2:
			x = WEEKNUM_X2;
			y = 40 - scrollY;
			break;
		case SETTING_NOW_THU_1:
			x = WEEKNUM_X1;
			y = 56 - scrollY;
			break;		
		case SETTING_NOW_THU_2:
			x = WEEKNUM_X2;
			y = 56 - scrollY;
			break;
		case SETTING_NOW_FRI_1:
			x = WEEKNUM_X1;
			y = 72 - scrollY;
			break;		
		case SETTING_NOW_FRI_2:
			x = WEEKNUM_X2;
			y = 72 - scrollY;
			break;
		default:
			break;
	}
	
	draw_clearArea(x, y, w);
	draw_clearArea(x, y+8, w);

	draw_bitmap(x, y, numFont11x16[setting.val] , 11, 16, INVERT, 0);// 11*16

	return DISPLAY_DONE;
}

static void showWeekStr(byte num)
{
	char buff[14] = {0};
	byte posXArray[8] = {6,6+16,6+16+16,6+16+16+8+2,6+16+16+8+11+2+2,\
	6+16+16+8+11+16+2+4,6+16+16+8+11+16+11+2+4+2,6+16+16+8+11+16+11+16+2+4+2};
	byte posY = 0;
	byte wordWidth = 0;
	const byte* bitmp;
	byte weekCalc = 0;
	byte fontValue =  99;
		
	if(num%2 == 0 && num <= 10)							//scroll??????????С????10?????????
		return;
	else
		weekCalc = num / 2;										//scroll??????????????13579?????01234

	posY = weekCalc*16 + 8;
	posY -= menuData.scroll*8;
	if(posY == 0)														//???y?????0??????α???
		draw_clearArea(35, 0, 56);						//???< MODE >	
	
	LOOP(8, cnt)
	{			
		switch(cnt)
		{
			case 0://周
				wordWidth = 16;
				bitmp = textDisplay[8];
				break;
			case 1://一
				wordWidth = 16;
				bitmp = textDisplay[9+weekCalc];
				break;
			case 2://:
				wordWidth = 8;
				bitmp = numFont11x16[10];	
				break;
			case 3://1
				wordWidth = 11;
				fontValue = driverInfo_P->limitCouple.limitRules[weekCalc*2];
				bitmp = numFont11x16[fontValue];
				break;
			case 4://和
				wordWidth = 16;
				bitmp = textDisplay[24];
				break;
			case 5://6
				wordWidth = 11;
				fontValue = driverInfo_P->limitCouple.limitRules[weekCalc*2+1];
				bitmp = numFont11x16[fontValue];
				break;
			case 6://限
				wordWidth = 16;
				bitmp = textDisplay[2];
				break;
			case 7://行
				wordWidth = 16;
				bitmp = textDisplay[3];
				break;
			default:
				break;
		}
		draw_bitmap(posXArray[cnt], posY, bitmp, wordWidth, 16, NOINVERT, 0);
	}	
	setMenuOption(1+weekCalc*2, buff, NULL, weekDrawFunc[weekCalc]);
}

static void weekNumDataUp()
{
	setting.val++;
	if(setting.val > 9)
		setting.val = 0;
	ruleAutoMatch();
	driverInfo_P->limitCouple.limitRules[setting.now-1] = setting.val;			//?????????????????????????
}

static void weekNumDataDown()
{
	setting.val--;
		
	byte max = 9;
	if(setting.val > max)																		// Overflow
		setting.val = max;
	ruleAutoMatch();
	driverInfo_P->limitCouple.limitRules[setting.now-1] = setting.val;			//?????????????????????????
}

static void beginSelect()
{
	setMenuFuncs(weekNumDataUp, mSelect, weekNumDataDown, itemLoader);
	menuData.func.draw = weekNumDraw;
}

static void endSelect()
{
	setting.now = SETTING_NOW_NONE;
	setMenuFuncs(mDown, mSelect, mUp, itemLoader);
	//setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);
	//在保存完当前参数后，再调用它，现在使用不会略过没有选项的空半行
	menuData.func.draw = NULL;
}

static void selectMondayNum()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_NOW_MON_1;
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_MON_1-1];
			break;
		case SETTING_NOW_MON_1:
			setting.now = SETTING_NOW_MON_2;
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_MON_2-1];			
			break;
		case SETTING_NOW_MON_2:
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_MON_2-1];		
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}	
}
static void selectTuesdayNum()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_NOW_TUE_1;
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_TUE_1-1];
			break;
		case SETTING_NOW_TUE_1:
			setting.now = SETTING_NOW_TUE_2;
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_TUE_2-1];
			break;
		case SETTING_NOW_TUE_2:
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_TUE_2-1];
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}	
}

static void selectWednesdayNum()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_NOW_WED_1;
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_WED_1-1];
			break;
		case SETTING_NOW_WED_1:
			setting.now = SETTING_NOW_WED_2;
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_WED_2-1];
			break;
		case SETTING_NOW_WED_2:
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_WED_2-1];
			endSelect();
			break;
		default:
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}	
}
static void selectThursdayNum()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_NOW_THU_1;
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_THU_1-1];
			break;
		case SETTING_NOW_THU_1:
			setting.now = SETTING_NOW_THU_2;
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_THU_2-1];
			break;
		case SETTING_NOW_THU_2:
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_THU_2-1];
			endSelect();
			break;
		default: 
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}	
}

static void selectFridayNum()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_NOW_FRI_1;
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_FRI_1-1];
			break;
		case SETTING_NOW_FRI_1:
			setting.now = SETTING_NOW_FRI_2;
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_FRI_2-1];
			break;
		case SETTING_NOW_FRI_2:
			setting.val = driverInfo_P->limitCouple.limitRules[SETTING_NOW_FRI_2-1];
			endSelect();
			break;
		default: 
			setting.now = SETTING_NOW_NONE;
			endSelect();
			break;
	}	
}

static void ruleAutoMatch(void)
{
	if(coupleDetails.matchTimes)
	{
		if(setting.val == 2 || setting.val == 7)				//使用匹配规则2
		{
			if(setting.now == SETTING_NOW_MON_1 || setting.now == SETTING_NOW_MON_2)//只有设置周一的数字会自动匹配规则
			{
				if(coupleDetails.usedRule2 == false)	//匹配规则2没有使用过
				{
					coupleDetails.usedRule2 = true;
					driverInfo_P->limitCouple.rule = 2;						//使用预设模式2
					memcpy(driverInfo_P->limitCouple.limitRules, presetMode[1], sizeof(driverInfo_P->limitCouple.limitRules));		//使用预设模式2
					coupleDetails.matchTimes -= 1;
					if(setting.now == SETTING_NOW_MON_1 && setting.val == 7)	//如果在周一的第一个数位置选择了规则2，把周一的两个号码交换位置
						switchTwoNum(&driverInfo_P->limitCouple.limitRules[0], &driverInfo_P->limitCouple.limitRules[1]);
				}	
			}		
		}
		else if(setting.val == 9)														//使用匹配规则3
		{
			if(setting.now == SETTING_NOW_MON_1 || setting.now == SETTING_NOW_MON_2)//只有设置周一的数字会自动匹配规则
			{
				if(coupleDetails.usedRule3 == false)			//匹配规则3没有使用过
				{
					coupleDetails.usedRule3 = true;				
					driverInfo_P->limitCouple.rule = 3;								//使用预设模式3
					memcpy(driverInfo_P->limitCouple.limitRules, presetMode[5], sizeof(driverInfo_P->limitCouple.limitRules));		//使用预设模式3
					coupleDetails.matchTimes -= 1;	
					if(setting.now == SETTING_NOW_MON_1)						//如果在周一的第一个数位置选择了规则3，把周一的两个号码交换位置
						switchTwoNum(&driverInfo_P->limitCouple.limitRules[0], &driverInfo_P->limitCouple.limitRules[1]);
				}			
			}
		}
		else if(setting.val == 1 || setting.val == 6)
		{
			if(setting.now == SETTING_NOW_MON_1 || setting.now == SETTING_NOW_MON_2)//只有设置周一的数字会自动匹配规则
			{
				if(coupleDetails.usedRule1 == false)			//匹配规则1没有使用过
				{
					coupleDetails.usedRule1 = true;				
					driverInfo_P->limitCouple.rule = 1;								//使用预设模式1
					memcpy(driverInfo_P->limitCouple.limitRules, presetMode[0], sizeof(driverInfo_P->limitCouple.limitRules));		//使用预设模式3
					coupleDetails.matchTimes -= 1;	
					if(setting.now == SETTING_NOW_MON_1 && setting.val == 6)						//如果在周一的第一个数位置选择了规则1，把周一的两个号码交换位置
						switchTwoNum(&driverInfo_P->limitCouple.limitRules[0], &driverInfo_P->limitCouple.limitRules[1]);
				}			
			}
		}
		else//当前的值为0,3,4,5,8时
			driverInfo_P->limitCouple.rule = 4;									//使用自定义模式4			
	}
	else
		driverInfo_P->limitCouple.rule = 4;										//使用自定义模式4
}

void switchTwoNum(byte *one, byte *two)
{
	byte temp;
	temp = *one;
	*one = *two;
	*two = temp;
}





