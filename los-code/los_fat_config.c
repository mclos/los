#include "los_fat_config.h"
#define ARRLENs(arr) (sizeof(arr) / sizeof(arr[0]))
#if (defined FAT_CAPI) || (defined WINDOW)
#ifdef WINDOW
#include <stdio.h>
#include <windows.h>
#include "dirent.h"

win_DIR *win_opendir(const char *name)
{
    HANDLE hFind;
    win_DIR *dir;
    WIN32_FIND_DATA FindData;
    char namebuf[512];
    sprintf(namebuf, "%s\\*.*", name);
    hFind = FindFirstFile(namebuf, &FindData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Find dir failed [%s](%d)\n", name, GetLastError());
        return 0;
    }
    dir = (DIR *)malloc(sizeof(DIR));
    if (!dir)
    {
        printf("DIR memory allocate fail\n");
        return 0;
    }
    memset(dir, 0, sizeof(DIR));
    dir->dd_fd = 0; // simulate return
    dir->hFind = hFind;
    return dir;
}
int win_readdir(win_DIR *d, lf_dirent_t *dirent)
{
    HANDLE hFind;
    int i;
    BOOL bf;
    WIN32_FIND_DATA FileData;
    if (!d)
    {
        return 1;
    }
    hFind = d->hFind;
    bf = FindNextFile(hFind, &FileData);
    if (!bf) //fail or end
    {
        return 1;
    }
    for (i = 0; i < 256; i++)
    {
        dirent->d_name[i] = FileData.cFileName[i];
        if (FileData.cFileName[i] == '\0')
            break;
    }
    dirent->d_reclen = i;
    dirent->d_reclen = FileData.nFileSizeLow;
    //check there is file or directory
    if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        dirent->d_type = 1;
    }
    else
    {
        dirent->d_type = 0;
    }
    return 0;
}
int win_closedir(win_DIR *d)
{
    if (!d)
        return -1;
    d->hFind = 0;
    free(d);
    return 0;
}
#else
// # 读取目录
int lfat_readdir(lf_dir_t *dir, lf_dirent_t *dpout)
{
    struct dirent *dp = 0;
    dp = readdir(dir);
    if (dp == 0)
        return 1;
    else
    {
        dpout->d_reclen = dp->d_reclen;
        dpout->d_type = dp->d_type;
        strcpy(dpout->d_name, dp->d_name);
        return 0;
    }
}
#endif
int lfat_size(lfile_t *file_handle)
{
    //获取当前读取文件的位置 进行保存
    unsigned int current_read_position = ftell(file_handle);
    int file_size;
    fseek(file_handle, 0, SEEK_END);
    //获取文件的大小
    file_size = ftell(file_handle);
    //恢复文件原来读取的位置
    fseek(file_handle, current_read_position, SEEK_SET);
    return file_size;
}

#elif FAT_FATFS

