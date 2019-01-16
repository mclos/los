/**
 * @file user_io.h
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
#ifndef USER_API_USERIO_H_
#define USER_API_USERIO_H_

#include <stdint.h>
/**
 * @brief los系统,自定义函数
 *
 * @param id 打印格式
 * @param ... 打印数据
 * @return no
 */
void los_function(int id, ...);
/**
 * @brief 整個los延时
 *
 * @param ms 延时ms毫秒
 * @return no
 */
void los_delay_ms(uint32_t ms);

#endif
