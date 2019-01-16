/**
 * @file         los.h
 * @author       lp
 * @version  	 R0.1
 * @par Copyright (c):2014-2017 LP,All Rights Reserved.
 */

#ifndef _LOS_H__
#define _LOS_H__
 
#include "stdint.h"
#include "los_t.h"

#ifdef FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#define LOS_LOCK taskENTER_CRITICAL()
#define LOS_UNLOCK taskEXIT_CRITICAL()
#else
#define LOS_LOCK
#define LOS_UNLOCK
#endif
enum LOS_RET
{
	LOS_OK,   //
	API_EXIT, //
	MEM_NO,   //
};
typedef void (*fun_u)(void *);
typedef struct _los_api_fun_t
{
	void (*fun)(void *);
} los_api_fun_t;

uint8_t los_get_u8(losc_t *lost, uint32_t num);
int8_t los_get_s8(losc_t *lost, uint32_t num);
uint16_t los_get_u16(losc_t *lost, uint32_t num);
int16_t los_get_s16(losc_t *lost, uint32_t num);
uint32_t los_get_u32(losc_t *lost, uint32_t num);
int32_t los_get_s32(losc_t *lost, uint32_t num);
float los_get_float(losc_t *lost, uint32_t num);
uint64_t los_get_u64(losc_t *lost, uint32_t num);
int64_t los_get_s64(losc_t *lost, uint32_t num);
double los_get_double(losc_t *lost, uint32_t num);

void *los_get_voidp(losc_t *lost, uint32_t num);
uint8_t *los_get_u8p(losc_t *lost, uint32_t num);
int8_t *los_get_s8p(losc_t *lost, uint32_t num);
uint16_t *los_get_u16p(losc_t *lost, uint32_t num);
int16_t *los_get_s16p(losc_t *lost, uint32_t num);
uint32_t *los_get_u32p(losc_t *lost, uint32_t num);
int32_t *los_get_s32p(losc_t *lost, uint32_t num);
uint64_t *los_get_u64p(losc_t *lost, uint32_t num);
int64_t *los_get_s64p(losc_t *lost, uint32_t num);
float *los_get_floatp(losc_t *lost, uint32_t num);
double *los_get_doublep(losc_t *lost, uint32_t num);

void los_return(losc_t *lost, int32_t ret);
void los_return_8b(losc_t *lost, int64_t ret);
void los_return_float(losc_t *lost, float ret);
void los_return_double(losc_t *lost, double ret);

void *los_get_p(losc_t *lost, uint32_t num);

losc_t *los_new(void);
uint32_t los_close(losc_t *lost);
uint32_t los_check_status(losc_t *los);
uint32_t los_quit(losc_t *lost);
uint32_t los_iqr(losc_t *lost, uint8_t num);

void los_set_use_api(const fun_u *use_api);
void los_set_lgui_api(const fun_u *lgui_api);

uint32_t los_tick(losc_t *lost);
uint32_t los_run_buf(losc_t *lost, uint8_t *data);
uint32_t los_run_fanme(losc_t *lost, uint8_t *fanme);
uint32_t los_rerun(losc_t *lost);
uint32_t los_add_lgui_api(losc_t *lost, const fun_u *lgui_api);
uint32_t los_add_use_api(losc_t *lost, const fun_u *use_api);
uint32_t los_wsize(losc_t *lost, uint16_t w, uint16_t h);
uint32_t los_get_wsize(losc_t *lost, uint16_t *w, uint16_t *h);
uint32_t los_free_ram(losc_t *los);
void los_set_path(losc_t *lost, uint8_t *path);
uint32_t los_rerun_no_ui(losc_t *lost);

#define LOS_LGUI_API_TYPE 0
#define LOS_USER_API_TYPE 1
extern const fun_u los_lgui_api[];
extern const fun_u los_user_api[];

uint32_t los_app_frist(uint8_t *pname);
/**
 * 外部调用
 * 用于最小化。当前运行的los
 * */
void los_app_min(void);

void los_set_api(uint8_t type, fun_u *api);
/**
 * los的主要入口：
 * type:
 *     0：是强制运行los.losfrist
 *     1：是查看los/mlos.文件的启动信息
*/
uint32_t los_app_main(uint8_t type);

/**PC上开机启动
 * */
uint8_t get_pc_los_path(uint8_t *name);
/*
开启新app
run_name:运行文件路径
cmd：传入参数
*/
int app_run(char *run_name, char *cmd);
uint8_t los_app_tick(void);

uint32_t los_appname_frist(uint8_t *app);

void *los_malloc(uint32_t size);
void *los_get_ram(void *p);
void los_free(void *p);
void los_app_mini(uint8_t type);

#endif /* _LPOS_H__ */
