/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/
#ifndef __LOS_T_H__
#define __LOS_T_H__

#include "string.h"
#include "stdint.h"

#ifdef LOS_LGUI_EN
#include "Lgui_obj.h"
#endif
#define los_api_vs 0   //编译版本
#define los_bulid_vs 0 //编译版本

#define REG_NUM 8 //reg num

#define USER_RAM_SIZE 0X00FFFFFF

typedef struct _lpram_t
{
	uint32_t offest;
	uint8_t *ram;
	uint16_t size;
	uint16_t bnum;
	uint16_t now;
	uint16_t min;
} lram_t;

void lram_init(lram_t *lram, uint8_t *ram, uint32_t num);
void *lrealloc(lram_t *lram, void *ram, uint32_t size);
void *lmalloc(lram_t *lram, uint32_t size);
uint32_t lfree(lram_t *lram, void *ram);

uint32_t lfree_size(lram_t *lram);
uint32_t lfree_max(lram_t *lram);

#define APP_THREAD

void *lpram_malloc(uint32_t size);
void lpram_free(void *ram);

/////////////////////////////////////////////////////////////
enum THREAD_FLG
{
	THREAD_FLG_80_EXIT = 0X80,
	THREAD_FLG_40_IO = 0X40,
	THREAD_FLG_20_INIT = 0X20,
	THREAD_FLG_10_CHANGE = 0X10,
	THREAD_FLG_08_TLOCK = 0X8,
	THREAD_FLG_04_CLOSE = 0X4,
	THREAD_FLG_02_WAIT = 0X2,
	THREAD_FLG_10_EVENT = 0X1,

};
/**
 * 线程类型
 * */
enum THREAD_TYPE
{
	THREAD_UI = 0, //
	THREAD_TIME = 1,
	THREAD_SERVER = 2,
};
typedef struct _THRED
{
	uint16_t name_num;
	uint8_t type;
	uint8_t run;
	uint32_t delay;
	uint32_t reload;
	uint8_t *ram;
	struct _THRED *next;
} thread_t;

typedef union _cpu_reg_t {
	uint8_t u8;
	int8_t s8;
	uint16_t u16;
	int16_t s16;
	uint32_t u32;
	int32_t s32;
	float f32;
	uint8_t data[8];
	uint64_t u64;
	int64_t s64;
	double d64;
} cpu_reg_t;

typedef struct _losc_t //6byte
{
	//	uint8_t *cmd;
	uint16_t cmd_len;
	uint8_t push_num;
	cpu_reg_t reg[REG_NUM];

	thread_t *thread;
	thread_t *thread_now;
	uint8_t thread_num_new;
	uint8_t os_flag;

	uint8_t *ram;
	uint32_t ram_len;
	uint32_t lvar_start;
	uint32_t num;
	uint32_t end;
	uint32_t irq_addr;

	uint32_t code_len;
	uint8_t *code;
	uint8_t *glb_psc;
	uint32_t txt_len;

	lram_t lram;
	uint8_t *path;
	uint8_t app_path_len;
	uint16_t lcd_w, lcd_h;
#ifdef LOS_LGUI_EN
	window_t *focus_win;
#endif
	void *lcd;
#ifdef WINDOW
	char filepath[512];
#else
	char filepath[128];
#endif
	uint32_t msg_addr;
} losc_t;

typedef uint32_t (*fun_os)(losc_t *);
////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _los_app_t
{
	uint8_t *path;
	uint8_t type;
	uint8_t run;
	losc_t *los;
	struct _los_app_t *next;
} los_app_t;
/**
 *  app链表
 * 第一个链表.内部不创建los
 * */
los_app_t *los_app_list_new(uint8_t *pname);
uint32_t los_app_list_del_one(los_app_t *phead, losc_t *los_old);
los_app_t *los_app_list_find(los_app_t *phead, losc_t *los_old);
los_app_t *los_app_list_add(los_app_t *L, uint8_t *pname);
los_app_t *los_app_list_find_path(los_app_t *phead, uint8_t *path);
/*返回单链表中元素的个数*/
uint32_t los_app_list_len(los_app_t *phead);

losc_t *los_app_list_set_star(los_app_t *phead, losc_t *los_old, uint8_t star);

