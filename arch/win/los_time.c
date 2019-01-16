/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/
#include "los.h"
#include "los_time.h"
#include <time.h>
#ifdef WINDOW
#include <sys/time.h>
#endif

/////////////////////////////////以下time//////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
static time_t win_tm;
/**
 * 这是win的time初始化
 * */
void win_time_init()
{
	win_tm = time(NULL);
}
 
void win_time_tick()
{
	win_tm++;
}

uint32_t los_get_time(tm_t *tm)
{
	struct tm *lctime = localtime(&win_tm);
	tm->sec = lctime->tm_sec;
	tm->min = lctime->tm_min;
	tm->hour = lctime->tm_hour;
	tm->mday = lctime->tm_mday;
	tm->month = lctime->tm_mon + 1;
	tm->year = lctime->tm_year + 1900;
	tm->week = lctime->tm_wday + 1;
	return 0;
}
uint32_t los_set_time(tm_t *tm)
{
	struct tm lctime;
	lctime.tm_sec = tm->sec;
	lctime.tm_min = tm->min;
	lctime.tm_hour = tm->hour;
	lctime.tm_mday = tm->mday;
	lctime.tm_mon = tm->month - 1;
	lctime.tm_year = tm->year - 1900;
	lctime.tm_wday = tm->week - 1;
	win_tm = mktime(&lctime);
	return 0;
}

unsigned int lp_log_time_out(void)
{
    time_t now; //秒数
    struct tm *tm_now;
    time(&now);
    tm_now = localtime(&now);
    return tm_now->tm_min * 60 + tm_now->tm_sec;
}
/////////////////////////////////以上time//////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//设置时间
void set_time_los(void *los)
{
	tm_t *mtm = los_get_voidp(los, 1);
	los_return(los, los_set_time(mtm));
}
// 返回当前时间
void get_time_los(void *los)
{
	tm_t *mtm = los_get_voidp(los, 1);
	los_return(los, los_get_time(mtm));
}