/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.Author's mailbox:lgtbp@126.com.
 /
 / 未经授权，禁止商用。Commercial use is prohibited without authorization.
 /----------------------------------------------------------------------------*/
#include "los.h"
const uint8_t tlen[] = {1, 2, 4, 8};
const uint8_t txtlen[] = {1, 2, 3, 4, 8};
static fun_os func;
typedef void (*los_cmd)(losc_t *);
uint32_t los_quit(losc_t *lp)
{
	lp->os_flag |= 0x80;
}
static void cmd_2(losc_t *lp)
{
	memset(&lp->ram[lp->num], 0, *lp->glb_psc);
	lp->num += *lp->glb_psc++;
}
static void cmd_3(losc_t *lp)
{
	uint16_t len = lp->glb_psc[0] | lp->glb_psc[1] << 8;
	memset(&lp->ram[lp->num], 0, len);
	lp->num += len;
	lp->glb_psc += 2;
}
static void cmd_4(losc_t *lp)
{
	uint32_t len = lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16;
	memset(&lp->ram[lp->num], 0, len);
	lp->num += len;
	lp->glb_psc += 3;
}
static void cmd_5(losc_t *lp)
{
	lp->reg[(*lp->glb_psc) & 0x7].d64 = lp->reg[(*lp->glb_psc) >> 3].d64;
	lp->glb_psc++;
}
static void cmd_6(losc_t *lp)
{
	lp->reg[REG_NUM - 1].u32 = *lp->glb_psc++;
}
static void cmd_7(losc_t *lp)
{
	lp->reg[REG_NUM - 1].u32 = lp->glb_psc[0] | lp->glb_psc[1] << 8;
	lp->glb_psc += 2;
}
static void cmd_8(losc_t *lp)
{
	lp->reg[REG_NUM - 1].u32 = lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16;
	lp->glb_psc += 3;
}
static void cmd_9(losc_t *lp)
{
	lp->reg[REG_NUM - 1].u32 = lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16 | lp->glb_psc[3] << 24;
	lp->glb_psc += 4;
}
static void cmd_10(losc_t *lp)
{
	lp->reg[REG_NUM - 1].u64 = lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16 | lp->glb_psc[3] << 24 | lp->glb_psc[4] << 32 | lp->glb_psc[5] << 40 | lp->glb_psc[6] << 48 | lp->glb_psc[7] << 56;
	lp->glb_psc += 8;
}
static void cmd_11(losc_t *lp)
{
	lp->num -= *lp->glb_psc++;
	lp->glb_psc = (uint8_t *)(lp->ram[lp->end + 1] | lp->ram[lp->end + 2] << 8 | lp->ram[lp->end + 3] << 16 | lp->ram[lp->end + 4] << 24);
	lp->end += 4;
}
static void cmd_12(losc_t *lp)
{
	lp->num -= lp->glb_psc[0] | lp->glb_psc[1] << 8;
	lp->glb_psc += 2;
	lp->glb_psc = (uint8_t *)(lp->ram[lp->end + 1] | lp->ram[lp->end + 2] << 8 | lp->ram[lp->end + 3] << 16 | lp->ram[lp->end + 4] << 24);
	lp->end += 4;
}
static void cmd_13(losc_t *lp)
{
	lp->num -= lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16;
	lp->glb_psc += 3;
	lp->glb_psc = (uint8_t *)(lp->ram[lp->end + 1] | lp->ram[lp->end + 2] << 8 | lp->ram[lp->end + 3] << 16 | lp->ram[lp->end + 4] << 24);
	lp->end += 4;
}
static void cmd_14(losc_t *lp)
{
	lp->push_num += 4;
	memcpy(&lp->ram[lp->num], lp->reg[*lp->glb_psc & 0x7].data, *lp->glb_psc >> 3);
	lp->num += 4;
	lp->glb_psc++;
}
static void cmd_15(losc_t *lp)
{
	lp->push_num += 8;
	memcpy(&lp->ram[lp->num], lp->reg[*lp->glb_psc & 0x7].data, *lp->glb_psc >> 3);
	lp->num += 8;
	lp->glb_psc++;
}
static void cmd_16(losc_t *lp)
{
	lp->push_num += 4;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = 0;
	lp->ram[lp->num++] = 0;
	lp->ram[lp->num++] = 0;
}
static void cmd_17(losc_t *lp)
{
	lp->push_num += 4;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = 0;
	lp->ram[lp->num++] = 0;
}
static void cmd_18(losc_t *lp)
{
	lp->push_num += 4;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = 0;
}
static void cmd_19(losc_t *lp)
{
	lp->push_num += 4;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = *lp->glb_psc++;
	lp->ram[lp->num++] = *lp->glb_psc++;
}
static void cmd_20(losc_t *lp)
{
	lp->push_num += 8;
	lp->ram[lp->num] = *lp->glb_psc++;
	lp->num += 8;
}
static void cmd_21(losc_t *lp)
{
	lp->push_num += 8;
	lp->ram[lp->num] = *lp->glb_psc++;
	lp->ram[lp->num + 1] = *lp->glb_psc++;
	lp->num += 8;
}
static void cmd_22(losc_t *lp)
{
	lp->push_num += 8;
	lp->ram[lp->num] = *lp->glb_psc++;
	lp->ram[lp->num + 1] = *lp->glb_psc++;
	lp->ram[lp->num + 2] = *lp->glb_psc++;
	lp->num += 8;
}
static void cmd_23(losc_t *lp)
{
	lp->push_num += 8;
	lp->ram[lp->num] = *lp->glb_psc++;
	lp->ram[lp->num + 1] = *lp->glb_psc++;
	lp->ram[lp->num + 2] = *lp->glb_psc++;
	lp->ram[lp->num + 3] = *lp->glb_psc++;
	lp->num += 8;
}
static void cmd_24(losc_t *lp)
{
	lp->push_num += 8;
	memcpy(&lp->ram[lp->num], lp->glb_psc, 8);
	lp->glb_psc += 8;
	lp->num += 8;
}
static void cmd_61(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7;
	uint8_t len = *lp->glb_psc >> 3, i;
	lp->glb_psc++;
	lp->reg[reg].d64 = 0;
	for (i = 0; i < len; ++i)
		lp->reg[reg].data[i] = *lp->glb_psc++;
}
static void cmd_62(losc_t *lp)
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
	addr=lp->num - addr;
	for (i = 0; i < len; ++i)
		lp->ram[addr + i] = lp->reg[id].data[i];
}
static void cmd_1(losc_t *lp)
{
}
static void cmd_63(losc_t *lp)
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
	lp->reg[id].u64 = 0;
	addr=lp->num - addr;
	for (i = 0; i < len; ++i)
		lp->reg[id].data[i] = lp->ram[addr + i];
}
static void cmd_25(losc_t *lp)
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
	lp->glb_psc = lp->code + t1;
}
static void cmd_26(losc_t *lp)
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
	lp->glb_psc = lp->code + t1;
}
static void cmd_27(losc_t *lp)
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
	lp->glb_psc = lp->code + t1;
}
void los_arg_clear(losc_t *lp)
{
	lp->num -= lp->push_num;
	lp->push_num = 0;
}
uint32_t los_call_addr(losc_t *lp, uint32_t addr_now)
{
	uint32_t addr;
	addr = (uint32_t)lp->glb_psc;
	lp->ram[lp->end--] = addr >> 24;
	lp->ram[lp->end--] = addr >> 16;
	lp->ram[lp->end--] = addr >> 8;
	lp->ram[lp->end--] = addr;
	lp->glb_psc = lp->code + addr_now;
}
static void cmd_28(losc_t *lp)
{
	uint32_t addr, t1;
	lp->push_num = 0;
	t1 = lp->glb_psc[0];
	lp->glb_psc++;
	addr = (uint32_t)lp->glb_psc;
	lp->ram[lp->end--] = addr >> 24;
	lp->ram[lp->end--] = addr >> 16;
	lp->ram[lp->end--] = addr >> 8;
	lp->ram[lp->end--] = addr;
	lp->glb_psc = lp->code + lp->reg[t1].u32;
}
static void cmd_29(losc_t *lp)
{
}
static void cmd_30(losc_t *lp)
{
	los_return(lp, (*func)(lp));
	lp->num -= lp->push_num;
	lp->push_num = 0;
}
static void cmd_31(losc_t *lp)
{
}
static void cmd_58(losc_t *lp)
{
	uint8_t type = *lp->glb_psc++;
	uint8_t a, b;
	a = lp->glb_psc[0] & 0x7;
	b = lp->glb_psc[0] >> 3;
	switch (type)
	{
	case 0:
		lp->reg[a].u32 += lp->reg[b].u32;
		break;
	case 1:
		lp->reg[a].u32 -= lp->reg[b].u32;
		break;
	case 2:
		lp->reg[a].u32 *= lp->reg[b].u32;
		break;
	case 3:
		lp->reg[a].u32 /= lp->reg[b].u32;
		break;
	case 4:
		lp->reg[a].u32 %= lp->reg[b].u32;
		break;
	case 5:
		lp->reg[a].u32 &= lp->reg[b].u32;
		break;
	case 6:
		lp->reg[a].u32 |= lp->reg[b].u32;
		break;
	case 7:
		lp->reg[a].u32 ^= lp->reg[b].u32;
		break;
	case 8:
		lp->reg[a].u32 = (lp->reg[a].u32 == lp->reg[b].u32) ? 1 : 0;
		break;
	case 9:
		lp->reg[a].u32 = (lp->reg[a].u32 != lp->reg[b].u32) ? 1 : 0;
		break;
	case 10:
		lp->reg[a].u32 = (lp->reg[a].u32 > lp->reg[b].u32) ? 1 : 0;
		break;
	case 11:
		lp->reg[a].u32 = (lp->reg[a].s32 > lp->reg[b].s32) ? 1 : 0;
		break;
	case 12:
		lp->reg[a].u32 = (lp->reg[a].u32 < lp->reg[b].u32) ? 1 : 0;
		break;
	case 13:
		lp->reg[a].u32 = (lp->reg[a].s32 < lp->reg[b].s32) ? 1 : 0;
		break;
	case 14:
		lp->reg[a].u32 = (lp->reg[a].u32 >= lp->reg[b].u32) ? 1 : 0;
		break;
	case 15:
		lp->reg[a].u32 = (lp->reg[a].s32 >= lp->reg[b].s32) ? 1 : 0;
		break;
	case 16:
		lp->reg[a].u32 = (lp->reg[a].u32 <= lp->reg[b].u32) ? 1 : 0;
		break;
	case 17:
		lp->reg[a].u32 = (lp->reg[a].s32 <= lp->reg[b].s32) ? 1 : 0;
		break;
	case 18:
		lp->reg[a].u32 <<= lp->reg[b].s32;
		break;
	case 19:
		lp->reg[a].u32 >>= lp->reg[b].s32;
		break;
	case 20:
		lp->reg[a].u32 >>= lp->reg[b].s32;
		break;
	case 21:
		lp->reg[a].u32 = (lp->reg[a].u32 && lp->reg[b].u32) ? 1 : 0;
		break;
	case 22:
		lp->reg[a].u32 = (lp->reg[a].u32 || lp->reg[b].u32) ? 1 : 0;
		break;
	case 23:
		lp->reg[a].s32 /= lp->reg[b].s32;
		break;
	case 24:
		lp->reg[a].s32 %= lp->reg[b].s32;
		break;
	}
	lp->glb_psc++;
}
static void cmd_59(losc_t *lp)
{
	uint8_t type = *lp->glb_psc++;
	uint8_t a, b;
	a = lp->glb_psc[0] & 0x7;
	b = lp->glb_psc[0] >> 3;
	switch (type)
	{
	case 0:
		lp->reg[a].f32 += lp->reg[b].f32;
		break;
	case 1:
		lp->reg[a].f32 -= lp->reg[b].f32;
		break;
	case 2:
		lp->reg[a].f32 *= lp->reg[b].f32;
		break;
	case 3:
		lp->reg[a].f32 /= lp->reg[b].f32;
		break;
	case 8:
		lp->reg[a].f32 = (lp->reg[a].f32 == lp->reg[b].f32) ? 1 : 0;
		break;
	case 9:
		lp->reg[a].f32 = (lp->reg[a].f32 != lp->reg[b].f32) ? 1 : 0;
		break;
	case 10:
		lp->reg[a].f32 = (lp->reg[a].f32 > lp->reg[b].f32) ? 1 : 0;
		break;
	case 11:
		lp->reg[a].f32 = (lp->reg[a].s32 > lp->reg[b].s32) ? 1 : 0;
		break;
	case 12:
		lp->reg[a].f32 = (lp->reg[a].f32 < lp->reg[b].f32) ? 1 : 0;
		break;
	case 13:
		lp->reg[a].f32 = (lp->reg[a].s32 < lp->reg[b].s32) ? 1 : 0;
		break;
	case 14:
		lp->reg[a].f32 = (lp->reg[a].f32 >= lp->reg[b].f32) ? 1 : 0;
		break;
	case 15:
		lp->reg[a].f32 = (lp->reg[a].s32 >= lp->reg[b].s32) ? 1 : 0;
		break;
	case 16:
		lp->reg[a].f32 = (lp->reg[a].f32 <= lp->reg[b].f32) ? 1 : 0;
		break;
	case 17:
		lp->reg[a].f32 = (lp->reg[a].s32 <= lp->reg[b].s32) ? 1 : 0;
		break;
	case 21:
		lp->reg[a].f32 = (lp->reg[a].f32 && lp->reg[b].f32) ? 1 : 0;
		break;
	case 22:
		lp->reg[a].f32 = (lp->reg[a].f32 || lp->reg[b].f32) ? 1 : 0;
		break;
	case 23:
		lp->reg[a].s32 /= lp->reg[b].s32;
		break;
	case 24:
		lp->reg[a].s32 %= lp->reg[b].s32;
		break;
	}
	lp->glb_psc++;
}
static void cmd_60(losc_t *lp)
{
	uint8_t type = *lp->glb_psc++;
	uint8_t a, b;
	a = lp->glb_psc[0] & 0x7;
	b = lp->glb_psc[0] >> 3;
	switch (type)
	{
	case 0:
		lp->reg[a].d64 += lp->reg[b].d64;
		break;
	case 1:
		lp->reg[a].d64 -= lp->reg[b].d64;
		break;
	case 2:
		lp->reg[a].d64 *= lp->reg[b].d64;
		break;
	case 3:
		lp->reg[a].d64 /= lp->reg[b].d64;
		break;
	case 8:
		lp->reg[a].d64 = (lp->reg[a].d64 == lp->reg[b].d64) ? 1 : 0;
		break;
	case 9:
		lp->reg[a].d64 = (lp->reg[a].d64 != lp->reg[b].d64) ? 1 : 0;
		break;
	case 10:
		lp->reg[a].d64 = (lp->reg[a].d64 > lp->reg[b].d64) ? 1 : 0;
		break;
	case 11:
		lp->reg[a].d64 = (lp->reg[a].s32 > lp->reg[b].s32) ? 1 : 0;
		break;
	case 12:
		lp->reg[a].d64 = (lp->reg[a].d64 < lp->reg[b].d64) ? 1 : 0;
		break;
	case 13:
		lp->reg[a].d64 = (lp->reg[a].s32 < lp->reg[b].s32) ? 1 : 0;
		break;
	case 14:
		lp->reg[a].d64 = (lp->reg[a].d64 >= lp->reg[b].d64) ? 1 : 0;
		break;
	case 15:
		lp->reg[a].d64 = (lp->reg[a].s32 >= lp->reg[b].s32) ? 1 : 0;
		break;
	case 16:
		lp->reg[a].d64 = (lp->reg[a].d64 <= lp->reg[b].d64) ? 1 : 0;
		break;
	case 17:
		lp->reg[a].d64 = (lp->reg[a].s32 <= lp->reg[b].s32) ? 1 : 0;
		break;
	case 21:
		lp->reg[a].d64 = (lp->reg[a].d64 && lp->reg[b].d64) ? 1 : 0;
		break;
	case 22:
		lp->reg[a].d64 = (lp->reg[a].d64 || lp->reg[b].d64) ? 1 : 0;
		break;
	case 23:
		lp->reg[a].s32 /= lp->reg[b].s32;
		break;
	case 24:
		lp->reg[a].s32 %= lp->reg[b].s32;
		break;
	}
	lp->glb_psc++;
}
static void cmd_34(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7, i;
	uint8_t len2 = tlen[(*lp->glb_psc >> 3) & 0x3];
	uint8_t len1 = tlen[*lp->glb_psc >> 5];
	lp->glb_psc++;
	for (i = len1; i < len2; i++)
		lp->reg[reg].data[i] = 0;
}
const uint32_t sext_data[] =
	{0x80, 0x8000, 0x800000, 0x80000000};
