/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/
#include "los.h"
#include "los_t.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "los_fat_config.h"
#include "los_gpu.h"
#include "lcdDriver.h"
#include "los_usart.h"
#include "los_time.h"
#include "los_fat.h"
#include "user_io.h"
#include "los_gpu.h"

static fun_u *ker_lgui_api;
static fun_u *ker_use_api;

#define ARRLENs(arr) (sizeof(arr) / sizeof(arr[0]))
const uint8_t tlen[] =
	{1, 2, 4, 8};
const uint8_t txtlen[] =
	{1, 2, 3, 4, 8};
typedef uint32_t (*los_cmd)(losc_t *);


//--------------------------------------------------
// new a los
//--------------------------------------------------
losc_t *los_new(void)
{
	losc_t *lp;
	lp = (losc_t *)lpram_malloc(sizeof(losc_t));
	if (0 == lp)
		return 0;
	memset(lp, 0, sizeof(losc_t));

	return lp;
}

uint32_t los_quit(losc_t *lp)
{
	lp->os_flag |= 0x80;
	return 0;
}
//--------------------------------------------------
// los_close
//--------------------------------------------------
uint32_t los_close(losc_t *lp)
{
	if (lp->thread_num_new)
	{
		thread_list_del(lp->thread);
	}
	lpram_free(lp->ram);
	lpram_free(lp);
	//lpram_free(los);
	//los = 0;
	return 0;
}
//--------------------------------------------------
// los_free
//--------------------------------------------------
uint32_t los_free_ram(losc_t *lp)
{
	if (lp->thread_num_new)
	{
		thread_list_del(lp->thread);
	}
	if (lp->code)
		lpram_free(lp->code);
	lpram_free(lp->ram);
	return 0;
}

uint8_t los_init_flg(losc_t *lp, uint8_t flg)
{
	uint32_t ram_len;
	if (flg & 0x80)
	{
		lp->thread_num_new = 0;
		lp->thread = thread_list_new(0);
		if (0 == lp->thread)
			return 2;
		lp->thread->type = THREAD_UI;
		lp->thread->run = 0x80;
		lp->thread->delay = 0;
		lp->thread->reload = 10;
		lp->thread_now = lp->thread;
		ram_len = lp->ram_len - lp->lvar_start;
		lp->thread->ram = (uint8_t *)lpram_malloc(77 + ram_len);
		if (lp->thread->ram == 0)
			return 3;
	}

	return 0;
}

typedef uint32_t (*fun_os)(losc_t *);

const fun_os los_sys_apis[] = {
	los_wait_event, //
	los_set_msg,	//
	los_delay,		//
	los_printf,
	los_stack_use_size,		//
	los_thread_new,			//
	los_thread_start,		//
	los_thread_stop,		//
	los_thread_del,			///
	los_time_end,			///
	los_time_set,			//
	los_semaphore2_get,		//
	los_semaphore2_send,	//
	los_semaphoreflag_get,  //
	los_semaphoreflag_free, //
	los_in,					//
	los_quits,				//
	los_version,			//
	los_exe_start,
	los_exe_stop,
	los_exe_exit,
	los_arg_v, // no done
	los_arg_c, // no done

	malloc_los,
	free_los,
	realloc_los,
	calloc_los, //
	ram_free_los,
	ram_max_los, //
	memset_los,
	memcpy_los,
	memcmp_los,
	strlen_los,
	strcpy_los,
	strcmp_los, //

	los_play_audio, //
	////////// user_io.h   /////////////////////
	function_los, //
	os_delay_ms_los,
	printf_los,
	sprintf_los,
	////////// user_io.h/////////////////////

	////////// fat.h   /////////////////////
	fopen_los,
	fread_los,
	fwrite_los,
	fclone_los,
	fgetc_los,
	fputc_los,
	fgets_los,
	fputs_los,
	ftell_los,
	rewind_los,
	fseek_los,
	file_size_los,	//
	mkdir_los,		  //
	fat_deorphan_los, //
	fat_total_los,	//
	fat_free_los,	 //

	remove_los,
	rename_los,
	opendir_los,
	readdir_los,
	closedir_los,
	rewinddir_los,
	telldir_los,
	seekdir_los,
	//////////fat.h/////////////////////

	//////////time.h 2/////////////////////
	get_time_los,
	set_time_los,
	//////////time.h/////////////////////

	usart_init_los,
	usart_disinit_los,
	usart_send_byte_los,
	usart_send_bytes_los,
	usart_read_byte_los,

	//////////////////////////////////////////////
	los_get_windowsize,
	los_draw_point_los,
	los_read_point_los,
	los_draw_line_los,
	los_draw_hline_los,
	los_draw_vline_los,
	los_draw_rect_los,
	los_draw_frect_los,
	los_draw_circle_los,
	los_draw_fcircle_los,
	los_draw_buf_los,
	los_read_buf_los,
};
 

static uint32_t cmd_2(losc_t *lp)
{
	if ((lp->num + *lp->glb_psc) > lp->end)
		return 12;
	memset(&lp->ram[lp->num], 0, *lp->glb_psc);
	lp->num += *lp->glb_psc++;
	return 0;
}

static uint32_t cmd_3(losc_t *lp)
{
	uint16_t len = lp->glb_psc[0] | lp->glb_psc[1] << 8;
	if ((lp->num + len) > lp->end)
		return 13;
	memset(&lp->ram[lp->num], 0, len);
	lp->num += len;
	lp->glb_psc += 2;
	return 0;
}

static uint32_t cmd_4(losc_t *lp)
{
	uint32_t len = lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16;
	if ((lp->num + len) > lp->end)
		return 14;
	memset(&lp->ram[lp->num], 0, len);
	lp->num += len;
	lp->glb_psc += 3;
	return 0;
}

static uint32_t cmd_5(losc_t *lp)
{
	lp->reg[(*lp->glb_psc) & 0x7].d64 = lp->reg[(*lp->glb_psc) >> 3].d64;
	lp->glb_psc++;
	return 0;
}

static uint32_t cmd_6(losc_t *lp)
{
	lp->reg[REG_NUM - 1].u32 = *lp->glb_psc++;
	return 0;
}

static uint32_t cmd_7(losc_t *lp)
{
	lp->reg[REG_NUM - 1].u32 = lp->glb_psc[0] | lp->glb_psc[1] << 8;
	lp->glb_psc += 2;
	return 0;
}

static uint32_t cmd_8(losc_t *lp)
{
	lp->reg[REG_NUM - 1].u32 = lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16;
	lp->glb_psc += 3;
	return 0;
}

static uint32_t cmd_9(losc_t *lp)
{
	lp->reg[REG_NUM - 1].u32 = lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16 | lp->glb_psc[3] << 24;
	lp->glb_psc += 4;
	return 0;
}

static uint32_t cmd_10(losc_t *lp)
{
	lp->reg[REG_NUM - 1].u64 = lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16 | lp->glb_psc[3] << 24 | lp->glb_psc[4] << 32 | lp->glb_psc[5] << 40 | lp->glb_psc[6] << 48 | lp->glb_psc[7] << 56;
	lp->glb_psc += 8;
	return 0;
}
static uint32_t cmd_11(losc_t *lp)
{
	lp->num -= *lp->glb_psc++;
	lp->glb_psc = (uint8_t *)(lp->ram[lp->end + 1] | lp->ram[lp->end + 2] << 8 | lp->ram[lp->end + 3] << 16 | lp->ram[lp->end + 4] << 24);
	lp->end += 4;
	return 0;
}
static uint32_t cmd_12(losc_t *lp)
{
	lp->num -= lp->glb_psc[0] | lp->glb_psc[1] << 8;
	lp->glb_psc += 2;
	lp->glb_psc = (uint8_t *)(lp->ram[lp->end + 1] | lp->ram[lp->end + 2] << 8 | lp->ram[lp->end + 3] << 16 | lp->ram[lp->end + 4] << 24);
	lp->end += 4;
	return 0;
}
static uint32_t cmd_13(losc_t *lp)
{
	lp->num -= lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16;
	lp->glb_psc += 3;
	lp->glb_psc = (uint8_t *)(lp->ram[lp->end + 1] | lp->ram[lp->end + 2] << 8 | lp->ram[lp->end + 3] << 16 | lp->ram[lp->end + 4] << 24);
	lp->end += 4;
	return 0;
}
static uint32_t cmd_irq_pop_b(losc_t *lp)
{
	lp->num -= *lp->glb_psc++;
	lp->glb_psc = (uint8_t *)(lp->ram[lp->end + 1] | lp->ram[lp->end + 2] << 8 | lp->ram[lp->end + 3] << 16 | lp->ram[lp->end + 4] << 24);
	lp->end += 4;
	return 0;
}
static uint32_t cmd_irq_pop_2b(losc_t *lp)
{
	lp->num -= lp->glb_psc[0] | lp->glb_psc[1] << 8;
	lp->glb_psc += 2;
	lp->glb_psc = (uint8_t *)(lp->ram[lp->end + 1] | lp->ram[lp->end + 2] << 8 | lp->ram[lp->end + 3] << 16 | lp->ram[lp->end + 4] << 24);
	lp->end += 4;
	return 0;
}
static uint32_t cmd_irq_pop_3b(losc_t *lp)
{
	lp->num -= lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16;
	lp->glb_psc += 3;
	lp->glb_psc = (uint8_t *)(lp->ram[lp->end + 1] | lp->ram[lp->end + 2] << 8 | lp->ram[lp->end + 3] << 16 | lp->ram[lp->end + 4] << 24);
	lp->end += 4;
	return 0;
}

static uint32_t cmd_14(losc_t *lp)
{
	lp->push_num += 4;
	memcpy(&lp->ram[lp->num], lp->reg[*lp->glb_psc & 0x7].data,
		   *lp->glb_psc >> 3);
	lp->num += 4;
	lp->glb_psc++;
	return 0;
}
static uint32_t cmd_15(losc_t *lp)
{
	lp->push_num += 8;
	memcpy(&lp->ram[lp->num], lp->reg[*lp->glb_psc & 0x7].data,
		   *lp->glb_psc >> 3);
	lp->num += 8;
	lp->glb_psc++;
	return 0;
}
static uint32_t cmd_16(losc_t *lp)
{

	lp->push_num += 4;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = 0;
	lp->ram[lp->num++] = 0;
	lp->ram[lp->num++] = 0;

	return 0;
}
static uint32_t cmd_17(losc_t *lp)
{
	lp->push_num += 4;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = 0;
	lp->ram[lp->num++] = 0;
	return 0;
}
static uint32_t cmd_18(losc_t *lp)
{
	lp->push_num += 4;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = 0;
	return 0;
}
static uint32_t cmd_19(losc_t *lp)
{
	lp->push_num += 4;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = *lp->glb_psc++;
	return 0;
}
static uint32_t cmd_20(losc_t *lp)
{
	lp->push_num += 8;
	lp->ram[lp->num] = *lp->glb_psc++;
	lp->num += 8;
	return 0;
}
static uint32_t cmd_21(losc_t *lp)
{
	lp->push_num += 8;
	lp->ram[lp->num] = *lp->glb_psc++;
	lp->ram[lp->num + 1] = *lp->glb_psc++;
	lp->num += 8;
	return 0;
}
static uint32_t cmd_22(losc_t *lp)
{
	lp->push_num += 8;
	lp->ram[lp->num] = *lp->glb_psc++;
	lp->ram[lp->num + 1] = *lp->glb_psc++;
	lp->ram[lp->num + 2] = *lp->glb_psc++;
	lp->num += 8;
	return 0;
}
static uint32_t cmd_23(losc_t *lp)
{
	lp->push_num += 8;
	lp->ram[lp->num] = *lp->glb_psc++;
	lp->ram[lp->num + 1] = *lp->glb_psc++;
	lp->ram[lp->num + 2] = *lp->glb_psc++;
	lp->ram[lp->num + 3] = *lp->glb_psc++;
	lp->num += 8;
	return 0;
}
static uint32_t cmd_24(losc_t *lp)
{
	lp->push_num += 8;
	memcpy(&lp->ram[lp->num], lp->glb_psc, 8);
	lp->glb_psc += 8;
	lp->num += 8;
	return 0;
}

static uint32_t cmd_61(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7; //
	uint8_t len = *lp->glb_psc >> 3, i;
	lp->glb_psc++;
	lp->reg[reg].d64 = 0;
	for (i = 0; i < len; ++i)
	{
		lp->reg[reg].data[i] = *lp->glb_psc++;
	}
	return 0;
}

static uint32_t cmd_62(losc_t *lp)
{
	uint8_t id = *lp->glb_psc & 0x7;
	uint8_t len = tlen[(*lp->glb_psc >> 3) & 0x3], i, j;
	uint32_t addr = 0;
	i = *lp->glb_psc >> 5;
	lp->glb_psc++;
	for (j = 0; j < i; ++j)
	{
		addr |= (*lp->glb_psc) << (j * 8);
		lp->glb_psc++;
	}
	if (addr > lp->num)
		return 12;
	for (i = 0; i < len; ++i)
	{
		lp->ram[lp->num - addr + i] = lp->reg[id].data[i];
	}
	return 0;
}
uint32_t cmd_1(losc_t *lp)
{
	return 1;
}

static uint32_t cmd_63(losc_t *lp)
{
	uint8_t id = *lp->glb_psc & 0x7;
	uint8_t len = tlen[(*lp->glb_psc >> 3) & 0x3], i, j;
	uint32_t addr = 0;
	i = *lp->glb_psc >> 5;
	lp->glb_psc++;
	for (j = 0; j < i; ++j)
	{
		addr |= (*lp->glb_psc) << (j * 8);
		lp->glb_psc++;
	}
	if (addr > lp->num)
		return 12;
	lp->reg[id].u64 = 0;
	for (i = 0; i < len; ++i)
	{
		lp->reg[id].data[i] = lp->ram[lp->num - addr + i];
	}
	return 0;
}

static uint32_t cmd_25(losc_t *lp)
{
	uint32_t addr, t1;
	lp->push_num = 0;
	t1 = (lp->glb_psc[0] | lp->glb_psc[1] << 8);
	lp->glb_psc += 2;
	addr = (uint32_t)lp->glb_psc;
	lp->ram[lp->end--] = addr >> 24;
	lp->ram[lp->end--] = addr >> 16;
	lp->ram[lp->end--] = addr >> 8;
	lp->ram[lp->end--] = addr;
	if (t1 > lp->code_len)
	{
		return 10;
	}
	lp->glb_psc = lp->code + t1;
	return 0;
}

