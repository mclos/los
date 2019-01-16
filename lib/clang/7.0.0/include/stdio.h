/**
 * @file stdio.h
 * @brief los的stdio api
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
#ifndef __LOS_STDIO_H__
#define __LOS_STDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

int printf(const char * fmt, ...);

int sprintf(char * buf, const char * fmt, ...);
FILE fopen(char *filename, char* type);
uint32_t fread(void *ptr, size_t nmemb, FILE stream);
size_t fwrite(void *ptr, size_t nmemb, FILE stream);
int fclose(FILE stream);
char fgetc(FILE stream);
int fputc(char src, FILE stream);
uint32_t fgets(char *str, uint32_t n, FILE stream);
int fputs(char *str, FILE stream);
uint32_t ftell(FILE stream);
int rewind(FILE stream);
int fseek(FILE stream, size_t offset, uint32_t whence);
uint32_t file_size(FILE stream);
uint32_t mkdir(char *path);
uint32_t fat_deorphan(void);
uint32_t fat_total(char c);
uint32_t fat_free(char c);

int remove(char *fname);
int rename(char *OldFilename, char *NewFilename);

DIR opendir(char * path);
uint32_t readdir(DIR dir_handle, lf_dirent_t* f);
int closedir(DIR dir_handle);
void rewinddir(DIR dir_handle);
uint32_t telldir(DIR dir_handle);
void seekdir(DIR dir_handle, uint32_t s);

#ifdef __cplusplus
}
#endif

#endif /* __STDIO_H__ */
