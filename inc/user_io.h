/*
 * user_io.c
 *
 *  Created on: 2018锟斤拷9锟斤拷4锟斤拷
 *      Author: PC
 */
#include "los.h"
#include "string.h"

void function_los(void *los);
void os_delay_ms_los(void *los);

/**
 * 获取当前lcd
 * */
void *los_get_lcd_buf(void);
/**
 * 把pop弹出来
 * */
void los_pop_lcd_buf(void *buf);

void los_delayms(unsigned int ms);