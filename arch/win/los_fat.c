/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/
#if (defined WINDOW) || (defined FAT_FATFS)
#include "los.h"
#include "stdio.h"
#include "stdint.h"
#include "los_fat.h"
#include "los_fat_config.h"
#ifdef WINDOW
uint8_t pc_los_path[128];
#else
uint8_t pc_los_path[128];
#endif
extern void *lpram_malloc(uint32_t size);
extern void lpram_free(void *ram);
void fopen_los(void *los)
{
	lfile_t *file;
	uint8_t buf[128];
	sprintf(buf, "%s%s", pc_los_path, los_get_voidp(los, 1));
#ifdef FAT_FATFS
	pc_los_path[1] -= 'a';
#endif
	file = lf_open(buf, los_get_u8p(los, 2));
	los_return(los, (uint32_t)file);
}
void fread_los(void *los)
{
	uint32_t size = lf_read((lfile_t *)los_get_u32(los, 3), los_get_p(los, 1), los_get_u32(los, 2));
	los_return(los, size);
}
void fwrite_los(void *los)
{
	uint32_t size = lf_write((lfile_t *)los_get_u32(los, 3), los_get_p(los, 1), los_get_u32(los, 2));
	los_return(los, size);
}
void fclone_los(void *los)
{
	int ret = lf_close((lfile_t *)los_get_u32(los, 1));
	los_return(los, ret);
}
void fgetc_los(void *los)
{
	uint8_t ret;
	los_return(los, ret);
}
void fputc_los(void *los)
{
	uint32_t size = 0;
	uint8_t data = los_get_u8(los, 1);
	if (size == 1)
		los_return(los, data);
	else
		los_return(los, -1);
}
void fgets_los(void *los)
{
}
void fputs_los(void *los)
{
}
void ftell_los(void *los)
{
}
void rewind_los(void *los)
{
}
void fseek_los(void *los)
{
}
void file_size_los(void *los)
{
}
void mkdir_los(void *los)
{
}
void fat_deorphan_los(void *los)
{
}
void fat_total_los(void *los)
{
}
extern uint32_t get_free(void);
void fat_free_los(void *los)
{
}
void remove_los(void *los) 
{
	int ret = remove(los_get_voidp(los, 1));
	los_return(los, ret);
}
void rename_los(void *los) 
{
	int ret = rename(los_get_voidp(los, 1), los_get_voidp(los, 2));
	los_return(los, ret);
}
void opendir_los(void *los)
{
	lf_dir_t *dirp;
	uint8_t buf[128];
	sprintf(buf, "%s%s", pc_los_path, los_get_voidp(los, 1));
#ifdef FAT_FATFS
	pc_los_path[1] -= 'a';
#endif
	dirp = lf_opendir((const char *)buf); 
	los_return(los, dirp);
}
void readdir_los(void *los)
{
	int ret;
	ret = lf_readdir(los_get_u32(los, 1), los_get_p(los, 2));
	los_return(los, ret);
}
void closedir_los(void *los)
{
	int ret = lf_closedir(los_get_u32(los, 1));
	los_return(los, ret);
}
void rewinddir_los(void *los)
{
}
void telldir_los(void *los)
{
}
void seekdir_los(void *los)
{
}
#endif
