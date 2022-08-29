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

static prev_menu_s prevMenuData;
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
	//��һ	�ܶ�	����	����	����
	{
		1,6,	2,7,	3,8,	4,9,	5,0,	//[0]Ԥ��ģʽ1
	},
	{
		2,7,	3,8,	4,9,	5,0,	1,6,	//[1]Ԥ��ģʽ2
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
		1,9,	2,8,	3,7,	4,6,	5,0,	//[5]Ԥ��ģʽ3
	},
 };

void modeCoupleSelect()
{
//	setMenuInfo(OPTION_COUNT, MENU_TYPE_ICON, PSTR(STR_MODEMENU));
//	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);
	setMenuInfo(OPTION_COUNT, MENU_TYPE_STR, PSTR(STR_LIMITCOUPLE));
	setMenuFuncs(mDown, mSelect, mUp, itemLoader);
//	setMenuOptions();
	//menuData.func.draw = thisdraw; //�󶨲˵���ͼ��������֪���˳�ʱ�����������û�м���ִ�е�ԭ��

	setPrevMenuOpen(&prevMenuData, modeCoupleSelect);	

	//1.��flash��ȡ��ǰ���޺Ź����Ƕ���
	static unsigned int sizeStruct = 0;
	static driverInfo_s inFlash;
	byte ruleInFlash = 0;
	sizeStruct = sizeof(driverInfo);
	readFlash(START_FLASH_ADDRESS, (uint16_t *)&inFlash, sizeStruct);
	//2.�����123������ֱ�Ӵ�presetMode�ж�ȡĬ�Ϲ���
	driverInfo.limitCouple.rule = 1;							//ʹ��Ԥ��ģʽ1,�Է����ݴ洢ʧ��
	driverInfo.limitCouple.matchTimes = 3;									//ÿ�ν�����棬����һ���Զ�ƥ�����
	if(inFlash.limitCouple.rule > 0 && inFlash.limitCouple.rule < 4)
	{
		if(inFlash.limitCouple.rule == 3)
			ruleInFlash = 5;
		else 
			ruleInFlash = inFlash.limitCouple.rule-1;
		memcpy(driverInfo.limitCouple.limitRules, presetMode[ruleInFlash], sizeof(driverInfo.limitCouple.limitRules));
		driverInfo.limitCouple.matchTimes -= 1;				//�Զ�ƥ�������1
		driverInfo.limitCouple.rule = inFlash.limitCouple.rule;
		if(driverInfo.limitCouple.rule == 1)
		{
			driverInfo.limitCouple.usedRule1 = true;		//��ʼ��ƥ�����2��3�ı�־λ
			driverInfo.limitCouple.usedRule2 = false;
			driverInfo.limitCouple.usedRule3 = false;
		}
		else if(driverInfo.limitCouple.rule == 2)
		{
			driverInfo.limitCouple.usedRule1 = false;
			driverInfo.limitCouple.usedRule2 = true;		//��ʼ��ƥ�����2��3�ı�־λ
			driverInfo.limitCouple.usedRule3 = false;
		}
		else if(driverInfo.limitCouple.rule == 3)
		{
			driverInfo.limitCouple.usedRule1 = false;
			driverInfo.limitCouple.usedRule2 = false;
			driverInfo.limitCouple.usedRule3 = true;
		}
	}
	else	//3.�����4���򣬴�flash��ȡ
	{
		memcpy(driverInfo.limitCouple.limitRules, inFlash.limitCouple.limitRules, sizeof(driverInfo.limitCouple.limitRules));
		driverInfo.limitCouple.usedRule1 = false;
		driverInfo.limitCouple.usedRule2 = false;
		driverInfo.limitCouple.usedRule3 = false;
		driverInfo.limitCouple.rule = 4;
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
		static unsigned int sizeStruct = 0;
		sizeStruct = sizeof(driverInfo);
		writeFlash(START_FLASH_ADDRESS, (uint16_t *)&driverInfo, sizeStruct);
	}
	
//		appconfig_save();
//	
//	setMenuFuncs(mUp, mSelect, mDown, itemLoader);

	setPrevMenuExit(&prevMenuData);
	doAction(exitSelected());
	
}

