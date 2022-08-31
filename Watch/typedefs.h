/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdbool.h>
#include <stdint.h>

 #if defined ( __CC_ARM   )
    #pragma anon_unions
 #endif

typedef uint8_t byte;
typedef uint16_t uint;
typedef uint32_t ulong;

extern byte oledBuffer[];


typedef unsigned int millis8_t;
typedef unsigned int  millis_t;

// bool from stdbool does extra stuff to make the value
// always either 0 or 1, which isn't needed most of the time.
// So here's BOOL which doesn't do that.
typedef	uint8_t BOOL;

typedef uint32_t timestamp_t;

typedef enum
{
	DISPLAY_DONE,
	DISPLAY_BUSY,
//	DISPLAY_TOOFAST
} display_t;

typedef enum
{
	MENU_TYPE_STR,
	MENU_TYPE_ICON
} menu_type_t;

typedef enum
{
	MONTH_JAN = 0,
	MONTH_FEB = 1,
	MONTH_MAR = 2,
	MONTH_APR = 3,
	MONTH_MAY = 4,
	MONTH_JUN = 5,
	MONTH_JUL = 6,
	MONTH_AUG = 7,
	MONTH_SEP = 8,
	MONTH_OCT = 9,
	MONTH_NOV = 10,
	MONTH_DEC = 11
} month_t;

typedef enum
{
	DAY_MON = 0,
	DAY_TUE = 1,
	DAY_WED = 2,
	DAY_THU = 3,
	DAY_FRI = 4,
	DAY_SAT = 5,
	DAY_SUN = 6,
} day_t;

typedef struct {
	byte secs;
	byte mins;
	byte hour;
	char ampm;
}time_s;

typedef struct {
	day_t day;//???
	byte date;
	month_t month;
	byte year;
}date_s;

typedef struct {
	time_s time;
	date_s date;
}timeDate_s;
//??1
typedef struct {
	byte limitRules[10];				//?????????????§»¦Â??
	byte rule;									//rule=1,2,3,4 ????1,????2,????3,????????
}limitCouple_s;								//?????????

typedef struct {
	bool usedRule1;							//??????????????1
	bool usedRule2;							//??????????????2
	bool usedRule3;							//??????????????3
	byte matchTimes;							//????????????¡ê???????????
}limCoupLocal_s;								//???????????????flash

//??2
typedef struct {
	bool normal;								//??????????¦Â?????????????????????¦Â???????????
	byte startYMD[3];						//?????????????
	byte endYMD[3];							//??????????????
}limitDS_s;										//?????

typedef struct {
	byte validDaysH;						//??????????????????
	byte validDaysT;						//?
	byte validDaysO;						//??
}limDSLocal_s;								//???????????????flash

//??3
typedef struct {				
	byte startYMD[3];						//??¦Ï????????
	byte endYMD[3];							//??¦Í?????????
	byte limitNum[10];					//???????¦Â??
}limitJJJ_s;

typedef struct {
	ulong tailNo;									//4????????????????4????????
	byte mode;
	
	limitCouple_s limitCouple;		//?????????
	limitDS_s limitDS;						//???????
	limitJJJ_s limitJJJ_stage[4];	//??????????
																//Todo????????????
}driverInfo_s;

typedef struct
{
	unsigned char hour;
	unsigned char min;
	float temp;
	unsigned char shidu;
	int height;
}HistoryData;

typedef enum
{
	TIMEMODE_24HR = 0,
	TIMEMODE_12HR = 1
} timemode_t;

typedef struct{
	byte hour;
	byte min;
	union {
		byte days;
			struct{ // get rid of these bitfields?
			bool mon:1;
			bool tues:1;
			bool wed:1;
			bool thurs:1;
			bool fri:1;
			bool sat:1;
			bool sun:1;
			bool enabled:1;
		};
	};
}alarm_s;

// Could use bitfields for the bools to save a few bytes of RAM and EEPROM, but uses an extra ~82 bytes of flash
typedef struct{
	//byte sleepMode;
	//byte sleepBrightness;
	byte sleepTimeout;
	//byte brightness;
	bool invert;
#if COMPILE_ANIMATIONS
	bool animations;
#endif
	//byte clockface;
	bool display180;
  bool CTRL_LEDs;
	bool showFPS;
	timemode_t timeMode;
	union {
		ulong volumes;
		struct{ // get rid of these bitfields?
			byte volUI;
			byte volAlarm;
			byte volHour;
			byte brightness;
			};
		};
}appconfig_s;

typedef display_t (*draw_f)(void);
typedef void (*display_f)(void);

// Function for buttons to call
// Return true to only call function once per press
// Return false to continually call function while the button is pressed

//?????????????bool????????
typedef bool (*button_f)(void);

typedef void (*menu_f)(void);
typedef void (*itemLoader_f)(byte);

typedef struct{
	menu_f btn1; // make array
	menu_f btn2;
	menu_f btn3;
	draw_f draw;
	itemLoader_f loader;
}menuFuncs_t;

typedef struct{
	byte selected;
	byte scroll;
	byte optionCount;
	bool isOpen;
	const char* title;
	menu_type_t menuType;
	menuFuncs_t func;
	menu_f prevMenu;
}menu_s;

typedef struct{
	byte lastSelected;
	menu_f last;
}prev_menu_s;

typedef struct{
	bool active;             //????
	byte offsetY;            //???Y
	void (*animOnComplete)(void);    //???????
	bool goingOffScreen;   //????????
}anim_s;

typedef struct{
	byte x;
	byte y;
	const byte* bitmap;
	byte width;
	byte height;
//	byte foreColour;
	bool invert;
	byte offsetY;
}image_s;


typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
#endif /* TYPEDEFS_H_ */