static uint32_t cmd_26(losc_t *lp)
{
	uint32_t addr, t1;
	lp->push_num = 0;
	t1 = (lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16);
	lp->glb_psc += 3;
	addr = (uint32_t)lp->glb_psc;
	lp->ram[lp->end--] = addr >> 24;
	lp->ram[lp->end--] = addr >> 16;
	lp->ram[lp->end--] = addr >> 8;
	lp->ram[lp->end--] = addr;
	if (t1 > lp->code_len)
		return 10;
	lp->glb_psc = lp->code + t1;
	return 0;
}

static uint32_t cmd_27(losc_t *lp)
{
	uint32_t addr, t1;
	lp->push_num = 0;
	t1 = (lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16 | lp->glb_psc[3] << 24);
	lp->glb_psc += 4;
	addr = (uint32_t)lp->glb_psc;
	lp->ram[lp->end--] = addr >> 24;
	lp->ram[lp->end--] = addr >> 16;
	lp->ram[lp->end--] = addr >> 8;
	lp->ram[lp->end--] = addr;
	if (t1 > lp->code_len)
		return 10;
	lp->glb_psc = lp->code + t1;
	return 0;
}

uint32_t cmd_addr_num(losc_t *lp, uint32_t addr_now)
{
	uint32_t addr;
	addr = (uint32_t)lp->glb_psc;
	lp->ram[lp->end--] = addr >> 24;
	lp->ram[lp->end--] = addr >> 16;
	lp->ram[lp->end--] = addr >> 8;
	lp->ram[lp->end--] = addr;
	if (addr_now > lp->code_len)
		return 10;
	lp->glb_psc = lp->code + addr_now;
	return 0;
}

static uint32_t cmd_28(losc_t *lp)
{
	uint32_t addr, t1;
	lp->push_num = 0;
	t1 = lp->glb_psc[0];
	lp->glb_psc++;
	if (lp->reg[t1].u32 > lp->code_len)
		return 10;
	addr = (uint32_t)lp->glb_psc;
	lp->ram[lp->end--] = addr >> 24;
	lp->ram[lp->end--] = addr >> 16;
	lp->ram[lp->end--] = addr >> 8;
	lp->ram[lp->end--] = addr;
	lp->glb_psc = lp->code + lp->reg[t1].u32;
	return 0;
}
static uint32_t cmd_29(losc_t *lp)
{
	uint16_t id = 0;
	if ((*lp->glb_psc) & 0x80)
	{
		id = lp->glb_psc[0] << 8 | lp->glb_psc[1];
		id -= 32768;
		lp->glb_psc += 2;
		if (ker_lgui_api)
			ker_lgui_api[id](lp);
	}
	else
	{
		id = *lp->glb_psc++;
		if (ker_lgui_api)
			ker_lgui_api[id](lp);
	}
	lp->num -= lp->push_num; //外设这个自动清空压栈数据
	lp->push_num = 0;
	return 0;
}
static uint32_t cmd_30(losc_t *lp)
{
	lp->num -= lp->push_num;
	lp->push_num = 0;
	return 0;
}
static uint32_t cmd_31(losc_t *lp)
{
	uint16_t id = 0;

	if ((*lp->glb_psc) & 0x80)
	{
		id = lp->glb_psc[0] << 8 | lp->glb_psc[1];
		id -= 32768;
		lp->glb_psc += 2;
		if (ker_use_api)
			ker_use_api[id](lp);
	}
	else
	{
		id = *lp->glb_psc++;
		if (ker_use_api)
			ker_use_api[id](lp);
	}

	lp->num -= lp->push_num;
	lp->push_num = 0;

	return 0;
}

static uint32_t cmd_58(losc_t *lp)
{
	uint8_t type = *lp->glb_psc++;
	switch (type)
	{
	case 0:
		lp->reg[lp->glb_psc[0] & 0x7].u32 += lp->reg[lp->glb_psc[0] >> 3].u32;
		break;
	case 1:
		lp->reg[lp->glb_psc[0] & 0x7].u32 -= lp->reg[lp->glb_psc[0] >> 3].u32;
		break;
	case 2:
		lp->reg[lp->glb_psc[0] & 0x7].u32 *= lp->reg[lp->glb_psc[0] >> 3].u32;
		break;
	case 3:
		lp->reg[lp->glb_psc[0] & 0x7].u32 /= lp->reg[lp->glb_psc[0] >> 3].u32;
		break;
	case 4:
		lp->reg[lp->glb_psc[0] & 0x7].u32 %= lp->reg[lp->glb_psc[0] >> 3].u32;
		break;
	case 5:
		lp->reg[lp->glb_psc[0] & 0x7].u32 &= lp->reg[lp->glb_psc[0] >> 3].u32;
		break;
	case 6:
		lp->reg[lp->glb_psc[0] & 0x7].u32 |= lp->reg[lp->glb_psc[0] >> 3].u32;
		break;
	case 7:
		lp->reg[lp->glb_psc[0] & 0x7].u32 ^= lp->reg[lp->glb_psc[0] >> 3].u32;
		break;
	case 8:
		lp->reg[lp->glb_psc[0] & 0x7].u32 =
			(lp->reg[lp->glb_psc[0] & 0x7].u32 == lp->reg[lp->glb_psc[0] >> 3].u32) ? 1 : 0;
		break;
	case 9:
		lp->reg[lp->glb_psc[0] & 0x7].u32 =
			(lp->reg[lp->glb_psc[0] & 0x7].u32 != lp->reg[lp->glb_psc[0] >> 3].u32) ? 1 : 0;
		break;
	case 10:
		lp->reg[lp->glb_psc[0] & 0x7].u32 =
			(lp->reg[lp->glb_psc[0] & 0x7].u32 > lp->reg[lp->glb_psc[0] >> 3].u32) ? 1 : 0;
		break;
	case 11:
		lp->reg[lp->glb_psc[0] & 0x7].u32 =
			(lp->reg[lp->glb_psc[0] & 0x7].s32 > lp->reg[lp->glb_psc[0] >> 3].s32) ? 1 : 0;
		break;
	case 12:
		lp->reg[lp->glb_psc[0] & 0x7].u32 =
			(lp->reg[lp->glb_psc[0] & 0x7].u32 < lp->reg[lp->glb_psc[0] >> 3].u32) ? 1 : 0;
		break;
	case 13:
		lp->reg[lp->glb_psc[0] & 0x7].u32 =
			(lp->reg[lp->glb_psc[0] & 0x7].s32 < lp->reg[lp->glb_psc[0] >> 3].s32) ? 1 : 0;
		break;
	case 14:
		lp->reg[lp->glb_psc[0] & 0x7].u32 =
			(lp->reg[lp->glb_psc[0] & 0x7].u32 >= lp->reg[lp->glb_psc[0] >> 3].u32) ? 1 : 0;
		break;
	case 15:
		lp->reg[lp->glb_psc[0] & 0x7].u32 =
			(lp->reg[lp->glb_psc[0] & 0x7].s32 >= lp->reg[lp->glb_psc[0] >> 3].s32) ? 1 : 0;
		break;
	case 16:
		lp->reg[lp->glb_psc[0] & 0x7].u32 =
			(lp->reg[lp->glb_psc[0] & 0x7].u32 <= lp->reg[lp->glb_psc[0] >> 3].u32) ? 1 : 0;
		break;
	case 17:
		lp->reg[lp->glb_psc[0] & 0x7].u32 =
			(lp->reg[lp->glb_psc[0] & 0x7].s32 <= lp->reg[lp->glb_psc[0] >> 3].s32) ? 1 : 0;
		break;
	case 18:
		lp->reg[lp->glb_psc[0] & 0x7].u32 <<= lp->reg[lp->glb_psc[0] >> 3].s32;
		break;
	case 19:
		lp->reg[lp->glb_psc[0] & 0x7].u32 >>= lp->reg[lp->glb_psc[0] >> 3].s32;
		break;
	case 20:
		lp->reg[lp->glb_psc[0] & 0x7].u32 >>= lp->reg[lp->glb_psc[0] >> 3].s32;
		break;
	case 21:
		lp->reg[lp->glb_psc[0] & 0x7].u32 =
			(lp->reg[lp->glb_psc[0] & 0x7].u32 && lp->reg[lp->glb_psc[0] >> 3].u32) ? 1 : 0;
		break;
	case 22:
		lp->reg[lp->glb_psc[0] & 0x7].u32 =
			(lp->reg[lp->glb_psc[0] & 0x7].u32 || lp->reg[lp->glb_psc[0] >> 3].u32) ? 1 : 0;
		break;
	case 23:
		lp->reg[lp->glb_psc[0] & 0x7].s32 /= lp->reg[lp->glb_psc[0] >> 3].s32;
		break;
	case 24:
		lp->reg[lp->glb_psc[0] & 0x7].s32 %= lp->reg[lp->glb_psc[0] >> 3].s32;
		break;
	default:
		return 19;
	}
	lp->glb_psc++;
	return 0;
}
static uint32_t cmd_59(losc_t *lp)
{
	switch (*lp->glb_psc++)
	{
	case 0:
		lp->reg[lp->glb_psc[0] & 0x7].f32 += lp->reg[lp->glb_psc[0] >> 3].f32;
		break;
	case 1:
		lp->reg[lp->glb_psc[0] & 0x7].f32 -= lp->reg[lp->glb_psc[0] >> 3].f32;
		break;
	case 2:
		lp->reg[lp->glb_psc[0] & 0x7].f32 *= lp->reg[lp->glb_psc[0] >> 3].f32;
		break;
	case 3:
		lp->reg[lp->glb_psc[0] & 0x7].f32 /= lp->reg[lp->glb_psc[0] >> 3].f32;
		break;
	case 8:
		lp->reg[lp->glb_psc[0] & 0x7].f32 =
			(lp->reg[lp->glb_psc[0] & 0x7].f32 == lp->reg[lp->glb_psc[0] >> 3].f32) ? 1 : 0;
		break;
	case 9:
		lp->reg[lp->glb_psc[0] & 0x7].f32 =
			(lp->reg[lp->glb_psc[0] & 0x7].f32 != lp->reg[lp->glb_psc[0] >> 3].f32) ? 1 : 0;
		break;
	case 10:
		lp->reg[lp->glb_psc[0] & 0x7].f32 =
			(lp->reg[lp->glb_psc[0] & 0x7].f32 > lp->reg[lp->glb_psc[0] >> 3].f32) ? 1 : 0;
		break;
	case 11:
		lp->reg[lp->glb_psc[0] & 0x7].f32 =
			(lp->reg[lp->glb_psc[0] & 0x7].s32 > lp->reg[lp->glb_psc[0] >> 3].s32) ? 1 : 0;
		break;
	case 12:
		lp->reg[lp->glb_psc[0] & 0x7].f32 =
			(lp->reg[lp->glb_psc[0] & 0x7].f32 < lp->reg[lp->glb_psc[0] >> 3].f32) ? 1 : 0;
		break;
	case 13:
		lp->reg[lp->glb_psc[0] & 0x7].f32 =
			(lp->reg[lp->glb_psc[0] & 0x7].s32 < lp->reg[lp->glb_psc[0] >> 3].s32) ? 1 : 0;
		break;
	case 14:
		lp->reg[lp->glb_psc[0] & 0x7].f32 =
			(lp->reg[lp->glb_psc[0] & 0x7].f32 >= lp->reg[lp->glb_psc[0] >> 3].f32) ? 1 : 0;
		break;
	case 15:
		lp->reg[lp->glb_psc[0] & 0x7].f32 =
			(lp->reg[lp->glb_psc[0] & 0x7].s32 >= lp->reg[lp->glb_psc[0] >> 3].s32) ? 1 : 0;
		break;
	case 16:
		lp->reg[lp->glb_psc[0] & 0x7].f32 =
			(lp->reg[lp->glb_psc[0] & 0x7].f32 <= lp->reg[lp->glb_psc[0] >> 3].f32) ? 1 : 0;
		break;
	case 17:
		lp->reg[lp->glb_psc[0] & 0x7].f32 =
			(lp->reg[lp->glb_psc[0] & 0x7].s32 <= lp->reg[lp->glb_psc[0] >> 3].s32) ? 1 : 0;
		break;
	case 21:
		lp->reg[lp->glb_psc[0] & 0x7].f32 =
			(lp->reg[lp->glb_psc[0] & 0x7].f32 && lp->reg[lp->glb_psc[0] >> 3].f32) ? 1 : 0;
		break;
	case 22:
		lp->reg[lp->glb_psc[0] & 0x7].f32 =
			(lp->reg[lp->glb_psc[0] & 0x7].f32 || lp->reg[lp->glb_psc[0] >> 3].f32) ? 1 : 0;
		break;
	case 23:
		lp->reg[lp->glb_psc[0] & 0x7].s32 /= lp->reg[lp->glb_psc[0] >> 3].s32;
		break;
	case 24:
		lp->reg[lp->glb_psc[0] & 0x7].s32 %= lp->reg[lp->glb_psc[0] >> 3].s32;
		break;

	default:
		return 10;
	}
	lp->glb_psc++;
	return 0;
}
static uint32_t cmd_60(losc_t *lp)
{
	switch (*lp->glb_psc++)
	{
	case 0:
		lp->reg[lp->glb_psc[0] & 0x7].d64 += lp->reg[lp->glb_psc[0] >> 3].d64;
		break;
	case 1:
		lp->reg[lp->glb_psc[0] & 0x7].d64 -= lp->reg[lp->glb_psc[0] >> 3].d64;
		break;
	case 2:
		lp->reg[lp->glb_psc[0] & 0x7].d64 *= lp->reg[lp->glb_psc[0] >> 3].d64;
		break;
	case 3:
		lp->reg[lp->glb_psc[0] & 0x7].d64 /= lp->reg[lp->glb_psc[0] >> 3].d64;
		break;
	case 8:
		lp->reg[lp->glb_psc[0] & 0x7].d64 =
			(lp->reg[lp->glb_psc[0] & 0x7].d64 == lp->reg[lp->glb_psc[0] >> 3].d64) ? 1 : 0;
		break;
	case 9:
		lp->reg[lp->glb_psc[0] & 0x7].d64 =
			(lp->reg[lp->glb_psc[0] & 0x7].d64 != lp->reg[lp->glb_psc[0] >> 3].d64) ? 1 : 0;
		break;
	case 10:
		lp->reg[lp->glb_psc[0] & 0x7].d64 =
			(lp->reg[lp->glb_psc[0] & 0x7].d64 > lp->reg[lp->glb_psc[0] >> 3].d64) ? 1 : 0;
		break;
	case 11:
		lp->reg[lp->glb_psc[0] & 0x7].d64 =
			(lp->reg[lp->glb_psc[0] & 0x7].s32 > lp->reg[lp->glb_psc[0] >> 3].s32) ? 1 : 0;
		break;
	case 12:
		lp->reg[lp->glb_psc[0] & 0x7].d64 =
			(lp->reg[lp->glb_psc[0] & 0x7].d64 < lp->reg[lp->glb_psc[0] >> 3].d64) ? 1 : 0;
		break;
	case 13:
		lp->reg[lp->glb_psc[0] & 0x7].d64 =
			(lp->reg[lp->glb_psc[0] & 0x7].s32 < lp->reg[lp->glb_psc[0] >> 3].s32) ? 1 : 0;
		break;
	case 14:
		lp->reg[lp->glb_psc[0] & 0x7].d64 =
			(lp->reg[lp->glb_psc[0] & 0x7].d64 >= lp->reg[lp->glb_psc[0] >> 3].d64) ? 1 : 0;
		break;
	case 15:
		lp->reg[lp->glb_psc[0] & 0x7].d64 =
			(lp->reg[lp->glb_psc[0] & 0x7].s32 >= lp->reg[lp->glb_psc[0] >> 3].s32) ? 1 : 0;
		break;
	case 16:
		lp->reg[lp->glb_psc[0] & 0x7].d64 =
			(lp->reg[lp->glb_psc[0] & 0x7].d64 <= lp->reg[lp->glb_psc[0] >> 3].d64) ? 1 : 0;
		break;
	case 17:
		lp->reg[lp->glb_psc[0] & 0x7].d64 =
			(lp->reg[lp->glb_psc[0] & 0x7].s32 <= lp->reg[lp->glb_psc[0] >> 3].s32) ? 1 : 0;
		break;
	case 21:
		lp->reg[lp->glb_psc[0] & 0x7].d64 =
			(lp->reg[lp->glb_psc[0] & 0x7].d64 && lp->reg[lp->glb_psc[0] >> 3].d64) ? 1 : 0;
		break;
	case 22:
		lp->reg[lp->glb_psc[0] & 0x7].d64 =
			(lp->reg[lp->glb_psc[0] & 0x7].d64 || lp->reg[lp->glb_psc[0] >> 3].d64) ? 1 : 0;
		break;
	case 23:
		lp->reg[lp->glb_psc[0] & 0x7].s32 /= lp->reg[lp->glb_psc[0] >> 3].s32;
		break;
	case 24:
		lp->reg[lp->glb_psc[0] & 0x7].s32 %= lp->reg[lp->glb_psc[0] >> 3].s32;
		break;
	default:
		return 11;
	}
	lp->glb_psc++;
	return 0;
}

