/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/
#ifndef WINDOW
#include "los_audio.h"
#include "lpram.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
extern TIM_HandleTypeDef htim4;
extern DMA_HandleTypeDef hdma_tim5_up;
extern TIM_HandleTypeDef htim5;

 
laudio_t *laudio = {0};

typedef struct WaveHeader
{
    uint8_t riff[4];      //资源交换文件标志;
    uint32_t size;        //从下个地址开始到文件结尾的字节数;
    uint8_t wave_flag[4]; //wave文件标识;
    uint8_t fmt[4];       //波形格式标识 ;
    uint32_t fmt_len;     //过滤字节(一般为00000010H) ;
    uint16_t tag;         //格式种类，值为1时，表示PCM线性编码 ;
    uint16_t channels;    //通道数，单声道为1，双声道为2 ;
    uint32_t samp_freq;   //采样频率 ;
    uint32_t byte_rate;   //数据传输率 (每秒字节＝采样频率×每个样本字节数) ;
    uint16_t block_align; //块对齐字节数 = channles * bit_samp / 8 ;
    uint16_t bit_samp;    //bits per sample (又称量化位数) ;
    uint32_t a;
    uint32_t len;
} wave_header_t;
/**
 * 这是调用函数
 * filename：打开的文件
 * cback：播放完，回调
 * */
uint8_t los_paly_wav(uint8_t *filename, uint32_t cback)
{
    int res;
    if (laudio) //已经有在播放了
    {
        los_paly_wav_stop();
    }
    laudio = lpram_malloc(sizeof(laudio_t)); //申请内存
    if (laudio == 0)
        return 2;
    memset(laudio, 0, sizeof(laudio_t));
    laudio->file = lf_open((const TCHAR *)filename, "rb"); //打开文件
    if (laudio->file == 0)
    {
        lpram_free(laudio);
        return 3;
    }                             //打开文件成功
    lf_seek(laudio->file, 44, 0); //直接向前44字节
    laudio->type = 0;             //8声道
    res = paly_ing();
    res |= paly_ing();
    if (res)
    {
        lpram_free(laudio);
        return 4;
    }
    HAL_DMA_Start_IT(htim5.hdma[TIM_DMA_ID_UPDATE], laudio->buf1, (uint32_t)(&htim4.Instance->CCR1), laudio->len2 + laudio->len1); //
    __HAL_TIM_ENABLE_DMA(&htim5, TIM_DMA_UPDATE);
    HAL_TIM_Base_Start(&htim5);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_GPIO_WritePin(CA_GPIO_Port, CA_Pin, 0);

    return 0;
}
/**
 * 清空buf flg
 */
void paly_buf(uint8_t type)
{
    if (laudio == 0)
        return;
    if (type == 0)
    {
        laudio->len1 = 0;
        laudio->flg &= 0xfe;
    }
    else
    {
        laudio->len2 = 0;
        laudio->flg &= 0xfD;
    }
}
void stop_wav()
{
    htim4.Instance->CCR1 = 128;
    HAL_GPIO_WritePin(CA_GPIO_Port, CA_Pin, 1);
    HAL_TIM_Base_Stop(&htim5);
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
}
/**
 * 停止播放
 * */
void los_paly_wav_stop(void)
{
    stop_wav();
    f_close(laudio->file);
    lpram_free(laudio);
    laudio = 0;
}
/**
 * 读取数据
 * */
uint8_t paly_ing(void)
{
    uint32_t br;
    uint8_t *read_buf, type = 0;
    uint16_t *p, len, i; //
    if (laudio == 0)     //已经有在播放了
        return 1;
    if (laudio->type == 0)
        len = AUDIO_BUF_LEN;
    else
        len = AUDIO_BUF_LEN2; //16bit
    if (laudio->at == 1)
        len *= 2; //2声道
    read_buf = lpram_malloc(len);
    if (read_buf == 0)
        return 1;
    if ((laudio->flg & 1) == 0) //buf1 缺失
    {
        p = laudio->buf1;
    }
    else
    { 
        type = 1;
        p = laudio->buf2;
    }

    br = lf_read(laudio->file, read_buf, len); //读出readlen个字节
    if (br == 0) //结束了-也可以回调了
    {
        br = laudio->call_back;
        los_paly_wav_stop();
        if (br) //有回调了
        {
        }
        return 10;
    }
    br /= ((laudio->type + 1) * (laudio->at + 1));
    if (type == 0) //长度
    {
        laudio->flg |= 0x1;
        laudio->len1 = br;
    }
    else
    {
        laudio->len2 = br;
        laudio->flg |= 0x2;
    }
    if (laudio->type == 0) //8bit
    {
        for (i = 0; i < br; i++)
        {
            p[i] = (read_buf[i]);
        }
    }
    lpram_free(read_buf);
    return 0;
}
#endif
