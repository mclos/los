/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/
#ifdef WINDOW
#include "los_audio.h"
#include "los_ram.h"
#include "windows.h"
#include <mmsystem.h>
 
laudio_t *laudio;
/**
 * 这是调用函数
 * */
uint8_t los_paly_wav(uint8_t *filename, uint32_t cback)
{
    int res;
    if (laudio)  
    {
        memset(laudio, 0, sizeof(laudio_t));
        PlaySound(filename, NULL, SND_FILENAME | SND_ASYNC);
    }
    else
    {
        laudio = lpram_malloc(sizeof(laudio_t));  
        if (laudio == 0)
            return 2;
        memset(laudio, 0, sizeof(laudio_t));

        PlaySound(filename, NULL, SND_FILENAME | SND_ASYNC);
    }
    return 0;
}
/**
 * 清空buf 
 */
void paly_buf(uint8_t type)
{
}
/**
 * 停止播放
 * */
void los_paly_wav_stop(void)
{
    lpram_free(laudio);
    laudio = 0;
    PlaySound(NULL,NULL,SND_FILENAME);
}
/**
 * 读取数据
 * */
uint8_t paly_ing(void)
{

    return 0;
}
#endif
