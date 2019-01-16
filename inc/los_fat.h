#ifndef FAT_API_H_
#define FAT_API_H_

#include "los.h"
#include "stdio.h"
#include "stdint.h"

#define LOS_FAT_FILE 0
#define LOS_FAT_DIR 1
#define LOS_FILE_NAME_LEN 255
 

//
// 文件的打开操作 fopen 打开一个文件
void fopen_los(void *los);
// fread 以二进制形式读取文件中的数据
void fread_los(void *los);
// fwrite 以二进制形式写数据到文件中去
void fwrite_los(void *los);
// 文件的关闭操作 fclose 关闭一个文件
void fclone_los(void *los);
// 文件的读写操作  从文件中读取一个字符
void fgetc_los(void *los);
// fputc 写一个字符到文件中去
void fputc_los(void *los);
//extern uint32_t lfs_gets(lfs_file_t *file, uint8_t *buf, uint32_t size);
//  fgets 从文件中读取一个字符串
void fgets_los(void *los);
//extern uint32_t lfs_puts(lfs_file_t *file, uint8_t *buf);
//　  fputs 写一个字符串到文件中去
void fputs_los(void *los);
// ftell 了解文件指针的当前位置
void ftell_los(void *los);
//　文件定位函数 rewind 反绕
void rewind_los(void *los);
//fseek 随机定位
void fseek_los(void *los);
void file_size_los(void *los);
void mkdir_los(void *los);
void fat_deorphan_los(void *los);
void fat_total_los(void *los);
extern uint32_t get_free(void);
void fat_free_los(void *los);

void remove_los(void *los);
void rename_los(void *los);

//# 打开目录
void opendir_los(void *los);
// # 读取目录
void readdir_los(void *los);
// # 关闭目录
void closedir_los(void *los);
// # 定位指针到开头
void rewinddir_los(void *los);
//# 返回目录的当前位置
void telldir_los(void *los);
//# 定位指定到目录的 POS 位置
void seekdir_los(void *los);

#endif
