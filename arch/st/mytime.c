/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/
#include "los.h"
#include "mytime.h"
#include <time.h>
#ifdef WINDOW
#include <sys/time.h>
#endif

 uint32_t los_get_time(tm_t *tm)
{
    return 0;
}
 uint32_t los_set_time(tm_t *tm)
{
    return 0;
}
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

extern RTC_HandleTypeDef hrtc;
unsigned int lp_log_time_out(void)
{
    RTC_TimeTypeDef stimestructure;
    HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
    return stimestructure.Minutes * 60 + stimestructure.Seconds;
}