thread_t *thread_list_new(uint16_t name_num);
thread_t *thread_list_find(thread_t *phead, uint16_t name_num);
thread_t *thread_list_find_run(thread_t *phead);
thread_t *thread_list_time_run(thread_t *phead);
thread_t *thread_list_add(thread_t *L, uint16_t name_num);
void thread_list_prin(thread_t *phead);
uint32_t thread_list_del(thread_t *p_last);

uint32_t thread_list_len(thread_t *phead);
uint32_t thread_list_del_one(thread_t *phead, uint8_t id);
////////////////////////////////////////////////////////////////////////////////////////////
uint32_t malloc_los(losc_t *los);
uint32_t free_los(losc_t *los);
uint32_t realloc_los(losc_t *los);
uint32_t calloc_los(losc_t *los);
uint32_t ram_free_los(losc_t *los);
uint32_t ram_max_los(losc_t *los);
uint32_t memset_los(losc_t *los);
uint32_t memcpy_los(losc_t *los);
uint32_t memcmp_los(losc_t *los);
uint32_t strlen_los(losc_t *los);
uint32_t strcpy_los(losc_t *los);
uint32_t strcmp_los(losc_t *los);

uint32_t los_thread_change(losc_t *lp, thread_t *thread_next);

uint32_t thread_change_check(losc_t *lp);

extern void check_lgui_event();

uint32_t los_wait_event(losc_t *lp);

uint32_t los_delay(losc_t *lp);

uint32_t los_printf(losc_t *lp);

uint32_t los_stack_use_size(losc_t *lp);

uint32_t los_thread_new(losc_t *lp);
//--------------------------------------------------
//开始线程
//--------------------------------------------------
uint32_t los_thread_start(losc_t *lp);
//--------------------------------------------------
//外部挂起线程
//--------------------------------------------------
uint32_t los_thread_stop(losc_t *lp);

//--------------------------------------------------
//删除线程
//--------------------------------------------------
uint32_t los_thread_del(losc_t *lp);
//--------------------------------------------------

//--------------------------------------------------
//time立刻结束
//--------------------------------------------------
uint32_t los_time_end(losc_t *lp);
//--------------------------------------------------

//创建线程 -----这些函数成功后，移到lpide。
//线程名字-第二个参数是函数的地址
//--------------------------------------------------
uint32_t los_time_set(losc_t *lp);

//--------------------------------------------------
//获取一个flag
//--------------------------------------------------
uint32_t los_semaphore2_get(losc_t *lp);

//--------------------------------------------------
//fa send一个flag
//--------------------------------------------------
uint32_t los_semaphore2_send(losc_t *lp);
//--------------------------------------------------
//获取一个flag
//--------------------------------------------------
uint32_t los_semaphoreflag_get(losc_t *lp);

//--------------------------------------------------
//fa send一个flag
//--------------------------------------------------
uint32_t los_semaphoreflag_free(losc_t *lp);
//--------------------------------------------------
//就只有自身线程运行10个tick
//线程请求进入临界或离开临界
//--------------------------------------------------
uint32_t los_in(losc_t *lp);
//--------------------------------------------------
//退出los
//--------------------------------------------------
uint32_t los_quits(losc_t *lp);
//这返回16字节的版本，8bit是api的版本号码，后8bit是代码更改序号
uint32_t los_version(losc_t *lp);

//返回cmd std
uint32_t los_arg_v(losc_t *lp);
uint32_t los_arg_c(losc_t *lp);
///////////////////////////////////////////////////////////
///////////////////////app////////////////////////////////
extern los_app_t *los_apps;
/////////////////////////////////////////////////////////
/**
 * 开始一个新的app，当前app，就退到后台
 * */
uint32_t los_exe_start(losc_t *lp);
/**
 * 退到后台
 * */
uint32_t los_exe_stop(losc_t *lp);
/**
 * 重后台删除
 * */
uint32_t los_exe_exit(losc_t *lp);

/**
 * 播放音乐
 * plya_audio(name,cb,type);
 * */
uint32_t los_play_audio(losc_t *lp);

void usart_check_out_los(void *los);
void printf_los(void *los);
void sprintf_los(void *los);

void los_get_windowsize(void *los);
void los_set_msg(void *los);
#endif /* LPIDE_H__ */