typedef struct _lfat_cmd_t
{
    char *cmd;
    uint8_t type; //描述
} lfat_cmd_t;
const lfat_cmd_t lfat_open_cmd[] =
    {
        "r", FA_READ,                                //	    以只读方式打开文件，该文件必须存在。
        "rb", FA_READ,                               //	    以只读方式打开文件，该文件必须存在。
        "r+", FA_READ | FA_WRITE,                    //		以读/写方式打开文件，该文件必须存在。
        "rb+", 0,                                    //		以读/写方式打开一个二进制文件，只允许读/写数据。
        "rt+", 0,                                    //		以读/写方式打开一个文本文件，允许读和写。
        "w", FA_CREATE_ALWAYS | FA_WRITE,            //		打开只写文件，若文件存在则长度清为 0，即该文件内容消失，若不存在则创建该文件。
        "w+", FA_CREATE_ALWAYS | FA_WRITE | FA_READ, //		打开可读/写文件，若文件存在则文件长度清为零，即该文件内容会消失。若文件不存在则建立该文件。
        "a", 0 | FA_WRITE,                           //		以附加的方式打开只写文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾，即文件原先的内容会被保留（EOF 符保留）。
        "a+", 0 | FA_WRITE | FA_READ,                //		以附加方式打开可读/写的文件。若文件不存在，则会建立该文件，如果文件存在，则写入的数据会被加到文件尾后，即文件原先的内容会被保留（原来的 EOF 符不保留）。
        "wb", 0,                                     //		以只写方式打开或新建一个二进制文件，只允许写数据。
        "wb+", 0,                                    //		以读/写方式打开或建立一个二进制文件，允许读和写。
        "wt+", 0,                                    //		以读/写方式打开或建立一个文本文件，允许读写。
        "at+", 0,                                    //		以读/写方式打开一个文本文件，允许读或在文本末追加数据。
        "ab+", 0,                                    //		以读/写方式打开一个二进制文件，允许读或在文件末追加数据。
        "wx", FA_CREATE_NEW | FA_WRITE,              //
        "w+x", FA_CREATE_NEW | FA_WRITE | FA_READ,   //
};
lfile_t *lfat_open(uint8_t *filename, uint8_t *type)
{
    uint8_t len = 0, i = 0;
    uint32_t otype = 0;
    //printf(" %s\r\n",filename);
    lfile_t *fp = lpram_malloc(sizeof(lfile_t)); //开辟FIL字节的内存区域
    if (fp == 0)                                 //内存申请失败.
        return 0;
    for (; i < ARRLENs(lfat_open_cmd); i++)
    {
        if (strcmp(lfat_open_cmd[i].cmd, type) == 0)
        {
            otype = lfat_open_cmd[i].type;
            break;
        }
    }
    LOS_LOCK;
    otype = f_open(fp, (const char *)&filename[2], otype);
    LOS_UNLOCK;
    if (otype == FR_OK) //打开文件-暂时只有a 目录
    {
        return fp;
    }
    else
    {
        lpram_free(fp);
        return 0;
    }
}
uint32_t lfat_read_file(FIL *file, uint8_t *buf, uint32_t sizeofbuf)
{
    static uint32_t BytesReadfile;
    LOS_LOCK;
    f_read(file, buf, sizeofbuf, &BytesReadfile);
    LOS_UNLOCK;
    return BytesReadfile;
}
uint32_t lfat_write_file(FIL *file, uint8_t *buf, uint32_t sizeofbuf)
{
    static uint32_t BytesWritefile;
    LOS_LOCK;
    f_write(file, buf, sizeofbuf, &BytesWritefile);
    LOS_UNLOCK;
    return BytesWritefile;
}
int lfat_close(lfile_t *file)
{
    int ret = 0;
    LOS_LOCK;
    ret = f_close(file);
    LOS_UNLOCK;
    lpram_free(file);
    return ret;
}
//# 打开目录
lf_dir_t *lfat_opendir(char *fname)
{
    FRESULT res;
    DIR *dir;
    dir = lpram_malloc(sizeof(DIR));
    if (dir == 0)
        return 0;
    LOS_LOCK;
    res = f_opendir(dir, &fname[2]); //打开目录指针-暂时只有a 目录
    LOS_UNLOCK;
    if (res == FR_OK)
        return dir;
    else
    {
        lpram_free(dir);
        return 0;
    }
}
// # 读取目录
int lfat_readdir(lf_dir_t *dir, lf_dirent_t *dpout)
{
    FILINFO dp;
    LOS_LOCK;
    int ret = f_readdir(dir, &dp);
    LOS_UNLOCK;
    if (ret != FR_OK || dp.fname[0] == 0)
        return 1;
    else
    {
        if (dp.fattrib & AM_DIR)
            dpout->d_type = LOS_FAT_DIR;
        else
            dpout->d_type = LOS_FAT_FILE;
        strcpy(dpout->d_name, dp.fname);
        return 0;
    }
}

