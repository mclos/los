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
 
 

/////////////////////////////////以下time//////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
static time_t win_tm;
/**
 * 这是win的time初始化
 * */
void win_time_init()
{
	 
}
 
void win_time_tick()
{
	 
}

uint32_t los_get_time(tm_t *tm)
{
	 
	return 0;
}
uint32_t los_set_time(tm_t *tm)
{
 
	return 0;
}

unsigned int lp_log_time_out(void)
{
    
    return 0;
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