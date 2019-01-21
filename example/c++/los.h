/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.Author's mailbox:lgtbp@126.com.
 /
 / 未经授权，禁止商用。Commercial use is prohibited without authorization.
 /----------------------------------------------------------------------------*/
#ifndef __LOS_T_H__
#define __LOS_T_H__
#include "stdio.h"
#include "string.h"
#include "stdint.h"
#define los_api_vs 0   
#define los_bulid_vs 0  
#define REG_NUM 8  
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
typedef struct _losc_t
{
	uint8_t os_flag;
	uint8_t push_num;
	cpu_reg_t reg[REG_NUM];
	uint8_t *ram;
	uint32_t ram_len;
	uint32_t lvar_start;
	uint32_t num;
	uint32_t end;
	uint32_t code_len;
	uint8_t *code;
	uint8_t *glb_psc;
	uint32_t txt_len;
} losc_t;
typedef uint32_t (*fun_os)(losc_t *);
////////////////////////////////////////////////////////////////////////////////////////////
/**
获取函数参数num从1开始
Get function parameter num from 1
*/
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
void los_return(losc_t *lp, int32_t ret);

/**
移植请修改内存管理api
Please modify the memory management API
*/
#define lpram_malloc malloc
#define lpram_free free
/**
退出los
*/
uint32_t los_quit(losc_t *lp);
/**
调用los文件中的函数，先调用此函数
Call the function in the los file, first call this function
*/
void los_arg_clear(losc_t *lp);
/**
调用los文件中的函数，传入arg参数
Call the function in the los file, passing in the arg parameter
*/
uint32_t los_push_arg(losc_t *lp, uint32_t arg);
/**
指定调用los文件中的函数的addr地址
Specify the addr address of the function in the call los file
*/
uint32_t los_call_addr(losc_t *lp, uint32_t addr);
/**
设置用户函数 
Set user function
*/
void los_set_function(fun_os f);
/**
运行los文件
addr参数:los文件在ram或rom中的地址
Run los file
Addr parameter: the address of the los file in ram or rom
*/
uint32_t los_app_first(uint8_t *addr);

/** note
使用请参考test.c main.c文件
Please refer to test.c main.c file for use.
*/
#endif /*  */
