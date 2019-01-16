/**
 * @file time.h
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
#ifndef USER_API_TIME_H_
#define USER_API_TIME_H_

#include <stdint.h>
/**
 * los的time结构体
 */
typedef struct tm {
	uint8_t sec;  ///< seconds,  range 0 to 59
	uint8_t min;  ///< minutes, range 0 to 59
	uint8_t hour;  ///< hours, range 0 to 23
	uint8_t mday;  ///< day of the month, range 1 to 31
	uint8_t month;  ///< month, range 0 to 11
	uint8_t week;  ///< day of the week, range 0 to 6
	uint16_t yday;  ///< day in the year, range 0 to 365
	uint16_t year;  ///< The number of years since 2000
} tm_t;
/**
 * @brief 获取当前的时间
 *
 * @param tm time结构体
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t time(tm_t *tm);
/**
 * @brief 设置当前的时间
 *
 * @param tm time结构体
 * @return
 * @retval 0:ok
 * @retval other err
 */
uint32_t los_set_time(tm_t *tm);
#endif
