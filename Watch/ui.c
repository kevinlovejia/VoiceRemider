/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"

float BatteryVol;
void drawBattery()
{              //battery_voltage();
	int bat; char ad[5];
	const byte* battIcon;
	if(BatteryVol < 3.2)
		battIcon = battIconEmpty;
	else if(BatteryVol < 3.7)
		battIcon = battIconLow;
	else if(BatteryVol < 4)
		battIcon = battIconHigh;
	else
		battIcon = battIconFull;

	draw_bitmap(0, FRAME_HEIGHT - 8, battIcon, 16, 8, NOINVERT, 0);
	if((4.2-BatteryVol)<1)//经测量断电时电压为3.3V左右，电压区间位3.3v-4.21v
	 bat=(BatteryVol-3.2)*99;
	else
	 bat=99;
	sprintf((char *)ad,"%d", bat);//
	draw_string(ad,NOINVERT,18,FRAME_HEIGHT-8);
}
millis8_t nowdr, lastdr = 0;

void drawDriveSignal()
{
	nowdr = millis();
	if(nowdr - lastdr > 2000)
	{		
		draw_bitmap(0, FRAME_HEIGHT - 16, noEntering, 16, 16, NOINVERT, 0);
		draw_bitmap(32-16, FRAME_HEIGHT - 16, textDisplay[0], 16, 16, NOINVERT, 0);
		draw_bitmap(48-16, FRAME_HEIGHT - 16, textDisplay[15], 16, 16, NOINVERT, 0);
		draw_bitmap(64-16, FRAME_HEIGHT - 16, textDisplay[2], 16, 16, NOINVERT, 0);
		draw_bitmap(80-16, FRAME_HEIGHT - 16, textDisplay[3], 16, 16, NOINVERT, 0);
		draw_bitmap(FRAME_WIDTH-32, FRAME_HEIGHT - 16, textDisplay[23], 16, 16, NOINVERT, 0);//汽车标志
		draw_bitmap(FRAME_WIDTH-14, FRAME_HEIGHT - 16, numFont11x16[1], 11, 16, NOINVERT, 0);//1
	}
	else if(nowdr - lastdr > 4000)
	{
		lastdr = nowdr;
		draw_bitmap(16, FRAME_HEIGHT - 16, driving, 16, 16, NOINVERT, 0);
		draw_bitmap(32, FRAME_HEIGHT - 16, textDisplay[0+4], 16, 16, NOINVERT, 0);
		draw_bitmap(48, FRAME_HEIGHT - 16, textDisplay[1+4], 16, 16, NOINVERT, 0);
		draw_bitmap(64, FRAME_HEIGHT - 16, textDisplay[2+4], 16, 16, NOINVERT, 0);
		draw_bitmap(80, FRAME_HEIGHT - 16, textDisplay[3+4], 16, 16, NOINVERT, 0);
	}
	
	

	
}