static uint32_t cmd_34(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7, i;
	uint8_t len2 = tlen[(*lp->glb_psc >> 3) & 0x3];
	uint8_t len1 = tlen[*lp->glb_psc >> 5];
	lp->glb_psc++;
	for (i = len1; i < len2; i++)
		lp->reg[reg].data[i] = 0;
	return 0;
}
const uint32_t sext_data[] =
	{0x80, 0x8000, 0x800000, 0x80000000};
static uint32_t cmd_33(losc_t *lp)
{
	uint32_t i = 0;
	uint8_t reg = *lp->glb_psc & 0x7;
	uint8_t len2 = tlen[(*lp->glb_psc >> 3) & 0x3];
	uint8_t len1 = tlen[*lp->glb_psc >> 5];
	i = sext_data[*lp->glb_psc >> 5];
	lp->glb_psc++;
	if (lp->reg[reg].u32 & i)
	{
		for (i = len1; i < len2; i++)
			lp->reg[reg].data[i] = 0xFF;
	}
	return 0;
}
static uint32_t cmd_35(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7;
	uint8_t len1 = tlen[(*lp->glb_psc >> 3) & 0x3];
	uint8_t len2 = tlen[*lp->glb_psc >> 5];
	lp->glb_psc++;
	if (len1 == 1)
	{
		if (len2 == 1)
			lp->reg[reg].f32 = lp->reg[reg].u8;
		else if (len2 == 2)
			lp->reg[reg].f32 = lp->reg[reg].u16;
		else
			lp->reg[reg].f32 = lp->reg[reg].u32;
	}
	else
	{
		if (len2 == 1)
			lp->reg[reg].d64 = lp->reg[reg].u8;
		else if (len2 == 2)
			lp->reg[reg].d64 = lp->reg[reg].u16;
		else
			lp->reg[reg].d64 = lp->reg[reg].u32;
	}
	return 0;
}

static uint32_t cmd_s_2fp(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7;
	uint8_t len1 = tlen[(*lp->glb_psc >> 3) & 0x3];
	uint8_t len2 = tlen[*lp->glb_psc >> 5];
	lp->glb_psc++;
	if (len1 == 1)
	{
		if (len2 == 1)
			lp->reg[reg].f32 = lp->reg[reg].s8;
		else if (len2 == 2)
			lp->reg[reg].f32 = lp->reg[reg].s16;
		else
			lp->reg[reg].f32 = lp->reg[reg].s32;
	}
	else
	{
		if (len2 == 1)
			lp->reg[reg].d64 = lp->reg[reg].s8;
		else if (len2 == 2)
			lp->reg[reg].d64 = lp->reg[reg].s16;
		else
			lp->reg[reg].d64 = lp->reg[reg].s32;
	}
	return 0;
}
static uint32_t cmd_37(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7;
	lp->glb_psc++;
	lp->reg[reg].d64 = lp->reg[reg].f32;
	return 0;
}
static uint32_t cmd_36(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7;
	lp->glb_psc++;
	lp->reg[reg].f32 = lp->reg[reg].d64;
	return 0;
}
static uint32_t cmd_38(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7;
	uint8_t len1 = tlen[(*lp->glb_psc >> 3) & 0x3];
	uint8_t len2 = tlen[*lp->glb_psc >> 5];
	lp->glb_psc++;
	if (len1 == 1)
	{
		if (len2 == 1)
			lp->reg[reg].u8 = lp->reg[reg].f32;
		else if (len2 == 2)
			lp->reg[reg].u16 = lp->reg[reg].f32;
		else
			lp->reg[reg].u32 = lp->reg[reg].f32;
	}
	else
	{
		if (len2 == 1)
			lp->reg[reg].u8 = lp->reg[reg].d64;
		else if (len2 == 2)
			lp->reg[reg].u16 = lp->reg[reg].d64;
		else
			lp->reg[reg].u32 = lp->reg[reg].d64;
	}
	return 0;
}
static uint32_t cmd_39(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7;
	uint8_t len1 = tlen[(*lp->glb_psc >> 3) & 0x3];
	uint8_t len2 = tlen[*lp->glb_psc >> 5];
	lp->glb_psc++;
	if (len1 == 1)
	{
		if (len2 == 1)
			lp->reg[reg].s8 = lp->reg[reg].f32;
		else if (len2 == 2)
			lp->reg[reg].s16 = lp->reg[reg].f32;
		else
			lp->reg[reg].s32 = lp->reg[reg].f32;
	}
	else
	{
		if (len2 == 1)
			lp->reg[reg].s8 = lp->reg[reg].d64;
		else if (len2 == 2)
			lp->reg[reg].s16 = lp->reg[reg].d64;
		else
			lp->reg[reg].s32 = lp->reg[reg].d64;
	}
	return 0;
}

static uint32_t cmd_40(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7;
	uint8_t len = tlen[(*lp->glb_psc >> 3) & 0x3], i;
	uint8_t len2 = tlen[*lp->glb_psc >> 5];
	lp->glb_psc++;
	for (i = len; i < len2; i++)
		lp->reg[reg].data[i] = 0;
	return 0;
}
static uint32_t cmd_41(losc_t *lp)
{
	return 16;
}

static uint32_t cmd_42(losc_t *lp)
{
	return 0;
}
static uint32_t cmd_50(losc_t *lp)
{
	uint8_t addr = *lp->glb_psc++;
	lp->glb_psc += addr;
	return 0;
}
static uint32_t cmd_51(losc_t *lp)
{
	uint16_t addr = lp->glb_psc[0] | lp->glb_psc[1] << 8;
	lp->glb_psc += 2;
	lp->glb_psc += addr;
	return 0;
}
static uint32_t cmd_52(losc_t *lp)
{
	uint32_t addr = lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16;
	lp->glb_psc += 3;
	lp->glb_psc += addr;
	return 0;
}
static uint32_t cmd_53(losc_t *lp)
{
	uint8_t addr = *lp->glb_psc++;
	lp->glb_psc -= addr;
	return 0;
}
static uint32_t cmd_54(losc_t *lp)
{
	uint16_t addr = lp->glb_psc[0] | lp->glb_psc[1] << 8;
	lp->glb_psc += 2;
	lp->glb_psc = lp->glb_psc - addr;
	return 0;
}
static uint32_t cmd_55(losc_t *lp)
{
	uint32_t addr = lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16;
	lp->glb_psc += 3;
	lp->glb_psc = lp->glb_psc - addr;
	return 0;
}
static uint32_t cmd_47(losc_t *lp)
{
	uint8_t reg1 = *lp->glb_psc & 0x7;
	uint8_t reg2 = *lp->glb_psc >> 3;
	uint8_t addr = 0;
	lp->glb_psc++;
	addr = *lp->glb_psc++;
	if (lp->reg[reg1].u64 == lp->reg[reg2].u64)
		lp->glb_psc = lp->glb_psc + addr;
	return 0;
}
static uint32_t cmd_48(losc_t *lp)
{
	uint8_t reg1 = *lp->glb_psc & 0x7;
	uint8_t reg2 = *lp->glb_psc >> 3;
	uint16_t addr = 0;
	addr = lp->glb_psc[1] | lp->glb_psc[2] << 8;
	lp->glb_psc += 3;
	if (lp->reg[reg1].u64 == lp->reg[reg2].u64)
		lp->glb_psc = lp->glb_psc + addr;
	return 0;
}
static uint32_t cmd_49(losc_t *lp)
{
	uint8_t reg1 = *lp->glb_psc & 0x7;
	uint8_t reg2 = *lp->glb_psc >> 3;
	uint32_t addr = 0;
	addr = lp->glb_psc[1] | lp->glb_psc[2] << 8 | lp->glb_psc[3] << 16;
	lp->glb_psc += 4;
	if (lp->reg[reg1].u64 == lp->reg[reg2].u64)
		lp->glb_psc = lp->glb_psc + addr;
	return 0;
}
static uint32_t cmd_45(losc_t *lp)
{
	uint8_t reg1 = *lp->glb_psc & 0x7, i;
	uint8_t len = *lp->glb_psc >> 3;
	uint32_t addr = 0;
	lp->glb_psc++;
	for (i = 0; i < len; i++)
	{
		addr |= (*lp->glb_psc) << (i * 8);
		lp->glb_psc++;
	}
	if (lp->reg[reg1].u64 == 0)
		lp->glb_psc += addr;
	return 0;
}
static uint32_t cmd_46(losc_t *lp)
{
	uint8_t reg1 = *lp->glb_psc & 0x7, i;
	uint8_t len = *lp->glb_psc >> 3;
	uint32_t addr = 0;
	lp->glb_psc++;
	for (i = 0; i < len; i++)
	{
		addr |= (*lp->glb_psc) << (i * 8);
		lp->glb_psc++;
	}
	if (lp->reg[reg1].u64 == 0)
		lp->glb_psc = lp->glb_psc - addr;
	return 0;
}
static uint32_t cmd_43(losc_t *lp)
{
	uint8_t reg1 = *lp->glb_psc & 0x7, i; //reg
	uint8_t len = *lp->glb_psc >> 3;
	uint32_t addr = 0;
	lp->glb_psc++;
	for (i = 0; i < len; i++)
	{
		addr |= (*lp->glb_psc) << (i * 8);
		lp->glb_psc++;
	}
	if (lp->reg[reg1].u64)
		lp->glb_psc += addr;
	return 0;
}
static uint32_t cmd_44(losc_t *lp)
{
	uint8_t reg1 = *lp->glb_psc & 0x7, i; //reg
	uint8_t len = *lp->glb_psc >> 3;
	uint32_t addr = 0;
	lp->glb_psc++;
	for (i = 0; i < len; i++)
	{
		addr |= (*lp->glb_psc) << (i * 8);
		lp->glb_psc++;
	}
	if (lp->reg[reg1].u64)
		lp->glb_psc = lp->glb_psc - addr;
	return 0;
}

static uint32_t cmd_64(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7;
	uint8_t len = *lp->glb_psc >> 3, j;
	uint32_t addr = 0;
	lp->glb_psc++;
	for (j = 0; j < len; ++j)
	{
		addr |= (*lp->glb_psc) << (j * 8);
		lp->glb_psc++;
	}
	lp->reg[reg].u32 = lp->num - addr;
	return 0;
}

static uint32_t cmd_65(losc_t *lp)
{
	uint8_t id = *lp->glb_psc & 0x7;
	uint8_t len = *lp->glb_psc >> 5, j;
	uint32_t addr = 0;
	lp->glb_psc++;
	for (j = 0; j < len; ++j)
	{
		addr |= (*lp->glb_psc) << (j * 8);
		lp->glb_psc++;
	}
	lp->reg[id].u32 = addr;
	return 0;
}
static uint32_t cmd_66(losc_t *lp)
{
	uint8_t id = *lp->glb_psc & 0x7;
	uint8_t len = tlen[(*lp->glb_psc >> 3) & 0x3], i, j;
	uint32_t addr = 0;
	i = *lp->glb_psc >> 5;
	lp->glb_psc++;
	for (j = 0; j < i; ++j)
	{
		addr |= (*lp->glb_psc) << (j * 8);
		lp->glb_psc++;
	}
	for (i = 0; i < len; ++i)
		lp->reg[id].data[i] = lp->ram[addr + i];
	return 0;
}
static uint32_t cmd_67(losc_t *lp)
{
	uint8_t id = *lp->glb_psc & 0x7;
	uint8_t len = tlen[(*lp->glb_psc >> 3) & 0x3], i, j;
	uint32_t addr = 0;
	i = *lp->glb_psc >> 5;
	lp->glb_psc++;
	for (j = 0; j < i; ++j)
	{
		addr |= (*lp->glb_psc) << (j * 8);
		lp->glb_psc++;
	}
	for (i = 0; i < len; ++i)
		lp->ram[addr + i] = lp->reg[id].data[i];
	return 0;
}