// # 关闭目录
int lfat_closedir(lf_dir_t *dir)
{
    int ret;
    LOS_LOCK;
    ret = f_closedir((DIR *)dir);
    LOS_UNLOCK;
    lpram_free(dir);
    return ret;
}
int lfat_seek(FIL *file, uint32_t offset, int fromwhere)
{
    int ret = 0;
    LOS_LOCK;
    //偏移起始位置：文件头0(SEEK_SET)，当前位置1(SEEK_CUR)，文件尾2(SEEK_END)
    if (fromwhere == 0)
        ret = f_lseek(file, offset);
    else if (fromwhere == 1)
        ret = f_lseek(file, f_tell(file) + offset);
    else
        ret = f_lseek(file, f_size(file) - offset);
    LOS_UNLOCK;
    return ret;
}

/**
 * 判断是否存在
 * */
int lfat_exit(uint8_t *name)
{
    if (f_stat(&name[2], 0) != 0)
        return 1;
    else
        return 0;
}
#endif

/**
 * 遍历文件列表列表 
 */
uint8_t los_fat_list(uint8_t *path)
{
    uint8_t data[128], res;
    lf_dir_t *dir;
    lf_dirent_t dp;
    //printf("open:%s\r\n", path);
    dir = lf_opendir(path); /* Open the directory */
    //printf("res=%d\r\n", res);
    if (dir)
    {
        for (;;)
        {
            //printf("read\r\n");
            res = lf_readdir(dir, &dp); /* Read a directory item */
            if (res)
                break; /* Break on error or end of dir */
            if (dp.d_type == 1)
            { /* It is a directory */
                printf("dir=%s\r\n", dp.d_name);
                sprintf(data, "%s/%s", path, dp.d_name);
                res = los_fat_list(data);
                if (res)
                    break;
            }
            else
            { /* It is a file. */
                printf("file:%s\r\n", dp.d_name);
            }
        }
        lf_closedir(dir);
    }
    return 0;
}
/**
 * 遍历文件列表列表 -只在这个目录
 */
uint8_t los_fat_list_dir(uint8_t *path)
{
    uint8_t res;
    lf_dir_t *dir;
    lf_dirent_t dp;
    dir = lf_opendir(path); /* Open the directory */
    if (dir)
    {
        for (;;)
        {
            //printf("read\r\n");
            res = lf_readdir(dir, &dp); /* Read a directory item */
            if (res)
                break; /* Break on error or end of dir */
            if (dp.d_type == 1)
            { /* It is a directory */
                printf("dir=%s\r\n", dp.d_name);
            }
            else
            { /* It is a file. */
                printf("file:%s\r\n", dp.d_name);
            }
        }
        lf_closedir(dir);
    }
    return 0;
}

//读取文件
//buf_path：路径
//len：长度-字符；类型
//返回：数据
uint8_t *los_fat_read_file(char *buf_path, uint32_t *len)
{
    int ret;
    lfile_t *fp;
    uint8_t *buf;
    len[0] = 0;
    fp = lf_open(buf_path, "rb"); //
    if (fp == 0)
        return NULL;      //err
    len[0] = lf_size(fp); //文件长度
    buf = lpram_malloc(len[0] + 1);
    lf_read(fp, buf, len[0]); //读取
    buf[len[0]] = 0;
    len[0] += 1;
    lf_close(fp); //关闭
    return buf;
}
//写文件
//path：路径
//data:数据
//len：长度
//返回：0 OK
uint32_t los_fat_save_file(char *path, uint8_t *data, uint32_t len)
{
    int ret;
    lfile_t *fp;
    fp = lf_open(path, "w"); //
    if (fp == 0)
        return 1;                  //err
    ret = lf_write(fp, data, len); //读取
    if (ret != 0)
        return 40 + ret; //err
    ret = lf_close(fp);  //关闭
    if (ret != 0)
        ret = 80 + ret; //err
    return ret;
}
#ifdef FAT_FATFS
FATFS fs_mcu;
/**
 * 初始化fatfs
 * */