static void cmd_33(losc_t *lp)
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
}
static void cmd_35(losc_t *lp)
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
}

static void cmd_37(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7;
	lp->glb_psc++;
	lp->reg[reg].d64 = lp->reg[reg].f32;
}
static void cmd_36(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7;
	lp->glb_psc++;
	lp->reg[reg].f32 = lp->reg[reg].d64;
}
static void cmd_38(losc_t *lp)
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
}
static void cmd_39(losc_t *lp)
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
}
static void cmd_40(losc_t *lp)
{
	uint8_t reg = *lp->glb_psc & 0x7;
	uint8_t len = tlen[(*lp->glb_psc >> 3) & 0x3], i;
	uint8_t len2 = tlen[*lp->glb_psc >> 5];
	lp->glb_psc++;
	for (i = len; i < len2; i++)
		lp->reg[reg].data[i] = 0;
}
static void cmd_41(losc_t *lp)
{
}
static void cmd_42(losc_t *lp)
{
}
static void cmd_50(losc_t *lp)
{
	uint8_t addr = *lp->glb_psc++;
	lp->glb_psc += addr;
}
static void cmd_51(losc_t *lp)
{
	uint16_t addr = lp->glb_psc[0] | lp->glb_psc[1] << 8;
	lp->glb_psc += 2;
	lp->glb_psc += addr;
}
static void cmd_52(losc_t *lp)
{
	uint32_t addr = lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16;
	lp->glb_psc += 3;
	lp->glb_psc += addr;
}
static void cmd_53(losc_t *lp)
{
	uint8_t addr = *lp->glb_psc++;
	lp->glb_psc -= addr;
}
static void cmd_54(losc_t *lp)
{
	uint16_t addr = lp->glb_psc[0] | lp->glb_psc[1] << 8;
	lp->glb_psc += 2;
	lp->glb_psc = lp->glb_psc - addr;
}
static void cmd_55(losc_t *lp)
{
	uint32_t addr = lp->glb_psc[0] | lp->glb_psc[1] << 8 | lp->glb_psc[2] << 16;
	lp->glb_psc += 3;
	lp->glb_psc = lp->glb_psc - addr;
}
static void cmd_47(losc_t *lp)
{
	uint8_t reg1 = *lp->glb_psc & 0x7;
	uint8_t reg2 = *lp->glb_psc >> 3;
	uint8_t addr = 0;
	lp->glb_psc++;
	addr = *lp->glb_psc++;
	if (lp->reg[reg1].u64 == lp->reg[reg2].u64)
		lp->glb_psc = lp->glb_psc + addr;
}
static void cmd_48(losc_t *lp)
{
	uint8_t reg1 = *lp->glb_psc & 0x7;
	uint8_t reg2 = *lp->glb_psc >> 3;
	uint16_t addr = 0;
	addr = lp->glb_psc[1] | lp->glb_psc[2] << 8;
	lp->glb_psc += 3;
	if (lp->reg[reg1].u64 == lp->reg[reg2].u64)
		lp->glb_psc = lp->glb_psc + addr;
}
static void cmd_49(losc_t *lp)
{
	uint8_t reg1 = *lp->glb_psc & 0x7;
	uint8_t reg2 = *lp->glb_psc >> 3;
	uint32_t addr = 0;
	addr = lp->glb_psc[1] | lp->glb_psc[2] << 8 | lp->glb_psc[3] << 16;
	lp->glb_psc += 4;
	if (lp->reg[reg1].u64 == lp->reg[reg2].u64)
		lp->glb_psc = lp->glb_psc + addr;
}
static void cmd_45(losc_t *lp)
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
}
static void cmd_46(losc_t *lp)
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
}
static void cmd_43(losc_t *lp)
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
	if (lp->reg[reg1].u64)
		lp->glb_psc += addr;
}
static void cmd_44(losc_t *lp)
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
	if (lp->reg[reg1].u64)
		lp->glb_psc = lp->glb_psc - addr;
}
static void cmd_64(losc_t *lp)
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
}
static void cmd_65(losc_t *lp)
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
}
static void cmd_66(losc_t *lp)
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
}
static void cmd_67(losc_t *lp)
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
}
static void cmd_56(losc_t *lp)
{
	uint8_t reg, reg1, reg2;
	reg = *lp->glb_psc & 0x3;
	reg1 = (*lp->glb_psc >> 2) & 0x7;
	reg2 = (*lp->glb_psc >> 5);
	if (lp->reg[reg].u32)
		lp->reg[reg].u32 = lp->reg[reg1].u32;
	else
		lp->reg[reg].u32 = lp->reg[reg2].u32;
	lp->glb_psc++;
}
static void cmd_57(losc_t *lp)
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
}
static void cmd_69(losc_t *lp)
{
	uint8_t *buf, i, len, reg1, reg2;
	reg1 = *lp->glb_psc & 0x7;
	reg2 = (*lp->glb_psc >> 3) & 0x7;
	len = tlen[*lp->glb_psc >> 6];
	lp->glb_psc++;
	buf = lp->ram + lp->reg[reg1].u32;
	for (i = 0; i < len; ++i)
		buf[i] = lp->reg[reg2].data[i];
}
static void cmd_68(losc_t *lp)
{
	uint8_t *data;
	uint8_t id = *lp->glb_psc & 0x7;
	uint8_t reg = (*lp->glb_psc >> 3) & 0x7, len, i;
	len = tlen[*lp->glb_psc >> 6];
	data = lp->ram + lp->reg[reg].u32;
	lp->reg[id].u64 = 0;
	lp->glb_psc++;
	for (i = 0; i < len; i++)
		lp->reg[id].data[i] = data[i];
}
static void cmd_70(losc_t *lp)
{
	uint32_t i, tlens = 0;
	uint8_t id = *lp->glb_psc & 0x7;
	uint8_t reg = (*lp->glb_psc >> 3) & 0x3, len, *addr, *data;
	len = *lp->glb_psc >> 6;
	lp->glb_psc++;
	addr = lp->ram + lp->reg[id].u32;
	data = lp->ram + lp->reg[reg].u32;
	for (i = 0; i < len; ++i)
	{
		tlens |= (*lp->glb_psc) << (i * 8);
		lp->glb_psc++;
	}
	for (i = 0; i < tlens; ++i)
		addr[i] = data[i];
}
uint32_t cmd_32(losc_t *lp)
{
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
uint32_t los_push_arg(losc_t *lp, uint32_t arg)
{
	lp->ram[lp->num++] = arg;
	lp->ram[lp->num++] = arg >> 8;
	lp->ram[lp->num++] = arg >> 16;
	lp->ram[lp->num++] = arg >> 24;
}
uint32_t los_rerun(losc_t *lp)
{
	uint8_t op = 0;
	while (1)
	{
		op = *lp->glb_psc;
		if (op == 0)
			return 1;
		lp->glb_psc++;
		los_cmd_api[op](lp);
	}
	return 0;
}
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
float los_get_float(losc_t *lp, uint32_t num)
{
	uint16_t len = 0;
	len = lp->num - num * 4;
	return (float)(lp->ram[len] | lp->ram[len + 1] << 8 | lp->ram[len + 2] << 16 | lp->ram[len + 3] << 24);
}
void los_return(losc_t *lp, int32_t ret)
{
	lp->reg[REG_NUM - 1].s32 = ret;
}
void los_return_float(losc_t *lp, float ret)
{
	lp->reg[REG_NUM - 1].f32 = ret;
}
static uint8_t los_init_code(losc_t *lp, uint8_t *buf)
{
	int ret;
	uint32_t tlens, stack, heap;
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
	tlens = buf[8] | buf[9] << 8 | buf[10] << 16 | buf[11] << 24;
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
	lp->ram = (uint8_t *)lpram_malloc(lp->end + heap);
	if (lp->ram == 0)
		return 7;
	memset(lp->ram, 0, lp->end + heap);
	lp->end--;
	tlens += lp->txt_len;
	if (tlens)
	{
		memcpy(lp->ram, buf, tlens);
		buf += tlens;
	}
	lp->code = buf;
	lp->glb_psc = lp->code;
	return 0;
}
uint32_t los_app_first(uint8_t *addr)
{
	int res;
	losc_t *lp;
	if (addr == 0)
		return 10;
	lp = (losc_t *)lpram_malloc(sizeof(losc_t));
	if (0 == lp)
		return 11;
	memset(lp, 0, sizeof(losc_t));
	res = los_init_code(lp, addr);
	if (res)
		return res;
	res = los_rerun(lp);
	lpram_free(lp->ram);
	lpram_free(lp);
	return res;
}
void los_set_function(fun_os f)
{
	func = f;
}