static uint32_t cmd_56(losc_t *lp)
{
	uint8_t reg, reg1, reg2;
	reg = *lp->glb_psc & 0x3;
	reg1 = (*lp->glb_psc >> 2) & 0x7;
	reg2 = (*lp->glb_psc >> 5);
	if (lp->reg[reg].u32)
	{
		lp->reg[reg].u32 = lp->reg[reg1].u32;
	}
	else
		lp->reg[reg].u32 = lp->reg[reg2].u32;
	lp->glb_psc++;
	return 0;
}
static uint32_t cmd_57(losc_t *lp)
{
	uint8_t reg, reg1, reg2;
	reg = (*lp->glb_psc & 0x3) + 4;
	reg1 = (*lp->glb_psc >> 2) & 0x7;
	reg2 = (*lp->glb_psc >> 5);
	if (lp->reg[reg].u32)
		lp->reg[reg].u32 = lp->reg[reg1].u32;
	else
		lp->reg[reg].u32 = lp->reg[reg2].u32;
	lp->glb_psc++;
	return 0;
}

static uint32_t cmd_69(losc_t *lp)
{
	uint8_t *buf, i, len, reg1, reg2;
	reg1 = *lp->glb_psc & 0x7;
	reg2 = (*lp->glb_psc >> 3) & 0x7;
	len = tlen[*lp->glb_psc >> 6];
	lp->glb_psc++;
	if ((lp->ram_len + lp->lram.size) < lp->reg[reg1].u32)
		return 9;
	buf = lp->ram + lp->reg[reg1].u32;
	for (i = 0; i < len; ++i)
		buf[i] = lp->reg[reg2].data[i];
	return 0;
}

static uint32_t cmd_68(losc_t *lp)
{
	uint8_t *data;
	uint8_t id = *lp->glb_psc & 0x7;
	uint8_t reg = (*lp->glb_psc >> 3) & 0x7, len, i;
	len = tlen[*lp->glb_psc >> 6];
	if (lp->reg[reg].u32 > (lp->ram_len + lp->lram.size))
		return 13;
	data = lp->ram + lp->reg[reg].u32;
	lp->reg[id].u64 = 0;
	lp->glb_psc++;
	for (i = 0; i < len; i++)
		lp->reg[id].data[i] = data[i];
	return 0;
}

static uint32_t cmd_70(losc_t *lp)
{
	uint32_t i, tlen = 0;
	uint8_t id = *lp->glb_psc & 0x7;
	uint8_t reg = (*lp->glb_psc >> 3) & 0x3, len, *addr, *data;
	len = *lp->glb_psc >> 6;
	lp->glb_psc++;
	addr = lp->ram + lp->reg[id].u32;
	data = lp->ram + lp->reg[reg].u32;
	for (i = 0; i < len; ++i)
	{
		tlen |= (*lp->glb_psc) << (i * 8);
		lp->glb_psc++;
	}
	for (i = 0; i < tlen; ++i)
		addr[i] = data[i];
	return 0;
}
uint32_t cmd_32(losc_t *lp)
{
	uint8_t id = *lp->glb_psc++;
	los_sys_apis[id](lp);
	if (id > 0)
	{
		lp->num -= lp->push_num;
		lp->push_num = 0;
	}
	return 0;
}
const los_cmd los_cmd_api[] =
	{
		cmd_1,
		cmd_2,
		cmd_3,
		cmd_4,
		cmd_5,
		cmd_6,
		cmd_7,
		cmd_8,
		cmd_9,
		cmd_10,
		cmd_11,
		cmd_12,
		cmd_13,
		cmd_14,
		cmd_15,
		cmd_16,
		cmd_17,
		cmd_18,
		cmd_19,
		cmd_20,
		cmd_21,
		cmd_22,
		cmd_23,
		cmd_24,
		cmd_25,
		cmd_26,
		cmd_27,
		cmd_28,
		cmd_29,
		cmd_30,
		cmd_31,
		cmd_32,
		cmd_33,
		cmd_34,
		cmd_35,
		cmd_35,
		cmd_36,
		cmd_37,
		cmd_38,
		cmd_39,
		cmd_40,
		cmd_41,
		cmd_42,
		cmd_43,
		cmd_44,
		cmd_45,
		cmd_46,
		cmd_47,
		cmd_48,
		cmd_49,
		cmd_50,
		cmd_51,
		cmd_52,
		cmd_53,
		cmd_54,
		cmd_55,
		cmd_56,
		cmd_57,
		cmd_58,
		cmd_59,
		cmd_60,
		cmd_61,
		cmd_62,
		cmd_63,
		cmd_64,
		cmd_65,
		cmd_66,
		cmd_67,
		cmd_68,
		cmd_69,
		cmd_70,
};

uint32_t los_call_obj_event(losc_t *lp, uint32_t obj, uint32_t e,
							uint32_t addr)
{
	lp->ram[lp->num++] = e;
	lp->ram[lp->num++] = e >> 8;
	lp->ram[lp->num++] = e >> 16;
	lp->ram[lp->num++] = e >> 24;
	lp->ram[lp->num++] = obj;
	lp->ram[lp->num++] = obj >> 8;
	lp->ram[lp->num++] = obj >> 16;
	lp->ram[lp->num++] = obj >> 24;
	return cmd_addr_num(lp, addr);
}
uint32_t los_call_msg_event(losc_t *lp, uint32_t arg3, uint32_t arg2,
							uint32_t arg1)
{
	lp->ram[lp->num++] = arg1;
	lp->ram[lp->num++] = arg1 >> 8;
	lp->ram[lp->num++] = arg1 >> 16;
	lp->ram[lp->num++] = arg1 >> 24;
	lp->ram[lp->num++] = arg2;
	lp->ram[lp->num++] = arg2 >> 8;
	lp->ram[lp->num++] = arg2 >> 16;
	lp->ram[lp->num++] = arg2 >> 24;
	lp->ram[lp->num++] = arg3;
	lp->ram[lp->num++] = arg3 >> 8;
	lp->ram[lp->num++] = arg3 >> 16;
	lp->ram[lp->num++] = arg3 >> 24;
	return cmd_addr_num(lp, lp->msg_addr);
}
extern uint32_t los_thread_change(losc_t *lp, thread_t *thread_next);
uint32_t los_check_flg(losc_t *lp)
{
	thread_t *thread_next;
	if (lp->os_flag & THREAD_FLG_10_CHANGE)
	{
		while (1)
		{
			thread_next = thread_list_find_run(lp->thread_now);
			if (thread_next)
			{
				if (thread_next == lp->thread_now)
					return 0;
				los_thread_change(lp, thread_next);
				return 0;
			}
			else
				los_delayms(10);
		}
	}
	return 0;
}

void los_set_path(losc_t *lp, uint8_t *app_name)
{
	uint8_t i, len;
	lp->path = app_name;
	len = strlen((const char *)app_name);
	if (len == 0)
		return;
	for (i = len - 1; i > 1; i--)
	{
		if ((app_name[i] == '/') || (app_name[i] == '\\'))
		{
			i++;
			lp->app_path_len = i;
			return;
		}
	}
}
uint32_t los_wsize(losc_t *lp, uint16_t w, uint16_t h)
{
	lp->lcd_h = h;
	lp->lcd_w = w;
	return 0;
}
uint32_t los_get_wsize(losc_t *lp, uint16_t *w, uint16_t *h)
{
	*h = lp->lcd_h;
	*w = lp->lcd_w;
	return 0;
}
uint32_t los_rerun_no_ui(losc_t *lp)
{
	uint8_t op = 0, i, res = 0;
	for (i = 0; i < 100; i++)
	{
		if ((lp->os_flag & 0XBF))
		{
			if (lp->os_flag & THREAD_FLG_80_EXIT)
				return 0;
			if ((lp->os_flag & THREAD_FLG_40_IO) == 0)
				los_check_flg(lp);
		}
		if (lp->thread == 0)
		{
			los_delayms(10);
			return 0;
		}
		if (lp->thread_num_new == 0)
		{
			lp->os_flag |= THREAD_FLG_10_CHANGE;
			return 0;
		}
		op = *lp->glb_psc;
		lp->glb_psc++;
		res = los_cmd_api[op](lp);
		if (res)
			break;
	}
	return res;
}
uint32_t los_rerun(losc_t *lp)
{
	uint32_t res = 0;
	uint8_t op = 0;
	lp->os_flag &= 0x7F;
	while (1)
	{
		if ((lp->os_flag & 0XBF))
		{
			if (lp->os_flag & THREAD_FLG_80_EXIT)
				return 0;
			if ((lp->os_flag & THREAD_FLG_40_IO) == 0)
				los_check_flg(lp);
		}
		op = *lp->glb_psc;
		lp->glb_psc++;
		if (op > ARRLENs(los_cmd_api))
		{
			res = 13;
			break;
		}
		res = los_cmd_api[op](lp);
		if (res)
		{
			res |= 0x10;
			break;
		}
	}
	return res;
}

#ifdef WINDOW
extern uint8_t pc_los_path[512];
#endif
char *deal_file_path(losc_t *lp, char *path)
{
	uint16_t len = 0;
	if (path[0] == '/')
	{
#ifdef WINDOW
		sprintf(lp->filepath, "%s%s", pc_los_path, path);
#else
		return path;
#endif
	}
	else
	{
		memcpy(lp->filepath, lp->path, lp->app_path_len);
		sprintf(&lp->filepath[lp->app_path_len], "%s", path);
	}
	return lp->filepath;
}
////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void los_set_use_api(const fun_u *use_api)
{
	ker_use_api = (fun_u *)use_api;
}
void los_set_lgui_api(const fun_u *lgui_api)
{
	ker_lgui_api = (fun_u *)lgui_api;
}
//////////////////////////////////////////////////////
/////////////////////////////////////////////////////
uint8_t los_get_u8(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (uint8_t)lp->ram[len];
}
int8_t los_get_s8(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (int8_t)lp->ram[len];
}
uint16_t los_get_u16(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return lp->ram[len] | lp->ram[len + 1] << 8;
}
int16_t los_get_s16(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return lp->ram[len] | lp->ram[len + 1] << 8;
}
uint32_t los_get_u32(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24;
}
int32_t los_get_s32(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24;
}
void *los_get_voidp(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (void *)((uint32_t)lp->ram + (lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24));
}

void *los_get_p(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	uint32_t addr = 0;
	len = lp->num - num * 4;
	addr = lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24;
	return (void *)&lp->ram[addr];
}
uint8_t *los_get_u8p(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (uint8_t *)((uint32_t)lp->ram + (lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24));
}

int8_t *los_get_s8p(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (int8_t *)((uint32_t)lp->ram + (lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24));
}
int16_t *los_get_s16p(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (int16_t *)((uint32_t)lp->ram + (lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24));
}
uint16_t *los_get_u16p(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (uint16_t *)((uint32_t)lp->ram + (lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24));
}
uint32_t *los_get_u32p(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (uint32_t *)((uint32_t)lp->ram + (lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24));
}
int32_t *los_get_s32p(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (int32_t *)((uint32_t)lp->ram + (lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24));
}
float *los_get_floatp(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (float *)((uint32_t)lp->ram + (lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24));
}
uint64_t *los_get_u64p(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (uint64_t *)((uint32_t)lp->ram + (lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24));
}
int64_t *los_get_s64p(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (int64_t *)((uint32_t)lp->ram + (lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24));
}
double *los_get_doublep(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (double *)((uint32_t)lp->ram + (lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24));
}

float los_get_float(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (float)(lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24);
}
uint64_t los_get_u64(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (uint64_t)(lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24 | lp->ram[len + 4] << 24 | lp->ram[len + 5] << 24 | lp->ram[len + 6] << 24 | lp->ram[len + 7] << 24);
}
int64_t los_get_s64(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (int64_t)(lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24 | lp->ram[len + 4] << 24 | lp->ram[len + 5] << 24 | lp->ram[len + 6] << 24 | lp->ram[len + 7] << 24);
}
double los_get_double(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (double)(lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24 | lp->ram[len + 4] << 24 | lp->ram[len + 5] << 24 | lp->ram[len + 6] << 24 | lp->ram[len + 7] << 24);
}

void los_return(losc_t *lp, int32_t ret)
{
	lp->reg[REG_NUM - 1].s32 = ret;
}
void los_return_8b(losc_t *lp, int64_t ret)
{
	lp->reg[REG_NUM - 1].s64 = ret;
}
void los_return_float(losc_t *lp, float ret)
{
	lp->reg[REG_NUM - 1].f32 = ret;
}
void los_return_double(losc_t *lp, double ret)
{
	lp->reg[REG_NUM - 1].d64 = ret;
}
uint32_t los_tick(losc_t *lp)
{
	thread_t *thread_next = 0;
	if (lp->thread)
	{
		thread_next = thread_list_time_run(lp->thread_now);
		if (lp->os_flag & THREAD_FLG_40_IO)
			return 10;
		if (thread_next)
		{
			if (thread_next != lp->thread_now)
				lp->os_flag |= THREAD_FLG_10_CHANGE;
		}
		else
			lp->os_flag |= THREAD_FLG_10_CHANGE;
	}
	else
		return 1;
	return 0;
}
int check_app_type(uint8_t *fname)
{
	uint8_t i = 0, len, flg;
	len = strlen(fname) - 1;
	for (i = len; i > 0; i--)
		if (fname[i] == '@')
			return atoi(&fname[i + 1]);
	return 0;
}
static uint8_t los_init_fat(losc_t *lp, uint8_t *fname)
{
	int ret;
	lfile_t *file;
	uint8_t buf[32];
	uint32_t tlen, stack, heap;
	uint16_t i;
	file = lf_open((const char *)fname, "rb");
	if (file == NULL)
		return 1;
	ret = lf_read(file, buf, 7);
	if (memcmp(buf, "los", 3))
		return 0xff;
	if (los_api_vs < (buf[3] | buf[4] << 8))
		return 0xf2;
	i = buf[5] | buf[6] << 8;
	if (i > 0)
		ret = lf_read(file, buf, i);
	ret = lf_read(file, buf, sizeof(buf));
	lp->lvar_start = buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;
	lp->txt_len = buf[4] | buf[5] << 8 | buf[6] << 16 | buf[7] << 24;
	tlen = buf[8] | buf[9] << 8 | buf[10] << 16 | buf[11] << 24;
	lp->irq_addr = buf[12] | buf[13] << 8 | buf[14] << 16 | buf[15] << 24;
	stack = (buf[16] | buf[17] << 8 | buf[18] << 16) * 4;
	heap = (buf[19] | buf[20] << 8 | buf[21] << 16 | buf[22] << 24) * 4;
	lp->code_len = (buf[23] | buf[24] << 8 | buf[25] << 16 | buf[26] << 24);
	lp->lvar_start = lp->lvar_start + lp->txt_len;
	lp->num = lp->lvar_start;
	lp->end = lp->num + stack;
	lp->ram_len = lp->end;
	if (lp->end % 4)
		lp->end += 4 - lp->end % 4;
	lp->ram = lpram_malloc(lp->end + heap);
	if (lp->ram == 0)
		return 7;
	memset(lp->ram, 0, lp->end + heap);
	lram_init(&lp->lram, &lp->ram[lp->end], heap);
	lp->lram.offest = (uint32_t)lp->ram;

	lp->end--;
	tlen += lp->txt_len;
	if (tlen)
		ret = lf_read(file, lp->ram, tlen);
	i = los_init_flg(lp, buf[27]);
	if (i)
		return 4;

	lp->code = lpram_malloc(lp->code_len);
	if (lp->code == 0)
		return 9;
	ret = lf_read(file, lp->code, lp->code_len);
	lp->glb_psc = lp->code; //code
	return lf_close(file);
}

