/**
 * @file usart.h
 * @brief los的usart api
 * @details
 * @mainpage
 * @author lp
 * @email lgtbp@126.com
 * @version 1
 * @license Copyright © 2018, lp. All rights reserved.
 *			This project (los) shall not be used for profit by any unit or individual without permission;
 *		if it is used for non-profit use such as personal use and communication,
 *      the author shall be specified and the source shall be marked.
 *         The copyright owner is not liable for any direct, indirect, incidental, special, disciplinary or
 *      consequential damages (including but not limited to the purchase of substitute goods or services;
 *      loss of use, data or profits; or commercial interruption) and then causes any liability theory,
 *      whether Contract, strict liability, or infringement (including negligence or otherwise),
 *      use the Software in any way, even if it is informed of the possibility of such damage.
 */
#ifndef USER_API_USART_H_
#define USER_API_USART_H_
#include <stdint.h>
/**
 * los的串口结构体
 */
typedef struct _los_usart_t
{
	uint8_t id;      ///<串口序号
	uint8_t stop;    ///<停止位
	uint8_t num;     ///<数据位
	uint8_t ver;     ///<检验位
	uint32_t prot;   ///<串口速度
	uint16_t cancel; ///<缓冲大小
} los_usart_t;

typedef void (*usart_iqr_t)(uint8_t *data, uint32_t len); ///<串口中断函数
/**
 * @brief 串口初始化函数
 *
 * @param los_usart 串口结构体
 * @param usart_iqr 串口中断函数
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint8_t usart_init(los_usart_t *los_usart, usart_iqr_t *usart_iqr);
/**
 * @brief dis串口初始化函数
 *
 * @param id 串口id
 * @return no
 */
void usart_disinit(uint8_t id);
/**
 * @brief 串口发送一字节函数
 *
 * @param id 串口id
 * @param data 数据
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint8_t usart_send_byte(uint8_t id, uint8_t data);
/**
 * @brief 串口发送x字节函数
 *
 * @param id 串口id
 * @param data 数据
 * @param len 数据长度
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint8_t usart_send_bytes(uint8_t id, uint8_t *data, uint32_t len);
/**
 * @brief 串口读取一字节函数
 *
 * @param id 串口id
 * @param data 数据
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint8_t usart_read_byte(uint8_t id, uint8_t *data);

#endif
