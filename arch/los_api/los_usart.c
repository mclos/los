/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/
#include "los.h"
#include "los_usart.h"
 

//串口的head
los_port_t los_port;
static uint8_t witch_port = 1; 
/////////////////////////////////以下uart//////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//外部启动注册
uint8_t los_usart_read_b(uint8_t id, uint8_t *ch)
{
	return 1;
}
uint8_t los_usart_send_b(uint8_t id, uint8_t ch)
{
	 
	return 1;
}
uint8_t los_usart_send_bs(uint8_t id, uint8_t *ch, uint32_t size)
{
	 
	return 1;
}

//外部启动注册
uint8_t los_usart_init(uint8_t id, los_uart_t *los_uart)
{
	 
	return 1;
}
uint8_t los_usart_deinit(uint8_t id)
{
 
	return 1; //直接返回错误
}

/////////////////////////////////以上uart//////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
 
 
void usart_check_out_los(void *los)
{
	uint8_t i = 0;
	 
	for (i = 0; i < USART_NUM; i++) //遍历窗口
	{
 		if (los_port.los[i] == los)
		{
			los_port.los[i] = 0;	 //清空。不然下次判断err
			los_usart_deinit(i + 1); //窗口要加一
			lpram_free(los_port.buf[i]); //释放串口缓存
 		}
	}
}
 
void usart_init_los(void *los)
{
	los_uart_t *los_uart = los_get_voidp(los, 1);
	uint8_t id = los_uart->id;
	id--;
 	if (id < USART_NUM) //在范围内
	{
 		if (los_port.los[id]) //这个已经被注册了
		{
			los_return(los, 4);
			return;
		}
		los_port.buf[id] = lpram_malloc(los_uart->cancel); //申请内存//什么的
		if (los_port.buf[id] == 0)
		{
			los_return(los, 2); //内存分配失败
			return;
		}
 		los_port.los[id] = los; //记录这个los
		los_port.rxlen[id] = 0;
		los_port.flg[id] = 0;
		los_port.len[id] = los_uart->cancel;
		los_port.cb[id] = los_get_u32(los, 2);
 		id = los_usart_init(id + 1, los_uart); //
		los_return(los, id);
	}
	else
	{
		los_return(los, 1);
	}
}
/**
 * 关闭
 * */
void usart_disinit_los(void *los)
{
	uint8_t id = los_get_u8(los, 1), ret = 2; //关闭
	if (id < USART_NUM_S)					  //在范围内
	{
		ret = los_usart_deinit(id);
	}
	los_return(los, id);
}
void usart_send_byte_los(void *los)
{
	uint8_t id = los_get_u8(los, 1), ret = 2;
	if (id < USART_NUM_S) //在范围内
	{
		ret = los_usart_send_b(id, los_get_u8(los, 2));
	}
	los_return(los, ret);
}
void usart_send_bytes_los(void *los)
{
	uint8_t id = los_get_u8(los, 1), ret = 2;
 	if (id < USART_NUM_S) //在范围内
	{
		ret = los_usart_send_bs(id, los_get_voidp(los, 2), los_get_u32(los, 3));
	}
	los_return(los, ret);
}
void usart_read_byte_los(void *los)
{
	uint8_t ret, ch, id = los_get_u8(los, 1);
	if (id < USART_NUM_S) //在范围内
	{
		ret = los_usart_read_b(id, &ch);
		*(uint8_t *)los_get_voidp(los, 2) = ch;
	}
	los_return(los, ret);
}