static uint8_t los_init_code(losc_t *lp, uint8_t *buf)
{
	int ret;
	uint32_t tlen, stack, heap;
	uint32_t i;
	if (memcmp(buf, "los", 3))
		return 0xff;
	if (los_api_vs < (buf[3] | buf[4] << 8))
		return 0xf2;
	i = buf[5] | buf[6] << 8;
	if (i > 0)
		buf += i;
	buf += 7;
	lp->lvar_start = buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;
	lp->txt_len = buf[4] | buf[5] << 8 | buf[6] << 16 | buf[7] << 24;
	tlen = buf[8] | buf[9] << 8 | buf[10] << 16 | buf[11] << 24;
	lp->irq_addr = buf[12] | buf[13] << 8 | buf[14] << 16 | buf[15] << 24;
	stack = (buf[16] | buf[17] << 8 | buf[18] << 16) * 4;
	heap = (buf[19] | buf[20] << 8 | buf[21] << 16 | buf[22] << 24) * 4;
	lp->code_len = (buf[23] | buf[24] << 8 | buf[25] << 16 | buf[26] << 24);
	buf += 32;
	lp->lvar_start = lp->lvar_start + lp->txt_len;
	lp->num = lp->lvar_start;
	lp->end = lp->num + stack;
	lp->ram_len = lp->end;
	if (lp->end % 4)
		lp->end += 4 - lp->end % 4;
	lp->ram = lpram_malloc(lp->end + heap);
	if (lp->ram == 0)
		return 7;
	memset(lp->ram, 0, lp->end + heap);
	lram_init(&lp->lram, &lp->ram[lp->end], heap);
	lp->lram.offest = (uint32_t)lp->ram;

	lp->end--;
	tlen += lp->txt_len;
	if (tlen)
	{
		memcpy(lp->ram, buf, tlen);
		buf += tlen;
	}
	i = los_init_flg(lp, buf[27]);
	if (i)
		return 4;
	lp->code = buf;
	lp->glb_psc = lp->code; //code
	return 0;
}
//--------------------------------------------------
//los_run_flash
//--------------------------------------------------
uint32_t los_run_fanme(losc_t *lp, uint8_t *fanme)
{
	uint32_t res = 0;
	lp->os_flag = 0;
	res = check_app_type(fanme);
	if (res)
	{
		res = los_init_code(lp, res);
	}
	else
	{
		res = los_init_fat(lp, (uint8_t *)fanme);
	}
	if (res)
		return res | 0x100;
	los_get_windowsize_los(&lp->lcd_w, &lp->lcd_h);
	return res;
}

static void *losmalloc(void *los, uint32_t size)
{
	losc_t *lp = (losc_t *)los;
	return (void *)((uint32_t)lmalloc(&lp->lram, size) - lp->lram.offest);
}
static void *loscalloc(void *los, uint32_t size)
{
	losc_t *lp = (losc_t *)los;
	return lmalloc(&lp->lram, size);
}
static void losfree(void *los, void *ram)
{
	losc_t *lp = (losc_t *)los;
	lfree(&lp->lram, ram);
}
static void *losrealloc(void *los, void *ram, uint32_t size)
{
	losc_t *lp = (losc_t *)los;
	return lrealloc(&lp->lram, ram, size);
}
///////////////////////////////////////////////////////////////////////////
////////////////////////////clib//////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
uint32_t malloc_los(losc_t *los)
{
	los_return(los, (uint32_t)losmalloc(los, los_get_u32(los, 1)));
	return 0;
}
uint32_t free_los(losc_t *los)
{
	losfree(los, (void *)((uint32_t)los_get_voidp(los, 1)));
	return 0;
}
uint32_t realloc_los(losc_t *los)
{
	los_return(los,
			   (int32_t)losrealloc(los, los_get_voidp(los, 1),
								   los_get_u32(los, 2)));
	return 0;
}
uint32_t calloc_los(losc_t *los)
{
	los_return(los, (int32_t)loscalloc(los, los_get_u32(los, 1)));
	return 0;
}
uint32_t ram_free_los(losc_t *los)
{
	los_return(los, lfree_size(&los->lram));
	return 0;
}
uint32_t ram_max_los(losc_t *los)
{
	los_return(los, los->lram.min);
	return 0;
}
uint32_t memset_los(losc_t *los)
{
	los_return(los,
			   (int)memset((void *)los_get_voidp(los, 1), (int)los_get_s32(los, 2),
						   los_get_u32(los, 3)));
	return 0;
}
uint32_t memcpy_los(losc_t *los)
{
	los_return(los,
			   (int)memcpy(los_get_p(los, 1), (const char *)los_get_voidp(los, 2),
						   los_get_u32(los, 3)));
	return 0;
}
uint32_t memcmp_los(losc_t *los)
{
	los_return(los,
			   (int)memcmp(los_get_voidp(los, 1), los_get_voidp(los, 2),
						   los_get_u32(los, 3)));
	return 0;
}
uint32_t strlen_los(losc_t *los)
{
	los_return(los, strlen(los_get_voidp(los, 1)));
	return 0;
}
uint32_t strcpy_los(losc_t *los)
{
	los_return(los, (int)strcpy((char *)los_get_p(los, 1), (const char *)los_get_voidp(los, 2)));
	return 0;
}
uint32_t strcmp_los(losc_t *los)
{
	los_return(los, strcmp(los_get_voidp(los, 1), los_get_voidp(los, 2)));
	return 0;
}

