
#ifndef LOS_AUDIO_H_
#define LOS_AUDIO_H_
#include "stdio.h"
#include "stdint.h"
#include "los_fat_config.h"

#ifndef WINDOW
void stop_wav();
#endif
#define AUDIO_BUF_LEN 512
#define AUDIO_BUF_LEN2 AUDIO_BUF_LEN * 2 //缓存长度
typedef struct _laudio_t
{
    lfile_t *file; //这是文件
    uint16_t buf1[AUDIO_BUF_LEN];
    uint16_t buf2[AUDIO_BUF_LEN];
    uint16_t len1;
    uint16_t len2; //这是缓冲大小，为0就停止
    uint8_t flg;   //那个buf先开始
    uint8_t type;  //这是几个bit类型----低2位
    uint8_t at;    //第3位---声道
    uint8_t speed; //这是速率；单位是K

    uint32_t call_back; //这是los注册了的函数，回调
} laudio_t;

/**
 * 读取数据
 * */
uint8_t paly_ing(void);
void paly_buf(uint8_t type);
/**
 * 这是调用函数
 * filename：打开的文件
 * cback：播放完，回调
 * */
uint8_t los_paly_wav(uint8_t *filename, uint32_t cback);
void los_paly_wav_stop(void); //停止
#endif
