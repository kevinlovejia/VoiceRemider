/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"

#define OPTION_COUNT getItemCount()

static prev_menu_s prevMenuData;

static void mOpen(void);
static void mSelect(void);
static void itemLoader(byte);

static uint8_t getItemCount()
{
  uint8_t cnt = 4;
#if COMPILE_GAME1 || COMPILE_GAME2 || COMPILE_GAME3
  ++cnt;
#endif
#if COMPILE_STOPWATCH
  ++cnt;
#endif
#if COMPILE_TORCH
  ++cnt;
#endif
#if COMPILE_BTRCCAR
  ++cnt;
#endif
#if COMPILE_TUNEMAKER
  ++cnt;
#endif	
  return cnt;
}

void mMainOpen()
{
	buttons_setFuncs(NULL, menu_select, NULL);
	beginAnimation(mOpen);  //�رն�����ʼ����ִ�д����˵�
}

//�����˵�
static void mOpen()
{
	display_setDrawFunc(menu_draw);  //�󶨻��ƺ���Ϊmenu_draw

	buttons_setFuncs(menu_up, menu_select, menu_down);  //�󶨰������ܺ���

 	setMenuInfo(OPTION_COUNT, MENU_TYPE_ICON, PSTR(STR_MAINMENU));   //��ȡ��ǰ�˵���Ϣ��ѡ��������˵����������ֻ���ͼ�꣩
	setMenuFuncs(MENUFUNC_NEXT, mSelect, MENUFUNC_PREV, itemLoader);  //�󶨲˵��ĺ���,��ǰ������ѡ��ȷ��

	setPrevMenuOpen(&prevMenuData, mOpen);  //�����ϼ��˵�

	beginAnimation2(NULL);      //�������ɶ���
	
}

static void mSelect()
{
	setPrevMenuExit(&prevMenuData);  //������һ�β˵���ѡ��
	doAction(true);
}

static void itemLoader(byte num)
{
	num = 0;
	//setMenuOption_P(num++, PSTR(STR_ALARMS), menu_alarm, mAlarmsOpen);
#if COMPILE_TORCH
	setMenuOption_P(num++, PSTR(STR_FLASHLIGHT), menu_torch, torch_open);
#endif
#if COMPILE_STOPWATCH
	setMenuOption_P(num++, PSTR(STR_STOPWATCH), menu_stopwatch, stopwatch_open);
#endif
#if COMPILE_BTRCCAR
	setMenuOption_P(num++, PSTR(STR_BTRCCAR), menu_stopwatch, btrccar_open);
#endif
#if COMPILE_TUNEMAKER
	setMenuOption_P(num++, PSTR(STR_TUNEMAKER), menu_tunemaker, tunemakerOpen);
#endif	
#if COMPILE_GAME1 || COMPILE_GAME2 || COMPILE_GAME3
	setMenuOption_P(num++, PSTR(STR_GAMES), menu_games, mGamesOpen);
#endif
	//setMenuOption_P(num++, PSTR(STR_CALCULATORS), menu_calc, calcOpen);
	//setMenuOption_P(num++, PSTR(STR_SETTINGS), menu_settings, mSettingsOpen);

	setMenuOption_P(num++, PSTR(STR_TAILNO), menu_tailnum[0], tailnum_select); 	//β��ѡ��
	setMenuOption_P(num++, PSTR(STR_MODE), menu_mode, mode_select);    					//ģʽ����
	setMenuOption_P(num++, PSTR(STR_TIMEDATE), menu_timedate, mTimeDateOpen);	
	setMenuOption_P(num++, PSTR(STR_SOUND), menu_sound, mSoundOpen);
	setMenuOption_P(OPTION_COUNT, PSTR(STR_EXIT), menu_exit, menu_close);
}