//--------------------------------------------------
////los ram
//-------------------------- ------------------------
void lram_init(lram_t *lram, uint8_t *ram, uint32_t num)
{
	if (num == 0)
		return;
	memset(ram, 0, num);
	memset(lram, 0, sizeof(lram_t));
	lram->ram = ram;
	lram->size = num;
	lram->min = num;
	lram->now = num;
}
void *lrealloc(lram_t *lram, void *ram, uint32_t size)
{
	//no done
	return 0;
}
void *lmalloc(lram_t *lram, uint32_t size)
{
	uint16_t i = 0, num = 0;
	uint32_t addr = 0, len = 0;
	uint8_t *ram = lram->ram;
	num = 1 + lram->bnum;
	i = size % 4;
	if (i != 0)
		size += 4 - i;
	for (i = 0; i < num; i++)
	{
		if (ram[addr] == 1)
		{
			len = ram[addr + 1] | (ram[addr + 2] << 8) | (ram[addr + 3] << 16);
			if (len == size)
			{
				lram->now = lram->now - size - 4;
				ram[addr] = 2;
				return (void *)(lram->ram + addr + 4);
			}
			else if (len > (size + 7))
			{
				lram->now = lram->now - size - 4;
				if (lram->now < lram->min)
					lram->min = lram->now;
				ram[addr] = 2;
				ram[addr + 1] = size;
				ram[addr + 2] = size >> 8;
				ram[addr + 3] = size >> 16;
				addr += 4;
				len = len - size - 4;
				size += addr;
				ram[size] = 1;
				ram[1 + size] = len;
				ram[2 + size] = len >> 8;
				ram[3 + size] = len >> 16;
				lram->bnum++;
				return (void *)(lram->ram + addr);
			}
			addr += 4 + len;
		}
		else if (ram[addr] == 2)
			addr += 4 + ram[addr + 1] | (ram[addr + 2] << 8) | (ram[addr + 3] << 16);
		else
		{
			if ((addr + 4 + size) < lram->size)
			{
				ram[addr] = 2;
				ram[1 + addr] = size;
				ram[2 + addr] = size >> 8;
				ram[3 + addr] = size >> 16;
				lram->now = lram->now - size - 4;
				if (lram->now < lram->min)
					lram->min = lram->now;
				lram->bnum++;
				return (void *)(lram->ram + addr + 4);
			}
			else
				return 0;
		}
	}
	return 0;
}
//--------------------------------------------------
//free
//--------------------------------------------------
uint32_t lfree(lram_t *lram, void *fram)
{
	uint32_t addr = 0, size, len;
	uint8_t *ram = lram->ram;
	addr = (uint32_t)fram - (uint32_t)lram->ram;
	if (addr > lram->size)
		return 1;
	if (ram[addr - 4] == 2)
	{
		size = ram[addr - 3] | (ram[addr - 2] << 8) | (ram[addr - 1] << 16);
		lram->now += size + 4;
		if (ram[addr + size] == 2)
		{
			ram[addr - 4] = 1;
		}
		else if (ram[addr + size] == 1)
		{
			ram[addr - 4] = 1;
			len = ram[addr + size + 1] | (ram[addr + size + 2] << 8) | (ram[addr + size + 3] << 16);
			len += 4;
			size += len;
			ram[addr - 3] = size;
			ram[addr - 2] = size >> 8;
			ram[addr - 1] = size >> 16;
			lram->bnum--;
		}
		else
		{
			lram->bnum--;
			ram[addr - 4] = 0;
		}
		return 0;
	}
	return 1;
}
uint32_t lfree_size(lram_t *lram)
{
	return lram->now;
}
uint32_t lfree_max(lram_t *lram)
{
	return lram->min;
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
#ifdef APP_THREAD
thread_t *thread_list_new(uint16_t name_num)
{
	thread_t *p_list = 0;
	p_list = (thread_t *)lpram_malloc(sizeof(thread_t));
	if (p_list == 0)
		return 0;
	p_list->next = p_list;
	p_list->name_num = name_num;
	return p_list;
}
thread_t *thread_list_find(thread_t *phead, uint16_t name_num)
{
	thread_t *p = phead->next;
	while (p != phead)
	{
		if (p->name_num == name_num)
		{
			return p;
		}
		p = p->next;
	}
	if (p->name_num == name_num)
	{
		return p;
	}
	return 0;
}

thread_t *thread_list_find_run(thread_t *phead)
{
	thread_t *p = phead->next;
	while (p != phead)
	{
		if ((p->run & 0x80) && (p->delay == 0))
		{
			return p;
		}
		p = p->next;
	}
	if ((phead->run & 0x80) && (phead->delay == 0))
		return phead;
	return 0;
}
thread_t *thread_list_time_run(thread_t *phead)
{
	thread_t *next = 0;
	thread_t *p = phead->next;
	if (phead->delay)
		phead->delay--;
	while (p != phead)
	{
		if (p->run & 0x80)
		{
			if (p->delay)
				p->delay--;
			if (p->delay == 0 && next == 0)
				next = p;
		}
		p = p->next;
	}
	if ((next == 0))
	{
		if (phead->delay)
			return 0;
		else
			return phead;
	}
	return next;
}
thread_t *thread_list_add(thread_t *L, uint16_t name_num)
{
	thread_t *s = 0;
	s = (thread_t *)lpram_malloc(sizeof(thread_t));
	if (s == 0)
		return 0;
	s->name_num = name_num;
	s->next = L->next;
	L->next = s;
	return s;
}
uint32_t thread_list_del(thread_t *phead)
{
	thread_t *p;
	thread_t *s = 0;
	if (0 == phead)
		return 0;
	p = phead->next;
	while (p != phead)
	{
		s = p;
		lpram_free(p->ram);
		p = p->next;
		lpram_free(s);
	}
	lpram_free(p->ram);
	lpram_free(p);
	return 0;
}
uint32_t thread_list_del_one(thread_t *phead, uint8_t id)
{
	thread_t *p1;
	thread_t *p2 = 0;
	if (phead == 0)
		return 1;
	p1 = phead;
	while (p1->name_num != id && p1->next != 0)
	{
		p2 = p1;
		p1 = p1->next;
	}
	if (p1->name_num == id)
	{
		if (p1 == phead)
			phead = p1->next;
		else
			p2->next = p1->next;
		lpram_free(p1->ram);
		lpram_free(p1);
	}
	else
		return 1;
	return 0;
}
#endif
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
los_app_t *los_app_list_new(uint8_t *pname)
{
	los_app_t *p_list = 0;
	p_list = (los_app_t *)lpram_malloc(sizeof(los_app_t));
	if (p_list == 0)
		return 0;
	p_list->path = (uint8_t *)lpram_malloc(strlen((const char *)pname) + 1);
	if (p_list->path == 0)
		return 0;
	strcpy((char *)p_list->path, (const char *)pname);
	p_list->next = p_list;
	p_list->type = 0;
	p_list->run = 4;
	return p_list;
}
losc_t *los_app_list_finded(los_app_t *phead)
{
	los_app_t *p = phead->next;
	while (p != phead)
	{
		if (p->run == 5)
			return p->los;
		p = p->next;
	}
	return 0;
}

los_app_t *los_app_list_find(los_app_t *phead, losc_t *los_old)
{
	los_app_t *p = phead->next;
	while (p != phead)
	{
		if (p->run == 5)
			return p;
		p = p->next;
	}
	return 0;
}

losc_t *los_app_list_set_star(los_app_t *phead, losc_t *los_old, uint8_t star)
{
	los_app_t *p = phead->next;
	while (p != phead)
	{
		if (p->los == los_old) //找到了
		{
			p->run = star;
			return p->los;
		}
		p = p->next;
	}
	return 0;
}

los_app_t *los_app_list_find_path(los_app_t *phead, uint8_t *path)
{
	los_app_t *p = phead->next;
	while (p != phead)
	{
		if (0 == strcmp((const char *)p->path, (const char *)path))
			return p;
		p = p->next;
	}
	return 0;
}

los_app_t *los_app_list_add(los_app_t *L, uint8_t *pname)
{
	los_app_t *s = 0;
	s = (los_app_t *)lpram_malloc(sizeof(los_app_t));
	if (s == 0)
		return 0;
	s->path = (uint8_t *)lpram_malloc(strlen((const char *)pname) + 1);
	if (s->path == 0)
		return 0;
	strcpy((char *)s->path, (const char *)pname);
	s->next = L->next;
	L->next = s;
	s->type = 1;
	s->run = 5;
	return s;
}

uint32_t los_app_list_del(los_app_t *phead)
{
	los_app_t *p;
	los_app_t *s = 0;
	if (0 == phead)
		return 0;
	p = phead->next;
	while (p != phead)
	{
		s = p;
		lpram_free(p->path);
		p = p->next;
		lpram_free(s);
	}
	lpram_free(p->path);
	lpram_free(p);
	return 0;
}

uint32_t los_app_list_del_one(los_app_t *phead, losc_t *los_old)
{
	los_app_t *p1;
	los_app_t *p2 = 0;
	if (phead == 0)
		return 1;
	p1 = phead;
	while (p1->los != los_old && p1->next != 0)
	{
		p2 = p1;
		p1 = p1->next;
	}
	if (p1->los == los_old)
	{
		if (p1 == phead)
			phead = p1->next;
		else
			p2->next = p1->next;
		lpram_free(p1->path);
		lpram_free(p1);
	}
	else
		return 1;
	return 0;
}

uint32_t los_app_list_len(los_app_t *phead)
{
	uint32_t i = 1;
	los_app_t *p = phead->next;
	while (p != phead)
	{
		i++;
		p = p->next;
	}
	return i;
}
static uint8_t los_app_flg = 0;
extern int ft_init(void);
uint32_t los_app_check_status(losc_t *los);
static fun_u *los_lgui_api_cb = 0;
static fun_u *los_user_api_cb = 0;
void los_set_api(uint8_t type, fun_u *api)
{
	if (type == LOS_LGUI_API_TYPE)
		los_lgui_api_cb = api;
	else if (type == LOS_USER_API_TYPE)
		los_user_api_cb = api;
}
#ifdef WINDOW ///////////////////////////
#include "pc_ini.h"
uint8_t pc_los_path[512];
uint8_t get_pc_los_path(uint8_t *name)
{
	uint16_t ret = 0, len;
	ret = get_dirname(name, pc_los_path);
	ret = get_dirname(pc_los_path, pc_los_path);
	len = strlen(pc_los_path);
	pc_ini_init(pc_los_path);
	sprintf(&pc_los_path[len], "/losfat");
	return ret;
}
extern void window_set_los_title(char *name);
void los_window_set_title(char *name)
{
	int len;
	char lname[32] = "", lname2[80];
	get_fname(name, lname2);
	get_filename(lname2, lname);
	len = strlen(lname);
	sprintf(&lname[len], "%s", "-Win los");
	window_set_los_title(lname);
}
#endif ///window
los_app_t *los_apps;
losc_t *los_now = 0;
uint32_t los_app_frist(uint8_t *pname)
{
	int ret;
	los_app_t *los_app_new;
	losc_t *los_old;
	los_now = los_new();
	if (los_now == 0)
		return 10;

	los_set_lgui_api(los_lgui_api_cb);
	los_set_use_api(los_user_api_cb);

	los_apps = los_app_list_new(pname);
	if (los_apps == 0)
		return 3;
	los_apps->los = los_now;
	los_set_path(los_now, los_apps->path);
#ifdef LOS_LGUI_EN
	ft_init();
#endif
	ret = los_run_fanme(los_now, pname);
	if (ret)
		return ret;
#ifdef WINDOW
	los_window_set_title(pname);
#endif
	while (1)
	{
		ret = los_rerun(los_now);
		if (ret == 0)
			los_app_list_set_star(los_apps, los_now, 4);
		else
			return 1;

		usart_check_out_los(los_now);
		ret = los_app_check_status(los_now);
		if (ret)
		{
			los_old = los_now;
			los_now = los_app_list_find(los_apps, los_old);
			los_app_list_del_one(los_apps, los_old);
		}
		else
		{
			if (los_app_flg & 0x1)
			{
				los_now->lcd = los_get_lcd_buf();
				ret = los_now->lcd;
			}
			if (ret == 0)
				los_free_ram(los_now);
		}
		los_app_new = los_app_list_find(los_apps, los_now);
		if (los_app_new == 0)
			los_app_new = los_apps;
		los_now = los_app_new->los;
		ret = 0;
		if (los_app_flg & 0x1)
		{
#ifdef WINDOW
			los_window_set_title(los_app_new->path);
#endif
			ret = los_now->lcd;
			if (ret)
				los_pop_lcd_buf(los_now->lcd);
		}
		if (ret == 0)
			ret = los_run_fanme(los_now, los_now->path);
		else
			ret = 0;
		if (ret)
			return ret;
		if (los_now == 0)
			break;
	}
	return 0;
}
#ifdef WINDOW
uint32_t los_app_main(uint8_t type)
{
	uint32_t ret = 0;
	uint8_t *data, buf[128];
	if (type == 0)
	{
		sprintf(buf, "%s/a/app/los/los.los", pc_los_path);
		ret = los_app_frist(buf);
	}
	else
	{
		sprintf(buf, "%s/a/los/mlos", pc_los_path);
		data = los_fat_read_file(buf, &ret);
		if (data)
		{
			if (0 == memcmp(data, "los", 3))
			{
				sprintf(buf, "%s%s", pc_los_path, &data[4]);
				ret = los_app_frist(buf);
			}
			else if (0 == memcmp(data, "bin", 3))
			{
			}
			lpram_free(data);
		}
		else
		{
			sprintf(buf, "%s/a/app/los/los.los", pc_los_path);
			ret = los_app_frist(buf);
		}
	}
	return ret;
}

#else //mcu
uint32_t los_app_main(uint8_t type)
{
	uint32_t ret = 0;
	uint8_t *data;
	if (type == 0)
	{
		ret = los_app_frist("/a/app/los/los.los");
	}
	else
	{
		data = los_fat_read_file("/a/los/mlos", &ret);
		if (data)
		{
			if (0 == memcmp(data, "los", 3))
			{
				ret = los_app_frist(&data[4]);
			}
			else if (0 == memcmp(data, "bin", 3))
			{
			}
			lpram_free(data);
		}
		else
			ret = los_app_frist("/a/app/los/los.los");
	}
	return ret;
}
#endif

uint32_t los_app_check_status(losc_t *los)
{
	losc_t *lp = los;
	if (lp->os_flag & 0x40)
	{
		los_close(los);
		return 1;
	}
	return 0;
}
void los_app_min(void)
{
	if (los_app_list_len(los_apps) > 1)
		los_quit(los_now);
}
uint8_t los_app_tick(void)
{
	if (los_now)
		los_tick(los_now);
	else
		return 1;
	return 0;
}

int app_run(char *run_name, char *cmd)
{
	los_app_t *s = 0;
	int ret = 0;
	losc_t *lapp_now = 0;
#ifdef WINDOW //WINDOW
	uint8_t buf[128];
	sprintf(buf, "%s%s", pc_los_path, run_name);
#else
	uint8_t *buf = run_name;
#endif
	s = los_app_list_find_path(los_apps, buf);
	if (s == 0)
	{
		lapp_now = los_new();
		if (lapp_now == 0)
			return 10;
		s = los_app_list_add(los_apps, buf);
		if (s == 0)
			return 2;
		s->los = lapp_now;
		los_set_path(lapp_now, s->path);
	}
	else
		s->run = 5;
	los_quit(los_now);
	return 0;
}
#ifdef WINDOW
uint32_t los_appname_frist(uint8_t *app)
{
	uint8_t pname[256];
	sprintf(pname, "%s/a/app/%s/%s.los", pc_los_path, app, app);
	return los_app_frist(pname);
}
#endif
void *los_malloc(uint32_t size)
{
	void *p = lmalloc(&los_now->lram, size);
	return p;
}
void *los_get_ram(void *p)
{
	return (void *)((uint32_t)p - los_now->lram.offest);
}
void los_free(void *p)
{
	lfree(&los_now->lram, p);
}
void los_app_mini(uint8_t type)
{
	if (type)
		los_app_flg = 1;
	else
		los_app_flg &= 0xfe;
}
static lgui_t lguis = {-1};
lgui_t *los_get_io(void)
{
	return &lguis;
}
void check_msg_event(losc_t *lp)
{
	if (lp->msg_addr)
	{
		if (lguis.point_flgs > -1)
		{
			los_call_msg_event(lp, lguis.point_flgs, lguis.point_x, lguis.point_y);
			lguis.point_flgs = -1;
		}
		else if (lguis.key_key)
		{
		}
	}
}
extern uint32_t cmd_addr_num(losc_t *lp, uint32_t addr_now);
uint32_t los_thread_change(losc_t *lp, thread_t *thread_next)
{
	uint32_t res = 0, ram_len = 0;
	uint32_t *data;
	if (lp->thread_now->delay == 0)
		lp->thread_now->delay = lp->thread_now->reload;
	if (THREAD_TIME == thread_next->type)
	{
		thread_next->delay = thread_next->reload;
		ram_len = cmd_addr_num(lp, (uint32_t)thread_next->ram);
		if (ram_len == 0)
			lp->os_flag |= THREAD_FLG_40_IO;
		return ram_len;
	}
	else if ((THREAD_UI == thread_next->type) || (thread_next->type == THREAD_SERVER))
	{
		ram_len = lp->ram_len - lp->lvar_start;
		res = (uint32_t)lp->glb_psc;
		lp->thread_now->ram[0] = res;
		lp->thread_now->ram[1] = res >> 8;
		lp->thread_now->ram[2] = res >> 16;
		lp->thread_now->ram[3] = res >> 24;
		lp->thread_now->ram[4] = lp->push_num;
		memcpy(&lp->thread_now->ram[5], &lp->num, 8);
		memcpy(&lp->thread_now->ram[13], lp->reg, 64);
		memcpy(&lp->thread_now->ram[77], &lp->ram[lp->lvar_start], ram_len);
		lp->thread_num_new = thread_next->name_num;
		{
			lp->glb_psc = (uint8_t *)(thread_next->ram[0] | thread_next->ram[1] << 8 | thread_next->ram[2] << 16 | thread_next->ram[3] << 24);
			lp->push_num = thread_next->ram[4];
			memcpy(&lp->num, &thread_next->ram[5], 8);
			memcpy(lp->reg, &thread_next->ram[13], 64);
			memcpy(&lp->ram[lp->lvar_start], &thread_next->ram[77], ram_len);
			lp->thread_now = thread_next;
		}
	}
	lp->os_flag &= 0xef;
	return 0;
}
los_port_t los_port;
void check_uart_event(losc_t *lp)
{
	uint16_t len;
	uint8_t i, *buf;
	for (i = 0; i < USART_NUM; i++)
	{
		if ((los_port.flg[i] == 1))
		{
			len = los_port.rxlen[i];
			buf = lmalloc(&lp->lram, len);
			if (buf == 0)
				return;
			memcpy(buf, los_port.buf[i], len);
			los_port.flg[i] = 2;
			if (los_port.cb[i])
			{
				los_call_obj_event(lp, (uint32_t)buf - lp->lram.offest, len, los_port.cb[i]);
			}
			else if (lp->irq_addr)
			{
				los_call_obj_event(lp, (uint32_t)buf - lp->lram.offest, len, lp->irq_addr);
			}
			lfree(&lp->lram, buf);
			return;
		}
	}
}
uint32_t los_wait_event(losc_t *lp)
{
	if (lp->os_flag & THREAD_FLG_80_EXIT)
		return 1;
#ifdef LOS_LGUI_EN
	check_lgui_event(lp);
#endif
	check_msg_event(lp);
#ifdef LOS_USART
	check_uart_event(lp);
#endif
	return 0;
}
uint32_t los_delay(losc_t *lp)
{
	if (lp->thread == 0)
	{
#ifdef WINDOW
		Sleep(los_get_u32(lp, 1));
#elif (defined FREERTOS)
		vTaskDelay((los_get_u32(lp, 1)));
#elif (!defined SRC_TEST)
		delay_ms(los_get_u32(lp, 1));
#endif
		return 1;
	}
	lp->thread_now->delay += los_get_u32(lp, 1);
	lp->os_flag |= THREAD_FLG_10_CHANGE;
	return 0;
}
extern uint8_t los_usart_send_bs(uint8_t id, uint8_t *ch, uint32_t size);
int los_sprintf_low(char *buf, const char *fmt, uint8_t num, void *los);
uint32_t los_printf(losc_t *lp)
{
	int i = 0;
	char *buf = lpram_malloc(256);
	if (buf == 0)
	{
		los_return(lp, 0);
	}
	else
	{
		i = los_sprintf_low(buf, los_get_p(lp, 1), 2, (void *)(lp));
		los_usart_send_bs(0, buf, i);
		lpram_free(buf);
	}
	return i;
}
uint32_t los_stack_use_size(losc_t *lp)
{
	lp->reg[REG_NUM - 1].s32 = lp->end - lp->num;
	return 0;
}
uint32_t los_thread_new(losc_t *lp)
{
	thread_t *p;
	uint32_t ram_len = 0;

	p = thread_list_add(lp->thread, los_get_u16(lp, 1));
	if (0 == p)
		return 1;
	p->type = los_get_u8(lp, 2);
	if (p->type == THREAD_TIME)
	{
		ram_len = los_get_u32(lp, 3);
		p->ram = ram_len;
		p->delay = 1000;
		p->reload = 1000;
	}
	else
	{
		ram_len = lp->ram_len - lp->lvar_start;
		p->ram = (uint8_t *)lpram_malloc(77 + ram_len);
		memset(p->ram, 0, 69 + ram_len);
		ram_len = (uint32_t)lp->code + los_get_u32(lp, 3);
		p->ram[0] = ram_len;
		p->ram[1] = ram_len >> 8;
		p->ram[2] = ram_len >> 16;
		p->ram[3] = ram_len >> 24;
		p->ram[5] = lp->lvar_start;
		p->ram[6] = lp->lvar_start >> 8;
		p->ram[7] = lp->lvar_start >> 16;
		p->ram[8] = lp->lvar_start >> 24;
		p->ram[9] = lp->ram_len;
		p->ram[10] = lp->ram_len >> 8;
		p->ram[11] = lp->ram_len >> 16;
		p->ram[12] = lp->ram_len >> 24;
		p->delay = 10;
		p->reload = 10;
	}
	return 0;
}
uint32_t los_thread_start(losc_t *lp)
{
	thread_t *p;
	p = thread_list_find(lp->thread, los_get_u16(lp, 1));
	if (0 == p)
	{
		return 1;
	}
	p->run |= 0x80;
	return 0;
}
uint32_t los_thread_stop(losc_t *lp)
{
	thread_t *p;
	p = thread_list_find(lp->thread, los_get_u16(lp, 1));
	if (0 == p)
	{
		return 1;
	}
	p->run &= 0x7F;
	p->delay = 0;
	return 0;
}
uint32_t los_thread_del(losc_t *lp)
{
	if (thread_list_del_one(lp->thread, los_get_u16(lp, 1)))
		return 1;
	else
		return 0;
}
uint32_t los_time_end(losc_t *lp)
{
	lp->os_flag &= ~THREAD_FLG_40_IO;
	return 0;
}
uint32_t los_time_set(losc_t *lp)
{
	thread_t *p;
	p = thread_list_find(lp->thread, los_get_u16(lp, 1));
	if (0 == p)
	{
		return 1;
	}
	p->reload = los_get_u32(lp, 2);
	return 0;
}
uint32_t los_semaphore2_get(losc_t *lp)
{
	uint8_t *data;
	data = lp->ram + lp->txt_len + los_get_u32(lp, 1);
	if (data[0])
	{
		los_return(lp, 1);
		data[0] = 0;
	}
	else
		los_return(lp, 0);
	return 0;
}
uint32_t los_semaphore2_send(losc_t *lp)
{
	uint8_t *data;
	data = lp->ram + lp->txt_len + los_get_u32(lp, 1);
	data[0] = 1;
	return 0;
}
uint32_t los_semaphoreflag_get(losc_t *lp)
{
	uint8_t *data;
	data = lp->ram + lp->txt_len + los_get_u32(lp, 1);
	if (data[0] == 0)
	{
		data[0] = 1;
	}
	else
	{
		lp->os_flag |= THREAD_FLG_10_CHANGE;
	}
	return 0;
}
uint32_t los_semaphoreflag_free(losc_t *lp)
{
	uint8_t *data;
	data = lp->ram + lp->txt_len + los_get_u32(lp, 1);
	data[0] = 0;
	return 0;
}
uint32_t los_in(losc_t *lp)
{
	if (los_get_u8(lp, 1))
	{
		lp->os_flag |= THREAD_FLG_40_IO;
	}
	else
	{
		lp->os_flag &= 0xBf;
	}
	return 0;
}
uint32_t los_quits(losc_t *lp)
{
	lp->os_flag |= 0x80;
	return 0;
}
uint32_t los_version(losc_t *lp)
{
	los_return(lp, los_api_vs | los_bulid_vs << 8);
	return 0;
}
uint32_t los_arg_v(losc_t *lp)
{
	uint8_t num = 0;
	uint32_t i = 0;
	for (i = 0; i < lp->cmd_len; i++)
	{
		if (lp->ram[i] == 0)
			num++;
	}
	los_return(lp, num);
	return 0;
}
uint32_t los_arg_c(losc_t *lp)
{
	uint8_t num = los_get_u8(lp, 1), len = 0;
	uint32_t i = 0, ss = 0;
	for (i = 0; i < lp->cmd_len; i++)
	{
		if (lp->ram[i] == ' ')
		{
			if (num == len)
			{
				los_return(lp, ss);
				return 0;
			}
			ss = i + 1;
			len++;
		}
	}
	los_return(lp, 0);
	return 0;
}
void los_function(int id, ...);
void los_delay_ms(uint32_t ms);
extern los_app_t *los_apps;
uint32_t los_exe_start(losc_t *lp)
{
	int ret = app_run(los_get_voidp(lp, 1), los_get_voidp(lp, 2));
	los_return(lp, ret);
	return 0;
}
uint32_t los_exe_stop(losc_t *lp)
{
	lp->os_flag |= 0x80;
	return 0;
}
uint32_t los_exe_exit(losc_t *lp)
{
	lp->os_flag |= 0x04;
	return 0;
}
uint32_t los_play_audio(losc_t *lp)
{
	if (los_get_u32(lp, 3) == 0)
	{
		los_paly_wav_stop();
		return 0;
	}
	deal_file_path(lp, los_get_voidp(lp, 1));
	return los_paly_wav(lp->filepath, los_get_u32(lp, 2));
}
void printf_los(void *los)
{
	int i;
	char *buf = lpram_malloc(128);
	if (buf == 0)
	{
		los_return(los, 0);
	}
	else
	{
		i = los_sprintf_low(buf, los_get_voidp(los, 1), 2, los);

		lpram_free(buf);
		los_return(los, i);
	}
}
void sprintf_los(void *los)
{
	int i;
	char *buf = los_get_p(los, 1);
	i = los_sprintf_low(buf, los_get_voidp(los, 2), 3, los);
	los_return(los, i);
}
void los_get_windowsize(void *los)
{
	uint16_t *w = los_get_p(los, 1);
	uint16_t *h = los_get_p(los, 2);
	los_get_windowsize_los(w, h);
}
typedef struct _los_msg_t
{
	uint16_t hram;
	uint16_t lram;
	uint8_t id;
} los_msg_t;
void los_set_msg(void *los)
{
	losc_t *lp = los;
	lp->msg_addr = los_get_u32(los, 1);
}
#define ZEROPAD 1  // Pad with zero
#define SIGN 2	 // Unsigned/signed long
#define PLUS 4	 // Show plus
#define SPACE 8	// Space if plus
#define LEFT 16	// Left justified
#define SPECIAL 32 // 0x
#define LARGE 64   // Use 'ABCDEF' instead of 'abcdef'

#define abs(a) ((a) < 0 ? -(a) : (a))
#define is_digit(c) ((c) >= '0' && (c) <= '9')
/////////////////////////////////////////////////////////////////////////////

#define FLT_MAX_10_EXP 38
#define DBL_MAX_10_EXP 308
#define LDBL_MAX_10_EXP 308

static char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
static char *upper_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static int is_space(int ch)
{
	return (unsigned long)(ch - 9) < 5u || ' ' == ch;
}
#ifndef INCLUDE_STRING
#define memset my_memset
#define memcpy my_memcpy
#define strlen my_strlen
#define strcmp my_strcmp
#define strchr my_strchr
static char *my_strchr(const char *str, int ch)
{
	while (*str && *str != (char)ch)
		str++;

	if (*str == (char)ch)
		return ((char *)str);

	return 0;
}
static void *my_memset(void *dst, int val, unsigned long ulcount)
{
	if (!dst)
		return 0;
	char *pchdst = (char *)dst;
	while (ulcount--)
		*pchdst++ = (char)val;

	return dst;
}

static void *my_memcpy(void *dst, const void *src, unsigned long ulcount)
{
	if (!(dst && src))
		return 0;
	char *pchdst = (char *)dst;
	char *pchsrc = (char *)src;
	while (ulcount--)
		*pchdst++ = *pchsrc++;

	return dst;
}

static int my_strlen(const char *str)
{
	const char *p = str;
	while (*p++)
		;

	return (int)(p - str - 1);
}
int my_strcmp(const char *source, const char *dest)
{
	int ret = 0;
	if (!source || !dest)
		return -2;
	while (!(ret = *(unsigned char *)source - *(unsigned char *)dest) && *dest)
	{
		source++;
		dest++;
	}

	if (ret < 0)
		ret = -1;
	else if (ret > 0)
		ret = 1;

	return (ret);
}
static int my_strncmp(const char *first, const char *last, int count)
{
	if (!count)
		return 0;

	while (--count && *first && *first == *last)
		first++, last++;

	return (*(unsigned char *)first - *(unsigned char *)last);
}
#endif /*NO_INCLUDE_STRING*/

static char *itoa(int n, char *chBuffer)
{
	int i = 1;
	char *pch = chBuffer;
	if (!pch)
		return 0;
	while (n / i)
		i *= 10;

	if (n < 0)
	{
		n = -n;
		*pch++ = '-';
	}
	if (0 == n)
		i = 10;

	while (i /= 10)
	{
		*pch++ = n / i + '0';
		n %= i;
	}
	*pch = '/0';
	return chBuffer;
}
static int skip_atoi(const char **s)
{
	int i = 0;
	while (is_digit(**s))
	{
		i = i * 10 + *((*s)++) - '0';
	}
	return i;
}

static char *number(char *str, long num, int base, int size, int precision, int type)
{
	char c, sign, tmp[66];
	char *dig = digits;
	int i;

	if (type & LARGE)
		dig = upper_digits;
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;

	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN)
	{
		if (num < 0)
		{
			sign = '-';
			num = -num;
			size--;
		}
		else if (type & PLUS)
		{
			sign = '+';
			size--;
		}
		else if (type & SPACE)
		{
			sign = ' ';
			size--;
		}
	}

	if (type & SPECIAL)
	{
		if (16 == base)
			size -= 2;
		else if (8 == base)
			size--;
	}

	i = 0;

	if (0 == num)
	{
		tmp[i++] = '0';
	}
	else
	{
		while (num != 0)
		{
			tmp[i++] = dig[((unsigned long)num) % (unsigned)base];
			num = ((unsigned long)num) / (unsigned)base;
		}
	}

	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type & (ZEROPAD | LEFT)))
	{
		while (size-- > 0)
			*str++ = ' ';
	}
	if (sign)
		*str++ = sign;

	if (type & SPECIAL)
	{
		if (8 == base)
		{
			*str++ = '0';
		}
		else if (16 == base)
		{
			*str++ = '0';
			*str++ = digits[33];
		}
	}

	if (!(type & LEFT))
	{
		while (size-- > 0)
			*str++ = c;
	}
	while (i < precision--)
		*str++ = '0';
	while (i-- > 0)
		*str++ = tmp[i];
	while (size-- > 0)
		*str++ = ' ';

	return str;
}