static void itemLoader(byte num)
{
	UNUSED(num);
	showWeekStr(num);
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
		
	if(num%2 == 0 && num <= 10)							//scrollΪż��������С����10����Ҫ����
		return;
	else
		weekCalc = num / 2;										//scrollΪ����ʱ��������13579�����01234

	posY = weekCalc*16 + 8;
	posY -= menuData.scroll*8;
	if(posY == 0)														//��ǰy����Ϊ0ʱ���һ�α���
		draw_clearArea(35, 0, 56);						//���< MODE >	
	
	LOOP(8, cnt)
	{			
		switch(cnt)
		{
			case 0://��
				wordWidth = 16;
				bitmp = textDisplay[8];
				break;
			case 1://һ
				wordWidth = 16;
				bitmp = textDisplay[9+weekCalc];
				break;
			case 2://:
				wordWidth = 8;
				bitmp = numFont11x16[10];	
				break;
			case 3://1
				wordWidth = 11;
				fontValue = driverInfo.limitCouple.limitRules[weekCalc*2];
				bitmp = numFont11x16[fontValue];
				break;
			case 4://��
				wordWidth = 16;
				bitmp = textDisplay[24];
				break;
			case 5://6
				wordWidth = 11;
				fontValue = driverInfo.limitCouple.limitRules[weekCalc*2+1];
				bitmp = numFont11x16[fontValue];
				break;
			case 6://��
				wordWidth = 16;
				bitmp = textDisplay[2];
				break;
			case 7://��
				wordWidth = 16;
				bitmp = textDisplay[3];
				break;
			default:
				break;
		}
//		if(posY == 0)	//��ǰ������ס����
//			draw_bitmap(posXArray[cnt], 8, bitmp+wordWidth, wordWidth, 8, NOINVERT, 0);//ֻ��ʾ���ֵ��°벿��
//		else	
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
	driverInfo.limitCouple.limitRules[setting.now-1] = setting.val;			//�ѵ�ǰ�������ֵ�����û��ṹ����
}

static void weekNumDataDown()
{
	setting.val--;
		
	byte max = 9;
	if(setting.val > max)																		// Overflow
		setting.val = max;
	ruleAutoMatch();
	driverInfo.limitCouple.limitRules[setting.now-1] = setting.val;			//�ѵ�ǰ�������ֵ�����û��ṹ����
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
	//�ڱ����굱ǰ�������ٵ�����������ʹ�ò����Թ�û��ѡ��Ŀհ���
	menuData.func.draw = NULL;
}

static void selectMondayNum()
{
	beginSelect();	
	
	switch(setting.now)
	{
		case SETTING_NOW_NONE:
			setting.now = SETTING_NOW_MON_1;
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_MON_1-1];
			break;
		case SETTING_NOW_MON_1:
			setting.now = SETTING_NOW_MON_2;
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_MON_2-1];			
			break;
		case SETTING_NOW_MON_2:
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_MON_2-1];		
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
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_TUE_1-1];
			break;
		case SETTING_NOW_TUE_1:
			setting.now = SETTING_NOW_TUE_2;
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_TUE_2-1];
			break;
		case SETTING_NOW_TUE_2:
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_TUE_2-1];
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
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_WED_1-1];
			break;
		case SETTING_NOW_WED_1:
			setting.now = SETTING_NOW_WED_2;
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_WED_2-1];
			break;
		case SETTING_NOW_WED_2:
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_WED_2-1];
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
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_THU_1-1];
			break;
		case SETTING_NOW_THU_1:
			setting.now = SETTING_NOW_THU_2;
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_THU_2-1];
			break;
		case SETTING_NOW_THU_2:
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_THU_2-1];
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
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_FRI_1-1];
			break;
		case SETTING_NOW_FRI_1:
			setting.now = SETTING_NOW_FRI_2;
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_FRI_2-1];
			break;
		case SETTING_NOW_FRI_2:
			setting.val = driverInfo.limitCouple.limitRules[SETTING_NOW_FRI_2-1];
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
	if(driverInfo.limitCouple.matchTimes)
	{
		if(setting.val == 2 || setting.val == 7)				//ʹ��ƥ�����2
		{
			if(setting.now == SETTING_NOW_MON_1 || setting.now == SETTING_NOW_MON_2)//ֻ��������һ�����ֻ��Զ�ƥ�����
			{
				if(driverInfo.limitCouple.usedRule2 == false)	//ƥ�����2û��ʹ�ù�
				{
					driverInfo.limitCouple.usedRule2 = true;
					driverInfo.limitCouple.rule = 2;						//ʹ��Ԥ��ģʽ2
					memcpy(driverInfo.limitCouple.limitRules, presetMode[1], sizeof(driverInfo.limitCouple.limitRules));		//ʹ��Ԥ��ģʽ2
					driverInfo.limitCouple.matchTimes -= 1;
					if(setting.now == SETTING_NOW_MON_1 && setting.val == 7)	//�������һ�ĵ�һ����λ��ѡ���˹���2������һ���������뽻��λ��
						switchTwoNum(&driverInfo.limitCouple.limitRules[0], &driverInfo.limitCouple.limitRules[1]);
				}	
			}		
		}
		else if(setting.val == 9)														//ʹ��ƥ�����3
		{
			if(setting.now == SETTING_NOW_MON_1 || setting.now == SETTING_NOW_MON_2)//ֻ��������һ�����ֻ��Զ�ƥ�����
			{
				if(driverInfo.limitCouple.usedRule3 == false)			//ƥ�����3û��ʹ�ù�
				{
					driverInfo.limitCouple.usedRule3 = true;				
					driverInfo.limitCouple.rule = 3;								//ʹ��Ԥ��ģʽ3
					memcpy(driverInfo.limitCouple.limitRules, presetMode[5], sizeof(driverInfo.limitCouple.limitRules));		//ʹ��Ԥ��ģʽ3
					driverInfo.limitCouple.matchTimes -= 1;	
					if(setting.now == SETTING_NOW_MON_1)						//�������һ�ĵ�һ����λ��ѡ���˹���3������һ���������뽻��λ��
						switchTwoNum(&driverInfo.limitCouple.limitRules[0], &driverInfo.limitCouple.limitRules[1]);
				}			
			}
		}
		else if(setting.val == 1 || setting.val == 6)
		{
			if(setting.now == SETTING_NOW_MON_1 || setting.now == SETTING_NOW_MON_2)//ֻ��������һ�����ֻ��Զ�ƥ�����
			{
				if(driverInfo.limitCouple.usedRule1 == false)			//ƥ�����1û��ʹ�ù�
				{
					driverInfo.limitCouple.usedRule1 = true;				
					driverInfo.limitCouple.rule = 1;								//ʹ��Ԥ��ģʽ1
					memcpy(driverInfo.limitCouple.limitRules, presetMode[0], sizeof(driverInfo.limitCouple.limitRules));		//ʹ��Ԥ��ģʽ3
					driverInfo.limitCouple.matchTimes -= 1;	
					if(setting.now == SETTING_NOW_MON_1 && setting.val == 6)						//�������һ�ĵ�һ����λ��ѡ���˹���1������һ���������뽻��λ��
						switchTwoNum(&driverInfo.limitCouple.limitRules[0], &driverInfo.limitCouple.limitRules[1]);
				}			
			}
		}
		else//��ǰ��ֵΪ0,3,4,5,8ʱ
			driverInfo.limitCouple.rule = 4;									//ʹ���Զ���ģʽ4			
	}
	else
		driverInfo.limitCouple.rule = 4;										//ʹ���Զ���ģʽ4
}

void switchTwoNum(byte *one, byte *two)
{
	byte temp;
	temp = *one;
	*one = *two;
	*two = temp;
}





