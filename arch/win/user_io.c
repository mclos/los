/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/
#include "los.h"
#include "user_io.h"
#include <windows.h>
#include "stdio.h"
#include "string.h"
void function_los(void *los)
{
}
void os_delay_ms_los(void *los)
{
	Sleep(los_get_u32(los, 1));
}

extern uint16_t win_width;
extern uint16_t win_height;
extern COLORREF *lcd_buf;
/**
 * 获取当前lcd
 * */
void *los_get_lcd_buf(void)
{
	void *p = lpram_malloc(win_width * win_height * 4);
	if (p)
	{
		memcpy(p, lcd_buf, win_width * win_height * 4);
	}
	return p;
}
/**
 * 把pop弹出来
 * */
void los_pop_lcd_buf(void *buf)
{
	memcpy(lcd_buf, buf, win_width * win_height * 4);
	lpram_free(buf);
}


 void los_delayms(unsigned int ms){
				Sleep(10);
 }