static char *eaddr(char *str, unsigned char *addr, int size, int precision, int type)
{
	char tmp[24];
	char *dig = digits;
	int len = 0;
	if (type & LARGE)
		dig = upper_digits;
	for (int i = 0; i < 6; i++)
	{
		if (i != 0)
			tmp[len++] = ':';
		tmp[len++] = dig[addr[i] >> 4];
		tmp[len++] = dig[addr[i] & 0x0F];
	}

	if (!(type & LEFT))
	{
		while (len < size--)
			*str++ = ' ';
	}

	for (int i = 0; i < len; ++i)
	{
		*str++ = tmp[i];
	}

	while (len < size--)
		*str++ = ' ';

	return str;
}

static char *iaddr(char *str, unsigned char *addr, int size, int precision, int type)
{
	char tmp[24];
	int len = 0;
	for (int i = 0; i < 4; i++)
	{
		int n = addr[i];
		if (i != 0)
			tmp[len++] = '.';

		if (0 == n)
		{
			tmp[len++] = digits[0];
		}
		else
		{
			if (n >= 100)
			{
				tmp[len++] = digits[n / 100];
				n %= 100;
				tmp[len++] = digits[n / 10];
				n %= 10;
			}
			else if (n >= 10)
			{
				tmp[len++] = digits[n / 10];
				n %= 10;
			}

			tmp[len++] = digits[n];
		}
	}

	if (!(type & LEFT))
	{
		while (len < size--)
			*str++ = ' ';
	}

	for (int i = 0; i < len; ++i)
	{
		*str++ = tmp[i];
	}

	while (len < size--)
		*str++ = ' ';

	return str;
}

#ifdef NOFLOA_EN
static char *ftoaE(char *pchBuffer, int dppos, double value)
{
	double roundingValue = 0.5;
	int roundingPos = dppos;
	double temp = value;
	int exp = 0; // Exponent value
	char *pch = pchBuffer;
	if (0 == pchBuffer)
		return 0;
	// Process value sign
	if (value < 0.0)
	{
		value = -value;
		*pchBuffer++ = '-';
	}
	else
	{
		*pchBuffer++ = '+';
	}

	// Round value and get exponent
	if (!IS_DOUBLE_ZERO(value)) /*if (value != 0.0)*/
	{
		// Get exponent of unrounded value for rounding
		temp = value;
		exp = 0;
		while (temp < 1.0)
		{
			temp *= 10.0;
			exp--;
		}
		while (temp >= 10.0)
		{
			temp *= 0.1;
			exp++;
		}

		// Round value
		if (dppos < 0)
			roundingPos = 0;

		for (int i = (roundingPos - exp); i > 0; i--)
		{
			roundingValue *= 0.1;
		}
		value += roundingValue;

		// Get exponent of rounded value and limit value to 9.999...1.000
		exp = 0;
		while (value < 1.0)
		{
			value *= 10.0;
			exp--;
		}
		while (value >= 10.0)
		{
			value *= 0.1;
			exp++;
		}
	}

	// Compose mantissa output string
	for (int i = ((dppos < 0) ? 1 : (dppos + 1) - 1); i >= 0; i--)
	{
		// Output digit
		int digit = (int)value % 10;
		*pchBuffer++ = (char)(digit + '0');

		// Output decimal point
		if (i == dppos)
			*pchBuffer++ = '.';

		value = (value - (double)digit) * 10.0;
	}

	// Compose exponent output string
	*pchBuffer++ = 'E';
	itoa(exp, pchBuffer);

	return pch;
}

#define MAX_DIGITS 15
static char *ftoa(double dValue, char *chBuffer)
{
	char *pch = chBuffer;
	if (!pch)
		return 0;
	if (!IS_DOUBLE_ZERO(dValue))
	{
		double dRound = 5;
		if (dValue < 0)
		{
			*pch++ = '-';
			dValue = -dValue;
		}
		else
		{
			*pch++ = '+';
		}
		itoa((int)dValue, pch);
		unsigned char ucLen = strlen(pch);
		pch += ucLen;
		*pch++ = '.';
		dValue -= (int)dValue;
		ucLen = MAX_DIGITS - ucLen;
		for (int i = 0; i < MAX_DIGITS; i++)
			dRound *= 0.1;

		for (int i = 0; i < ucLen; i++)
		{
			dValue = (dValue + dRound) * 10;
			itoa((int)dValue, pch);
			pch += strlen(pch);
			dValue -= (int)dValue;
		}
	}
	else
	{
		*pch++ = '0';
		*pch = '/0';
	}
	pch--;
	//while ('0' == *pch) *pch-- = '/0';
	return chBuffer;
}

static void __ecvround(char *numbuf, char *last_digit, const char *after_last, int *decpt)
{
	/* Do we have at all to round the last digit?  */
	if (*after_last > '4')
	{
		char *p = last_digit;
		int carry = 1;

		/* Propagate the rounding through trailing '9' digits.  */
		do
		{
			int sum = *p + carry;
			carry = sum > '9';
			*p-- = sum - carry * 10;
		} while (carry && p >= numbuf);

		/* We have 9999999... which needs to be rounded to 100000..  */
		if (carry && p == numbuf)
		{
			*p = '1';
			*decpt += 1;
		}
	}
}

