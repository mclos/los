/**
 * @file string.h
 * @brief los的string api
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
#ifndef _LOS_STRING_H_
#define _LOS_STRING_H_

#include"stdint.h"

void *malloc(uint32_t size);
void free(void *);
void * realloc(void *, uint32_t size);
void * calloc(uint32_t size);
uint32_t ram_free(void);
uint32_t ram_max(void);
void *memset(void *str, int c, uint32_t size);
void *memcpy(void *dest, void *src, uint32_t size);
int memcmp(void *str1, void *str2, uint32_t size);
uint32_t strlen(char *str);
char *strcpy(char *dest, char *src);
int strcmp(char *str1, char *str2);

#endif
