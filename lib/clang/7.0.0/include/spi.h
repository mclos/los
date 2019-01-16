/**
 * @file spi.h
 * @brief los的spi api
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
#ifndef USER_API_SPI_H_
#define USER_API_SPI_H_

void spi1_init(void);
void spi1_disinit(void);
void spi1_speed(uint32_t speed);
uint8_t spi1_send_byte(uint8_t data);
uint16_t spi1_send_word(uint16_t data);
void spi1_cs_set(void);
void spi1_cs_reset(void);

#endif