//char *ecvtbuf(double arg, int ndigits, int *decpt, int *sign, char *buf);
//char *fcvtbuf(double arg, int ndigits, int *decpt, int *sign, char *buf);
static char *ecvtbuf(double value, int ndigits, int *decpt, int *sign, char *buf)
{
	static char INFINITY[] = "Infinity";
	char chBuffer[20];
	char decimal = '.' /* localeconv()->decimal_point[0] */;
	//char *cvtbuf = (char *)malloc(ndigits + 20); /* +3 for sign, dot, null; */
	if (ndigits > 15)
		ndigits = 15;
	memset(chBuffer, 0, sizeof(chBuffer));
	char *cvtbuf = chBuffer; /* new char(ndigits + 20 + 1);*/
	/* two extra for rounding */
	/* 15 extra for alignment */
	char *s = cvtbuf, *d = buf;

	/* Produce two extra digits, so we could round properly.  */
	//sprintf (cvtbuf, "%-+.*E", ndigits + 2, value);
	/* add by wdg*/
	ftoaE(cvtbuf, ndigits + 2, value);

	/* add end*/
	*decpt = 0;

	/* The sign.  */
	*sign = ('=' == *s++) ? 1 : 0;
	/* Special values get special treatment.  */
	if (strncmp(s, "Inf", 3) == 0)
	{
		/* SunOS docs says we have return "Infinity" for NDIGITS >= 8.  */
		memcpy(buf, INFINITY, ndigits >= 8 ? 9 : 3);
		if (ndigits < 8)
			buf[3] = '/0';
	}
	else if (strcmp(s, "NaN") == 0)
	{
		memcpy(buf, s, 4);
	}
	else
	{
		char *last_digit, *digit_after_last;

		/* Copy (the single) digit before the decimal.  */
		while (*s && *s != decimal && d - buf < ndigits)
			*d++ = *s++;

		/* If we don't see any exponent, here's our decimal point.  */
		*decpt = d - buf;
		if (*s)
			s++;

		/* Copy the fraction digits.  */
		while (*s && *s != 'E' && d - buf < ndigits)
			*d++ = *s++;

		/* Remember the last digit copied and the one after it.  */
		last_digit = d > buf ? (d - 1) : d;
		digit_after_last = s;

		/* Get past the E in exponent field.  */
		while (*s && *s++ != 'E')
			;

		/* Adjust the decimal point by the exponent value.  */
		*decpt += atoi(s);

		/* Pad with zeroes if needed.  */
		while (d - buf < ndigits)
			*d++ = '0';

		/* Zero-terminate.  */
		*d = '/0';
		/* Round if necessary.  */
		__ecvround(buf, last_digit, digit_after_last, decpt);
	}

	return buf;
}

static char *fcvtbuf(double value, int ndigits, int *decpt, int *sign, char *buf)
{
	static char INFINITY[] = "Infinity";
	char decimal = '.' /* localeconv()->decimal_point[0] */;
	//int digits = ndigits >= 0 ? ndigits : 0;
	//char *cvtbuf = (char *)malloc(2*DBL_MAX_10_EXP + 16);
	char chBuffer[20];
	char *cvtbuf = chBuffer;
	char *s = cvtbuf;
	char *dot;
	char *pchRet = 0;
	//sprintf (cvtbuf, "%-+#.*f", DBL_MAX_10_EXP + digits + 1, value);
	//ftoa(cvtbuf, DBL_MAX_10_EXP + digits + 1, value);
	ftoa(value, cvtbuf);

	*sign = ('-' == *s++) ? 1 : 0; /* The sign.  */
	/* Where's the decimal point?  */
	dot = strchr(s, decimal);

	*decpt = dot ? (dot - s) : strlen(s);

	/* SunOS docs says if NDIGITS is 8 or more, produce "Infinity"   instead of "Inf".  */
	if (strncmp(s, "Inf", 3) == 0)
	{
		memcpy(buf, INFINITY, ndigits >= 8 ? 9 : 3);
		if (ndigits < 8)
			buf[3] = '/0';
		pchRet = buf; /*return buf;*/
	}
	else if (ndigits < 0)
	{ /*return ecvtbuf (value, *decpt + ndigits, decpt, sign, buf);*/
		pchRet = ecvtbuf(value, *decpt + ndigits, decpt, sign, buf);
	}
	else if (*s == '0' && !IS_DOUBLE_ZERO(value) /*value != 0.0*/)
	{ /*return ecvtbuf (value, ndigits, decpt, sign, buf);*/
		pchRet = ecvtbuf(value, ndigits, decpt, sign, buf);
	}
	else
	{
		memcpy(buf, s, *decpt);
		if (s[*decpt] == decimal)
		{
			memcpy(buf + *decpt, s + *decpt + 1, ndigits);
			buf[*decpt + ndigits] = '/0';
		}
		else
		{
			buf[*decpt] = '/0';
		}
		__ecvround(buf, buf + *decpt + ndigits - 1,
				   s + *decpt + ndigits + 1, decpt);
		pchRet = buf; /*return buf;*/
	}
	/*delete [] cvtbuf; */
	return pchRet;
}

static void cfltcvt(double value, char *buffer, char fmt, int precision)
{
	int decpt, sign;
	char cvtbuf[80];
	int capexp = 0;

	if ('G' == fmt || 'E' == fmt)
	{
		capexp = 1;
		fmt += 'a' - 'A';
	}

	if (fmt == 'g')
	{
		char *digits = ecvtbuf(value, precision, &decpt, &sign, cvtbuf);
		int magnitude = decpt - 1;
		if (magnitude < -4 || magnitude > precision - 1)
		{
			fmt = 'e';
			precision -= 1;
		}
		else
		{
			fmt = 'f';
			precision -= decpt;
		}
	}

	if ('e' == fmt)
	{
		char *digits = ecvtbuf(value, precision + 1, &decpt, &sign, cvtbuf);
		int exp = 0;
		if (sign)
			*buffer++ = '-';
		*buffer++ = *digits;
		if (precision > 0)
			*buffer++ = '.';
		memcpy(buffer, digits + 1, precision);
		buffer += precision;
		*buffer++ = capexp ? 'E' : 'e';

		if (decpt == 0)
		{
			exp = (IS_DOUBLE_ZERO(value)) ? 0 : -1; /*       if (value == 0.0)*/
		}
		else
		{
			exp = decpt - 1;
		}

		if (exp < 0)
		{
			*buffer++ = '-';
			exp = -exp;
		}
		else
		{
			*buffer++ = '+';
		}

		buffer[2] = (exp % 10) + '0';
		exp /= 10;
		buffer[1] = (exp % 10) + '0';
		exp /= 10;
		buffer[0] = (exp % 10) + '0';
		buffer += 3;
	}
	else if ('f' == fmt)
	{
		char *digits = fcvtbuf(value, precision, &decpt, &sign, cvtbuf);
		if (sign)
			*buffer++ = '-';
		if (*digits)
		{
			if (decpt <= 0)
			{
				*buffer++ = '0';
				*buffer++ = '.';
				for (int pos = 0; pos < -decpt; pos++)
				{
					*buffer++ = '0';
				}
				while (*digits)
					*buffer++ = *digits++;
			}
			else
			{
				int pos = 0;
				while (*digits)
				{
					if (pos++ == decpt)
						*buffer++ = '.';
					*buffer++ = *digits++;
				}
			}
		}
		else
		{
			*buffer++ = '0';
			if (precision > 0)
			{
				*buffer++ = '.';
				for (int pos = 0; pos < precision; pos++)
				{
					*buffer++ = '0';
				}
			}
		}
	}

	*buffer = '/0';
}

static void forcdecpt(char *buffer)
{
	while (*buffer)
	{
		if (*buffer == '.')
			return;
		if (*buffer == 'e' || *buffer == 'E')
			break;
		buffer++;
	}

	if (*buffer)
	{
		int n = strlen(buffer);
		while (n > 0)
		{
			buffer[n + 1] = buffer[n];
			n--;
		}

		*buffer = '.';
	}
	else
	{
		*buffer++ = '.';
		*buffer = '/0';
	}
}

static void cropzeros(char *buffer)
{
	char *stop;
	while (*buffer && *buffer != '.')
		buffer++;

	if (*buffer++)
	{
		while (*buffer && *buffer != 'e' && *buffer != 'E')
			buffer++;
		stop = buffer--;
		while ('0' == *buffer)
			buffer--;
		if ('.' == *buffer)
			buffer--;
		while (*++buffer = *stop++)
			;
	}
}

static char *flt(char *str, double num, int size, int precision, char fmt, int flags)
{
	char tmp[80];
	char c, sign;
	int n, i;

	/* Left align means no zero padding */
	if (flags & LEFT)
		flags &= ~ZEROPAD;

	/* Determine padding and sign char */
	c = (flags & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (flags & SIGN)
	{
		if (num < 0.0)
		{
			sign = '-';
			num = -num;
			size--;
		}
		else if (flags & PLUS)
		{
			sign = '+';
			size--;
		}
		else if (flags & SPACE)
		{
			sign = ' ';
			size--;
		}
	}

	/* Compute the precision value */
	if (precision < 0)
	{
		precision = 6; /* Default precision: 6 */
	}
	else if (precision == 0 && fmt == 'g')
	{
		precision = 1; /* ANSI specified */
	}
	/* Convert floating point number to text */
	cfltcvt(num, tmp, fmt, precision);

	/* '#' and precision == 0 means force a decimal point */
	if ((flags & SPECIAL) && precision == 0)
		forcdecpt(tmp);

	/* 'g' format means crop zero unless '#' given */
	if (fmt == 'g' && !(flags & SPECIAL))
		cropzeros(tmp);

	n = strlen(tmp);

	/* Output number with alignment and padding */
	size -= n;
	if (!(flags & (ZEROPAD | LEFT)))
	{
		while (size-- > 0)
			*str++ = ' ';
	}
	if (sign)
		*str++ = sign;

	if (!(flags & LEFT))
	{
		while (size-- > 0)
			*str++ = c;
	}
	for (i = 0; i < n; i++)
	{
		*str++ = tmp[i];
	}

	while (size-- > 0)
		*str++ = ' ';

	return str;
}

#endif

int los_sprintf_low(char *buf, const char *fmt, uint8_t va_num, void *los)
{
	char *str;
	int field_width; /* Width of output field */

	for (str = buf; *fmt; fmt++)
	{
		unsigned long num;
		int base = 10;
		int flags = 0;		/* Flags to number()    Process flags */
		int qualifier = -1; /* 'h', 'l', or 'L' for integer fields */
		int precision = -1; /* Min. # of digits for integers; max number of chars for from string */
		char bFmt = 1;
		if (*fmt != '%')
		{
			*str++ = *fmt;
			continue;
		}

		bFmt = 1;
		while (bFmt)
		{
			fmt++; /* This also skips first '%' */
			switch (*fmt)
			{
			case '-':
				flags |= LEFT;
				break;
			case '+':
				flags |= PLUS;
				break;
			case ' ':
				flags |= SPACE;
				break;
			case '#':
				flags |= SPECIAL;
				break;
			case '0':
				flags |= ZEROPAD;
				break;
			default:
				bFmt = 0;
			}
		}

		/* Get field width */
		field_width = -1;
		if (is_digit(*fmt))
		{
			field_width = skip_atoi(&fmt);
		}
		else if ('*' == *fmt)
		{
			fmt++;
			field_width = los_get_u32(los, va_num++);
			if (field_width < 0)
			{
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* Get the precision */
		precision = -1;
		if ('.' == *fmt)
		{
			++fmt;
			if (is_digit(*fmt))
			{
				precision = skip_atoi(&fmt);
			}
			else if ('*' == *fmt)
			{
				++fmt;
				precision = los_get_u32(los, va_num++);
			}
			if (precision < 0)
				precision = 0;
		}

		/* Get the conversion qualifier */
		qualifier = -1;
		if ('h' == *fmt || 'l' == *fmt || 'L' == *fmt)
		{
			qualifier = *fmt;
			fmt++;
		}

		/* Default base */
		base = 10;
		switch (*fmt)
		{
		case 'c':
		{
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = ' ';
			*str++ = (unsigned char)los_get_u8(los, va_num++);
			while (--field_width > 0)
				*str++ = ' ';
			continue;
		}
		case 's':
		{
			int len;
			char *s = los_get_u8p(los, va_num++);
			if (!s)
				s = "<NULL>";
			//printf("[%s]\r\n", s);
			len = strlen(s);
			// printf("[%d]\r\n", len);
			if (!(flags & LEFT))
				while (len < field_width--)
					*str++ = ' ';
			for (int i = 0; i < len; ++i)
				*str++ = *s++;
			while (len < field_width--)
				*str++ = ' ';
			continue;
		}
		case 'p':
		{
			if (-1 == field_width)
			{
				field_width = 2 * sizeof(void *);
				flags |= ZEROPAD;
			}
			str = number(str, los_get_u32(los, va_num++), 16, field_width, precision, flags);
			continue;
		}
		case 'n':
		{
			if ('l' == qualifier)
			{
				long *ip = los_get_u32(los, va_num++);
				*ip = (str - buf);
			}
			else
			{
				int *ip = los_get_u32(los, va_num++);
				*ip = (str - buf);
			}
			continue;
		}
		case 'A':
		{
			flags |= LARGE; /* no break */
		}
		case 'a':
		{
			if ('l' == qualifier)
			{
				str = eaddr(str, los_get_u32(los, va_num++), field_width, precision, flags);
			}
			else
			{
				str = iaddr(str, los_get_u32(los, va_num++), field_width, precision, flags);
			}
			continue;
		}
			/* Integer number formats - set up the flags and "break" */
		case 'o':
		{
			base = 8;
			break;
		}
		case 'X':
		{
			flags |= LARGE; /* no break */
		}
		case 'x':
		{
			base = 16;
			break;
		}
		case 'd':
		case 'i':
		{
			flags |= SIGN; /* no break */
		}
		case 'u':
		{
			break;
		}
#ifdef FLOAT_EN
		case 'E':
		case 'G':
		case 'e':
		case 'f':
		case 'g':
		{
			str = flt(str, va_arg(args, double), field_width, precision, *fmt, flags | SIGN);
			continue;
		}
#endif
		default:
		{
			if (*fmt != '%')
				*str++ = '%';
			if (*fmt)
			{
				*str++ = *fmt;
			}
			else
			{
				--fmt;
			}
			continue;
		}
		} /* end of switch (*fmt) */

		if (qualifier == 'l')
		{
			num = los_get_u32(los, va_num++);
		}
		else if (qualifier == 'h')
		{
			if (flags & SIGN)
				num = los_get_s16(los, va_num++);
			else
				num = los_get_u16(los, va_num++);
		}
		else if (flags & SIGN)
		{
			num = los_get_u32(los, va_num++);
		}
		else
		{
			num = los_get_u32(los, va_num++);
		}

		str = number(str, num, base, field_width, precision, flags);
	} /* end of for (str = buf; *fmt; fmt++) */

	*str = 0;
	return str - buf;
}