uint8_t los_fat_init(void)
{
    uint32_t res = 0;

    res = f_mount(&fs_mcu, "0:", 1); //¹ÒÔØFLASH.
    if (res == 0X0D)                 //挂载失败
    {
        printf("Flash Disk Formatting..[%x].\r\n", res);
        res = f_mkfs("0:", 1, 4096); //格式化
        if (res == 0)
        {
            f_setlabel((const TCHAR *)"0:Los VM");  //名字
            printf("Flash Disk Format Finish\r\n"); ////
            f_mkdir("0:/los");                      //新建路径
        }
        else
        {
            printf("Flash Disk Format Error=%d\r\n", res);
        }
    }
    return res;
}
#endif

/**
 *待分离com的数据
 *buf是输入地址，输出信息
len=  comm_arg_deal((char*)com, &(char **)buf);
 */
int comm_arg_deal(char *com, uint8_t ***buf)
{
    uint32_t len = 0, i, num = 0, snum = 0;
    uint8_t flg = 0, data[128];
    *buf = (uint8_t **)malloc(sizeof(uint8_t *) * 100);
    if (*buf == 0)
        return 0;
    len = strlen(com);
    if (com[len] != ' ' && com[len] != '"')
    { //添加结尾
        com[len++] = ' ';
    }
    for (i = 0; i < len;)
    {
        if (com[i] == ' ' && flg == 0)
        { //这是以
            (*buf)[snum] = malloc(num + 1);
            memcpy((*buf)[snum], data, num);
            (*buf)[snum][num] = 0;
            snum++;
            num = 0; //复制完了
            i++;
            while (com[i] == ' ')
                i++;
        }
        else if (com[i] == '"' && num == 0)
        {
            flg = 1;                //标记
            data[num++] = com[i++]; //复制数据
        }
        else if (com[i] == '"' && flg == 1)
        {
            data[num++] = com[i]; //复制数据
            (*buf)[snum] = malloc(num + 1);
            memcpy((*buf)[snum], data, num);
            (*buf)[snum][num] = 0;
            flg = 0; //复制完了
            num = 0;
            snum++;
            i++;
            while (com[i] == ' ')
                i++;
        }
        else
            data[num++] = com[i++]; //复制数据
    }
    return snum;
}

/**
 * 文件名.。带后缀
 */
uint8_t get_filename(char *path, char *name)
{
    int i, j = 0;
    for (i = 0; path[i] != 0; i++)
        if (path[i] == '/' || path[i] == '\\')
            j = i;
    strcpy(name, &path[j + 1]);
    if (j)
        return 0;
    return 1;
}
/*
 *文件的路径--后面没有/的
 */
uint8_t get_dirname(char *path, char *name)
{
    int i, j = 0, len;
    len = strlen(path);
    for (i = 0; len > i; i++)
    {
        if (path[i] == '/' || path[i] == '\\')
            j = i;
    }
    memcpy(name, path, j);
    name[j] = 0;
    if (j)
        return 0;
    return 1;
}
//有路径文件名，不带后缀
uint8_t get_fname(char *path, char *name)
{
    uint8_t flg = 1;
    int i, j = 0, len;
    len = strlen(path);
    for (i = 0; len > i; i++)
    {
        if (path[i] == '.')
        {
            flg = 0;
            j = i;
        }
    }
    memcpy(name, path, j);
    name[j] = 0;
    return flg;
}

//返回文件名后缀
uint8_t get_fname_hx(char *path, char *name)
{
    uint8_t flg = 1;
    int i, j = 0;
    for (i = 0; path[i] != 0; i++)
    {
        if (path[i] == '.')
        {
            flg = 0;
            j = i;
        }
    }
    strcpy(name, &path[j + 1]);
    return flg;
}
