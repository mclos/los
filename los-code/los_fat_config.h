/**
 * 这个用于统一fat-api
 * */
#ifndef _LOS_FAT_CONFIG__API_H_
#define _LOS_FAT_CONFIG__API_H_
#include "stdint.h"
#include "los.h"
#define LOS_FAT_FILE 0
#define LOS_FAT_DIR 1

#define NAME_MAX 32
typedef struct _lf_dirent_t
{
    unsigned int d_ino;        /* inode number  */
    unsigned short d_off;      /* offset to this dirent   */
    unsigned short d_reclen;   /* length of this d_name   */
    unsigned char d_type;      /* the type of d_name   */
    char d_name[NAME_MAX + 1]; /* file name (null-terminated)   */
} lf_dirent_t;
#if (defined FAT_CAPI) || (defined WINDOW)
#ifdef WINDOW //这下面WINDOW是capi
#include <windows.h>
typedef struct _dirdesc
{
    int dd_fd;    /** file descriptor associated with directory */
    long dd_loc;  /** offset in current buffer */
    long dd_size; /** amount of data returned by getdirentries */
    char *dd_buf; /** data buffer */
    int dd_len;   /** size of data buffer */
    long dd_seek; /** magic cookie returned by getdirentries */

    HANDLE hFind;
} win_DIR;
#define __dirfd(dp) ((dp)->dd_fd)
win_DIR *win_opendir(const char *);
int win_readdir(win_DIR *, lf_dirent_t *);
int win_closedir(win_DIR *);

#define lf_dir_t win_DIR
#define lf_opendir(fname) win_opendir(fname)
#define lf_readdir(dir, dp) win_readdir(dir, dp)
#define lf_closedir(dir) win_closedir(dir)

#else //这下面linux是capi

#define lf_dir_t DIR
int lfat_readdir(lf_dir_t *dir, lf_dirent_t *dpout);
#define lf_opendir(fname) opendir(fname)
#define lf_readdir(dir, dp) lfat_readdir(dir, dp)
#define lf_closedir(dir) closedir(dir)
#endif
//这下面是capi共同的
#define lfile_t FILE
#define lf_open(fname, type) fopen(fname, type)
#define lf_read(file, buf, sizeofbuf) ((size_t)fread((void *)(buf), (size_t)1, (size_t)(sizeofbuf), (file)))
#define lf_write(file, buf, sizeofbuf) ((size_t)fwrite((const void *)(buf), (size_t)1, (size_t)(sizeofbuf), (file)))
#define lf_close(file) fclose(file)
#define lf_size(fp) lfat_size(fp)
#define lf_seek(fp, offset, fromwhere) fseek(fp, offset, fromwhere)
#define lf_exit(fname) access(fname, 0)
#define lf_mkdir(fname) mkdir(fname)
#define lf_tell(fname) ftell(fname)

#elif FAT_FATFS //fatfs文件系统

#include "ff.h"
#define lfile_t FIL
#define lf_dir_t DIR
uint32_t lfat_read_file(FIL *file, uint8_t *buf, uint32_t sizeofbuf);
uint32_t lfat_write_file(FIL *file, uint8_t *buf, uint32_t sizeofbuf);
lfile_t *lfat_open(uint8_t *filename, uint8_t *type);
int lfat_close(lfile_t *file);
lf_dir_t *lfat_opendir(char *fname);
int lfat_readdir(lf_dir_t *dir, lf_dirent_t *dpout);
int lfat_closedir(lf_dir_t *dir);
int lfat_seek(FIL *file, uint32_t offset, int fromwhere);
int lfat_exit(uint8_t *name);

#define lf_open(fname, type) lfat_open(fname, type)
#define lf_read(file, buf, sizeofbuf) lfat_read_file(file, buf, sizeofbuf)
#define lf_write(file, buf, sizeofbuf) lfat_write_file(file, buf, sizeofbuf)
#define lf_close(file) lfat_close(file)
#define lf_opendir(fname) lfat_opendir(fname)
#define lf_readdir(dir, dp) lfat_readdir(dir, dp)
#define lf_closedir(dir) lfat_closedir(dir)
#define lf_size(fp) f_size(fp)
#define lf_seek(fp, offset, fromwhere) lfat_seek(fp, offset, fromwhere)
#define lf_exit(fname) lfat_exit(fname)
#define lf_mkdir(fname) f_mkdir(fname)
#define lf_tell(fname) f_tell(fname)
#endif

uint8_t los_fat_init(void);
/**
 * 遍历文件列表列表 
 */
uint8_t los_fat_list(uint8_t *path);
/**
 * 遍历文件列表列表 -只在这个目录
 */
uint8_t los_fat_list_dir(uint8_t *path);

//读取文件
//buf_path：路径
//len：长度
//返回：数据
uint8_t *los_fat_read_file(char *buf_path, uint32_t *len);
//写文件
//path：路径
//data:数据
//len：长度
//返回：0 OK
uint32_t los_fat_save_file(char *path, uint8_t *data, uint32_t len);

void los_fat_test();

/**
 * 文件存在不
 * 0存在 
 * */
uint8_t lf_file_exit(uint8_t *name);

/**
 *待分离com的数据
 *buf是输入地址，输出信息
len=  comm_arg_deal((char*)com, &(char **)buf);
 */
int comm_arg_deal(char *com, uint8_t ***buf);
/**
 * 文件名.。带后缀
 */
uint8_t get_filename(char *path, char *name);
/*
 *文件的路径--后面没有/的
 */
uint8_t get_dirname(char *path, char *name);
//文件名，不带后缀
uint8_t get_fname(char *path, char *name);
//返回文件名后缀
uint8_t get_fname_hx(char *path, char *name);

#endif /* _LOS_FAT_CONFIG__API_H_ */
