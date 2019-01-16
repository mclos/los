/**
 * @file gpio.h
 * @brief los gpio
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
#ifndef USER_API_GPIO_H_
#define USER_API_GPIO_H_

void los_gpio_init(uint32_t gpio, uint8_t mode, uint8_t pull);
void los_gpio_disinit(uint32_t gpio);

void los_gpiog_init(uint32_t group, uint32_t gpio, uint8_t mode, uint8_t pull);
void los_gpiog_disinit(uint32_t group, uint32_t gpio);

void los_gpio_set(uint32_t gpio);
void los_gpio_reset(uint32_t gpio);

void los_gpiog_set(uint32_t group, uint32_t gpio);
void los_gpiog_reset(uint32_t group, uint32_t gpio);

void los_gpio_int(uint32_t gpio);
void los_gpio_reint(uint32_t gpio);
void los_gpiog_int(uint32_t group, uint32_t gpio);
void los_gpiog_reint(uint32_t group, uint32_t gpio);
#endif
